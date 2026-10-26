#ifdef Q_OS_WIN32

#include "windows.hpp"

#include <Windows.h>
#include <ShlObj.h>
#include <QDir>

bool windows::createShortcut(
	const QString &shortcutPath,
	const QString &targetPath,
	const QString &arguments,
	const QString &workingDirectory,
	const QString &iconPath)
{
	HRESULT	   hr			= CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	const bool uninitialize = SUCCEEDED(hr);

	if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
		return false;

	IShellLinkW *shellLink = nullptr;

	hr = CoCreateInstance(
		CLSID_ShellLink,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IShellLinkW,
		reinterpret_cast<void **>(&shellLink));

	if (FAILED(hr)) {
		if (uninitialize)
			CoUninitialize();
		return false;
	}

	shellLink->SetPath(reinterpret_cast<LPCWSTR>(
		targetPath.utf16()));

	if (!arguments.isEmpty())
		shellLink->SetArguments(reinterpret_cast<LPCWSTR>(
			arguments.utf16()));

	if (!workingDirectory.isEmpty())
		shellLink->SetWorkingDirectory(
			reinterpret_cast<LPCWSTR>(workingDirectory.utf16()));

	if (!iconPath.isEmpty())
		shellLink->SetIconLocation(
			reinterpret_cast<LPCWSTR>(iconPath.utf16()), 0);

	IPersistFile *persistFile = nullptr;
	hr						  = shellLink->QueryInterface(
		   IID_IPersistFile,
		   reinterpret_cast<void **>(&persistFile));

	if (SUCCEEDED(hr)) {
		hr = persistFile->Save(
			reinterpret_cast<LPCOLESTR>(shortcutPath.utf16()),
			TRUE);
		persistFile->Release();
	}

	shellLink->Release();

	if (uninitialize)
		CoUninitialize();

	return SUCCEEDED(hr);
}

#endif
