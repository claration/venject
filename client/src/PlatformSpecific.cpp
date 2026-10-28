#include "PlatformSpecific.hpp"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <shlobj.h>
#endif

namespace platform {

#if defined(Q_OS_LINUX) || defined(Q_OS_WIN32)

bool createShortcut(
	const QString &name,
	const QString &fileName,
	const QString &targetExecutablePath,
	const QString &targetArguments,
	const QString &iconPath)
{

#ifdef Q_OS_LINUX
	if (fileName.isEmpty() || targetExecutablePath.isEmpty()) return false;

	QString appsDir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
	if (appsDir.isEmpty()) return false;

	QDir().mkpath(appsDir);

	QString fullPath = appsDir + "/" + "vencord-launcher-" + fileName + ".desktop";

	QFile file(fullPath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

	QTextStream out(&file);
	out << "[Desktop Entry]\n"
		<< "Type=Application\n"
		<< "Name=" << name << "\n"
		<< "Exec=" << targetExecutablePath << (targetArguments.isEmpty() ? "" : " " + targetArguments) << "\n";

	if (!iconPath.isEmpty()) out << "Icon=" << iconPath << "\n";

	out << "Terminal=false\n"
		<< "Categories=Network;\n";
	file.close();

	QFile::setPermissions(
		fullPath,
		QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadUser | QFile::ExeUser
	);
	return true;
#elif defined(Q_OS_WIN32)
	if (name.isEmpty() || targetExecutablePath.isEmpty()) return false;

	PWSTR path = nullptr;
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Programs, 0, nullptr, &path);
	if (FAILED(hr)) return false;

	QDir programsDir(QString::fromWCharArray(path));
	CoTaskMemFree(path);

	if (!programsDir.mkpath("VencordLauncher")) return false;

	QString fullPath = programsDir.absoluteFilePath("VencordLauncher/Vencord Launcher - " + name + ".lnk");

	hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	const bool uninitialize = SUCCEEDED(hr);
	if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) return false;

	IShellLinkW *shellLink = nullptr;
	hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
	if (FAILED(hr)) {
		if (uninitialize) CoUninitialize();
		return false;
	}

	shellLink->SetPath(reinterpret_cast<LPCWSTR>(targetExecutablePath.utf16()));

	if (!targetArguments.isEmpty()) {
		shellLink->SetArguments(reinterpret_cast<LPCWSTR>(targetArguments.utf16()));
	}

	if (!iconPath.isEmpty()) {
		shellLink->SetIconLocation(reinterpret_cast<LPCWSTR>(iconPath.utf16()), 0);
	}

	IPersistFile *persistFile = nullptr;
	hr = shellLink->QueryInterface(IID_PPV_ARGS(&persistFile));
	if (SUCCEEDED(hr)) {
		hr = persistFile->Save(fullPath.toStdWString().c_str(), TRUE);
		persistFile->Release();
	} else {
		hr = E_FAIL;
	}

	shellLink->Release();
	if (uninitialize) CoUninitialize();

	return SUCCEEDED(hr);
#endif
}

bool deleteShortcut(const QString &fileName)
{
#ifdef Q_OS_LINUX
	if (fileName.isEmpty()) return false;

	QString appsDir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
	if (appsDir.isEmpty()) return false;

	QString fullPath = appsDir + "/" + "vencord-launcher-" + fileName + ".desktop";

	QFile file(fullPath);
	if (file.exists()) return file.remove();

	return true;

#elif defined(Q_OS_WIN32)
	if (fileName.isEmpty()) return false;

	PWSTR path = nullptr;
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Programs, 0, nullptr, &path);
	if (FAILED(hr)) return false;

	QString fullPath = QString::fromWCharArray(path) + "\\VencordLauncher\\Vencord Launcher - " + fileName + ".lnk";
	CoTaskMemFree(path);

	return QFile::remove(fullPath);
#endif
}

#endif

}
