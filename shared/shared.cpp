#include "shared.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#ifdef _WIN32
#include <windows.h>
#else				// _WIN32
#include <limits.h> // PATH_MAX, setbuffer
#include <unistd.h> // readlink
#endif				// _WIN32

namespace fs = std::filesystem;

namespace shared
{

fs::path OurExecutablePath()
{
#ifdef _WIN32
	std::wstring path(256, L'\0');

	for (;;) {
		const auto len = GetModuleFileNameW(
			nullptr,
			path.data(),
			static_cast<DWORD>(path.size()));

		if (len == 0)
			return {};

		if (len < path.size() - 1) {
			path.resize(len);
			return fs::path(path);
		}

		path.resize(path.size() * 2);
	}
#else
#if __linux__
	if (const char* appImageEnv = std::getenv("APPIMAGE")) {
		if (appImageEnv[0] != '\0') {
			return fs::path(appImageEnv);
		}
	}
#endif
	char buffer[PATH_MAX];

	const auto len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	if (len == -1)
		return {};

	buffer[len] = '\0';

	return fs::path(buffer);
#endif
}

fs::path OurHookLibraryPath()
{
#ifdef _WIN32
	return OurExecutablePath().parent_path() / "libvencord_hook.dll";
#elif __APPLE__
	return OurExecutablePath().parent_path() / "libvencord_hook.dylib";
#else
	if (const char* appDirEnv = std::getenv("APPDIR")) {
		if (appDirEnv[0] != '\0') {
			return fs::path(appDirEnv) / "bin/libvencord_hook.so";
		}
	}
	return OurExecutablePath().parent_path() / "libvencord_hook.so";
#endif
}


} // namespace shared
