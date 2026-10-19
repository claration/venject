#ifdef __APPLE__
#error "Apple is not supported"
#elif defined(__linux__)

#include <cstdarg> // va
#include <dlfcn.h>
#include <fcntl.h> // O_CREAT
#include <string>

#include <dobby.h>

#include "globals.h"
#include "utils.hpp"

namespace
{

template <typename T>
T resolve(const char *name)
{
	return reinterpret_cast<T>(dlsym(RTLD_NEXT, name));
}

using uv_fs_lstat_t = int (*)(const void *, const void *, const char *, void *);
using xstat64_t		= int (*)(int, const char *, struct stat64 *);
using open64_t		= int (*)(const char *, int, ...);
using openat64_t	= int (*)(int, const char *, int, ...);

uv_fs_lstat_t original_uv_fs_lstat = nullptr;
xstat64_t	  original_xstat64	   = nullptr;
open64_t	  original_open64	   = nullptr;
openat64_t	  original_openat64	   = nullptr;

int uv_fs_lstat(const void *loop, const void *req, const char *path, void *buf)
{
	auto redirected = utils::redirect(path);

	return original_uv_fs_lstat(
		loop,
		req,
		redirected.empty() ? path : redirected.c_str(),
		buf);
}

extern "C" 
int __xstat64(int version, const char *path, struct stat64 *out)
{
	if (!original_xstat64)
		original_xstat64 = resolve<xstat64_t>("__xstat64");

	auto redirected = utils::redirect(path);

	return original_xstat64(
		version,
		redirected.empty() ? path : redirected.c_str(),
		out);
}

extern "C" 
int open64(const char *path, int flags, ...)
{
	if (!original_open64)
		original_open64 = resolve<open64_t>("open64");

	auto redirected = utils::redirect(path);

	const char *target = redirected.empty()
							 ? path
							 : redirected.c_str();

	if (!(flags & O_CREAT))
		return original_open64(target, flags);

	va_list args;
	va_start(args, flags);

	const auto mode = va_arg(args, int);

	const auto result =
		original_open64(target, flags, mode);

	va_end(args);

	return result;
}

extern "C" 
int openat64(int dirfd, const char *path, int flags, ...)
{
	if (!original_openat64)
		original_openat64 = resolve<openat64_t>("openat64");

	auto redirected = utils::redirect(path);

	const char *target = redirected.empty()
							 ? path
							 : redirected.c_str();

	if (!(flags & O_CREAT))
		return original_openat64(dirfd, target, flags);

	va_list args;
	va_start(args, flags);

	const auto mode = va_arg(args, int);

	const auto result = original_openat64(
		dirfd,
		target,
		flags,
		mode);

	va_end(args);

	return result;
}

__attribute__((constructor)) 
void init()
{
	void *target = dlsym(
		RTLD_DEFAULT,
		"uv_fs_lstat");

	if (!target)
		return;

	DobbyHook(
		target,
		reinterpret_cast<void *>(&uv_fs_lstat),
		reinterpret_cast<void **>(&original_uv_fs_lstat));
}

} // namespace

#else // _WIN32

#include <string>
#include <string_view>
#include <filesystem>
#include <iostream>

#include <windows.h>
#include <detours.h>

#include "utils.hpp"

namespace fs = std::filesystem;

using get_file_attributes_w_t = DWORD (*) (LPCWSTR);
using create_file_w_t = HANDLE (*) (LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
using move_file_ex_w_t = BOOL (*) (LPCWSTR, LPCWSTR, DWORD);
using create_process_w_t = WINBOOL (*) (LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);

static get_file_attributes_w_t original_get_file_attributes_w_t = nullptr;
static create_file_w_t original_create_file_w = nullptr;
static move_file_ex_w_t original_move_file_ex_w = nullptr;
static create_process_w_t original_create_process_w = nullptr;

#define ERROR_HOOKING_MSG(msg) \
MessageBoxA(               \
                           NULL,                  \
                           (msg),                 \
                           "Error Hooking",       \
                           MB_OK                  \
    )

#define ATTACH(orig, target)                                      \
{                                                          \
		LONG result = DetourAttach(&(PVOID&)orig, (PVOID)(target)); \
        if (result != 0) {                                        \
            ERROR_HOOKING_MSG("Failed to hook " #orig             \
                              ". Please report this issue.");     \
            DetourTransactionAbort();                             \
            return 1;                                             \
    }                                                         \
}

std::string lpcwstr_to_string(LPCWSTR lpcwszStr)
{
	int strLength = WideCharToMultiByte(CP_UTF8, 0, lpcwszStr, -1, nullptr, 0, nullptr, nullptr);
	std::string str(strLength, 0);

	WideCharToMultiByte(CP_UTF8, 0, lpcwszStr, -1, &str[0], strLength, nullptr, nullptr);

	return str;
}

fs::path get_dll_path()
{
	std::string dll_path_str{std::getenv("HOOK_DLL_PATH")};
	return dll_path_str;
}

extern "C" DWORD get_file_attributes_w(LPCWSTR lp_file_name)
{
	if (lp_file_name)
	{
		return original_get_file_attributes_w_t(lp_file_name);
	}

	std::string str = lpcwstr_to_string(lp_file_name);
	auto redirected = utils::redirect(str.c_str());

	if (!redirected.empty())
	{
		std::wstring wstring{redirected.begin(), redirected.end()};
		return original_get_file_attributes_w_t(wstring.c_str());
	}
	else
	{
		return original_get_file_attributes_w_t(lp_file_name);
	}
}

extern "C" HANDLE create_file_w(LPCWSTR lp_file_name, DWORD dw_desired_access, DWORD dw_share_mode, LPSECURITY_ATTRIBUTES lp_security_attributes, DWORD dw_creation_disposition, DWORD dw_flags_and_attributes, HANDLE h_template_file)
{
	if (lp_file_name)
	{
		return original_create_file_w(lp_file_name, dw_desired_access, dw_share_mode, lp_security_attributes, dw_creation_disposition, dw_flags_and_attributes, h_template_file);
	}

	std::string str = lpcwstr_to_string(lp_file_name);
	auto redirected = utils::redirect(str.c_str());

	if (!redirected.empty())
	{
		std::wstring wstring{redirected.begin(), redirected.end()};
		return original_create_file_w(wstring.c_str(), dw_desired_access, dw_share_mode, lp_security_attributes, dw_creation_disposition, dw_flags_and_attributes, h_template_file);
	}
	else
	{
		return original_create_file_w(lp_file_name, dw_desired_access, dw_share_mode, lp_security_attributes, dw_creation_disposition, dw_flags_and_attributes, h_template_file);
	}
}

extern "C" BOOL move_file_ex_w(LPCWSTR lp_existing_file, LPCWSTR lp_new_file_name, DWORD dw_flags)
{
	std::wstring new_file_name{lp_new_file_name};

	std::wstring_view app_asar_str{L"\\_app.asar"};
	std::wstring::size_type app_asar_pos = new_file_name.find(app_asar_str);
	if (app_asar_pos != std::wstring::npos)
	{
		std::wstring redirect_to = new_file_name.replace(app_asar_pos, app_asar_str.size(), L"\\app.asar");
		return original_move_file_ex_w(lp_existing_file, redirect_to.c_str(), dw_flags);
	}

	return original_move_file_ex_w(lp_existing_file, lp_new_file_name, dw_flags);
}

extern "C" BOOL create_process_w(LPCWSTR lp_application_name, LPWSTR lp_command_line, LPSECURITY_ATTRIBUTES lp_process_attributes, LPSECURITY_ATTRIBUTES lp_thread_attributes, BOOL b_inherit_handles, DWORD dw_creation_flags, LPVOID lp_environment, LPCWSTR lp_current_directory, LPSTARTUPINFOW lp_startup_info, LPPROCESS_INFORMATION lp_process_information)
{
	if (!lp_command_line)
	{
		return original_create_process_w(lp_application_name, lp_command_line, lp_process_attributes, lp_thread_attributes, b_inherit_handles, dw_creation_flags, lp_environment, lp_current_directory, lp_startup_info, lp_process_information);
	}

	std::wstring command_line{lp_command_line};
	if (command_line.contains(L"--") && !command_line.contains(L"--type=renderer"))
	{
		return original_create_process_w(lp_application_name, lp_command_line, lp_process_attributes, lp_thread_attributes, b_inherit_handles, dw_creation_flags, lp_environment, lp_current_directory, lp_startup_info, lp_process_information);
	}

	std::string dll_path = get_dll_path().string();

	BOOL success = DetourCreateProcessWithDllW(
		lp_application_name,
		lp_command_line,
		lp_process_attributes,
		lp_thread_attributes,
		b_inherit_handles,
		dw_creation_flags,
		lp_environment,
		lp_current_directory,
		lp_startup_info,
		lp_process_information,
		dll_path.c_str(),
		original_create_process_w
		);

	if (success != 1)
	{
		std::cerr << "[venject] Failed to create process" << std::endl;
		return success;
	}

	ResumeThread(lp_process_information->hThread);
	return success;
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fwdReason, LPVOID)
{
    if (DetourIsHelperProcess())
    {
        return TRUE;
    }

    if (fwdReason != DLL_PROCESS_ATTACH)
    {
        return 1;
    }

	original_get_file_attributes_w_t = GetFileAttributesW;
	original_create_file_w = CreateFileW;
	original_move_file_ex_w = MoveFileExW;
	original_create_process_w = CreateProcessW;

    DetourRestoreAfterWith();

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

#ifdef _DEBUG
    AllocConsole();
#endif

	ATTACH(GetFileAttributesW, get_file_attributes_w)
	ATTACH(CreateFileW, create_file_w)
	ATTACH(MoveFileExW, move_file_ex_w)
	ATTACH(CreateProcessW, create_process_w)

	DetourTransactionCommit();

    return TRUE;
}

// TODO: use detours instead of dobby for windows
// use this as a reference https://github.com/MeguminSama/Electron-Hook/blob/main/src/windows/hooks.rs

#endif
