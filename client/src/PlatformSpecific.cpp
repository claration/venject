#include "PlatformSpecific.hpp"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <shlobj.h>
#endif

namespace platform {

#if defined(Q_OS_LINUX) || defined(Q_OS_WIN32)

QString createShortcut(
	const QString &name,
	const QString &fileName,
	const QString &targetExecutablePath,
	const QString &targetArguments,
	const QString &iconPath,
	ShortcutLocation location)
{

#ifdef Q_OS_LINUX
	if (location == ShortcutLocation::Autostart) {
		return QString(); // Autostart is Windows only
	}
	if (targetExecutablePath.isEmpty()) return QString();

	QString targetDir;
	QString baseName = fileName.isEmpty() ? name : fileName;

	if (location == ShortcutLocation::Applications) {
		targetDir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
	} else if (location == ShortcutLocation::Desktop) {
		targetDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	} else if (location == ShortcutLocation::Custom) {
		targetDir = QDir::tempPath();
		if (baseName.isEmpty()) baseName = "vencord-launcher-shortcut";
	}

	if (targetDir.isEmpty()) return QString();
	QDir().mkpath(targetDir);

	QString fullPath;
	if (location == ShortcutLocation::Custom) {
		fullPath = targetDir + "/" + baseName + ".desktop";
	} else {
		fullPath = targetDir + "/vencord-launcher-" + baseName + ".desktop";
	}

	QFile file(fullPath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return QString();

	QTextStream out(&file);
	out << "[Desktop Entry]\n"
		<< "Type=Application\n"
		<< "Name=" << "Vencord Launcher - " << name << "\n"
		<< "Exec=" << "env __VENCORDLAUNCHER_MODE_CLI=1 \"" << targetExecutablePath << "\""
		<< (targetArguments.isEmpty() ? "" : " " + targetArguments) << "\n";

	if (!iconPath.isEmpty()) out << "Icon=" << iconPath << "\n";

	out << "Terminal=false\n"
		<< "Categories=Network;\n";
	file.close();

	QFile::setPermissions(
		fullPath,
		QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadUser | QFile::ExeUser
	);
	return fullPath;

#elif defined(Q_OS_WIN32)
	if (name.isEmpty() || targetExecutablePath.isEmpty()) return QString();

	PWSTR path = nullptr;
	HRESULT hr = E_FAIL;
	QString fullPath;
	QString subDir;

	if (location == ShortcutLocation::Custom) {
		QString baseName = fileName.isEmpty() ? name : fileName;
		QString tempDir = QDir::tempPath();
		fullPath = tempDir + "/" + baseName + ".lnk";
		fullPath = QDir::toNativeSeparators(fullPath);
	} else {
		if (location == ShortcutLocation::Applications) {
			hr = SHGetKnownFolderPath(FOLDERID_Programs, 0, nullptr, &path);
			subDir = "VencordLauncher";
		} else if (location == ShortcutLocation::Desktop) {
			hr = SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &path);
		} else if (location == ShortcutLocation::Autostart) {
			hr = SHGetKnownFolderPath(FOLDERID_Startup, 0, nullptr, &path);
		}

		if (FAILED(hr) || !path) return QString();

		QDir dir(QString::fromWCharArray(path));
		CoTaskMemFree(path);

		if (!subDir.isEmpty()) {
			dir.mkpath(subDir);
		}

		QString dirPath = subDir.isEmpty() ? dir.absolutePath() : dir.absoluteFilePath(subDir);
		fullPath = dirPath + "/Vencord Launcher - " + name + ".lnk";
	}

	hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	const bool uninitialize = SUCCEEDED(hr);
	if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) return QString();

	IShellLinkW *shellLink = nullptr;
	hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
	if (FAILED(hr)) {
		if (uninitialize) CoUninitialize();
		return QString();
	}

	shellLink->SetPath(L"C:\\Windows\\System32\\cmd.exe");

	QString cmdArgs = QString("/c set \"__VENCORDLAUNCHER_MODE_CLI=1\" && start /b \"\" \"%1\"%2")
		.arg(targetExecutablePath)
		.arg(targetArguments.isEmpty() ? "" : " " + targetArguments);

	shellLink->SetArguments(reinterpret_cast<LPCWSTR>(cmdArgs.utf16()));
	shellLink->SetShowCmd(SW_HIDE);

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

	return SUCCEEDED(hr) ? fullPath : QString();
#endif
}

bool deleteShortcut(const QString &fileName)
{
	if (fileName.isEmpty()) return false;
	bool deletedAny = false;

#ifdef Q_OS_LINUX
	// Applications
	QString appsDir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
	if (!appsDir.isEmpty()) {
		QString appPath = appsDir + "/vencord-launcher-" + fileName + ".desktop";
		QFile file(appPath);
		if (file.exists()) {
			if (file.remove()) deletedAny = true;
		}
	}

	// Desktop
	QString desktopDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	if (!desktopDir.isEmpty()) {
		QString desktopPath = desktopDir + "/vencord-launcher-" + fileName + ".desktop";
		QFile file(desktopPath);
		if (file.exists()) {
			if (file.remove()) deletedAny = true;
		}
	}

#elif defined(Q_OS_WIN32)
	PWSTR path = nullptr;
	HRESULT hr = S_OK;

	// Applications folder
	hr = SHGetKnownFolderPath(FOLDERID_Programs, 0, nullptr, &path);
	if (SUCCEEDED(hr) && path) {
		QString appPath = QString::fromWCharArray(path) + "\\VencordLauncher\\Vencord Launcher - " + fileName + ".lnk";
		CoTaskMemFree(path);
		path = nullptr;
		if (QFile::exists(appPath) && QFile::remove(appPath)) deletedAny = true;
	}

	// Desktop folder
	hr = SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &path);
	if (SUCCEEDED(hr) && path) {
		QString desktopPath = QString::fromWCharArray(path) + "\\Vencord Launcher - " + fileName + ".lnk";
		CoTaskMemFree(path);
		path = nullptr;
		if (QFile::exists(desktopPath) && QFile::remove(desktopPath)) deletedAny = true;
	}

	// Autostart folder
	hr = SHGetKnownFolderPath(FOLDERID_Startup, 0, nullptr, &path);
	if (SUCCEEDED(hr) && path) {
		QString startupPath = QString::fromWCharArray(path) + "\\Vencord Launcher - " + fileName + ".lnk";
		CoTaskMemFree(path);
		path = nullptr;
		if (QFile::exists(startupPath) && QFile::remove(startupPath)) deletedAny = true;
	}
#endif

	return deletedAny;
}

#endif

}
