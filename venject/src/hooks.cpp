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

	const auto result = original_open64(target, flags, mode);

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
	// uv_fs_lstat needs special treatment, vencord uses it
	void *target = dlsym(RTLD_DEFAULT, "uv_fs_lstat");
	if (!target) return;

	DobbyHook(
		target,
		reinterpret_cast<void *>(&uv_fs_lstat),
		reinterpret_cast<void **>(&original_uv_fs_lstat)
	);
}

} // namespace

#else // _WIN32

#include <cstdlib>
#include <string>
#include <filesystem>
#include <iostream>

#include <windows.h>
#include <detours.h>

#include "utils.hpp"
#include "../../shared/shared.hpp" // OurExecutablePath

namespace fs = std::filesystem;

using get_file_attributes_w_t 	= DWORD   (*) (LPCWSTR);
using create_file_w_t 			= HANDLE  (*) (LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
using move_file_ex_w_t 			= BOOL 	  (*) (LPCWSTR, LPCWSTR, DWORD);
using create_process_w_t 		= WINBOOL (*) (LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);
using uv_fs_lstat_t 			= int (*) (const void *, const void *, const char *, void *);

static get_file_attributes_w_t 	original_get_file_attributes_w_t 	= nullptr;
static create_file_w_t 			original_create_file_w 				= nullptr;
static move_file_ex_w_t 		original_move_file_ex_w 			= nullptr;
static create_process_w_t 		original_create_process_w 			= nullptr;
static uv_fs_lstat_t 			original_uv_fs_lstat 				= nullptr;

#define ERROR_HOOKING_MSG(msg) \
MessageBoxA(               \
                           NULL,                  \
                           (msg),                 \
                           "Error Hooking",       \
                           MB_OK                  \
    )

#define ATTACH(orig, target)                                      	\
{                                                          			\
		LONG result = DetourAttach(&(PVOID&)orig, (PVOID)(target)); \
        if (result != 0) {                                        	\
            ERROR_HOOKING_MSG("Failed to hook " #orig             	\
                              ". Please report this issue.");     	\
            DetourTransactionAbort();                             	\
            return FALSE;                                           \
    }                                                         		\
}

std::string lpcwstr_to_string(LPCWSTR lpcwszStr)
{
	if (!lpcwszStr) return {};

	int strLength = WideCharToMultiByte(CP_UTF8, 0, lpcwszStr, -1, nullptr, 0, nullptr, nullptr);
	if (strLength <= 0) return {};

	std::string str(static_cast<size_t>(strLength), 0);
	WideCharToMultiByte(CP_UTF8, 0, lpcwszStr, -1, str.data(), strLength, nullptr, nullptr);
	str.resize(static_cast<size_t>(strLength) - 1);
	return str;
}

std::wstring utf8_to_wstring(const std::string &utf8)
{
	if (utf8.empty()) return {};

	int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
	if (wlen <= 0) return {};

	std::wstring wstr(static_cast<size_t>(wlen), 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, wstr.data(), wlen);
	wstr.resize(static_cast<size_t>(wlen) - 1);
	return wstr;
}

fs::path get_dll_path()
{
	const char *env = std::getenv("HOOK_DLL_PATH");
	if (!env || env[0] == 0) return {};

	return fs::path(env);
}

extern "C" DWORD get_file_attributes_w(LPCWSTR lp_file_name)
{
	if (!lp_file_name) 
		return original_get_file_attributes_w_t(lp_file_name);

	std::string str = lpcwstr_to_string(lp_file_name);
	auto redirected = utils::redirect(str.c_str());

	if (!redirected.empty())
	{
		std::wstring redirected_w = utf8_to_wstring(redirected);
		if (!redirected_w.empty())
			return original_get_file_attributes_w_t(redirected_w.c_str());
	}

	return original_get_file_attributes_w_t(lp_file_name);
}

extern "C" HANDLE create_file_w(LPCWSTR lp_file_name, DWORD dw_desired_access, DWORD dw_share_mode, LPSECURITY_ATTRIBUTES lp_security_attributes, DWORD dw_creation_disposition, DWORD dw_flags_and_attributes, HANDLE h_template_file)
{
	if (!lp_file_name)
	{
		return original_create_file_w(lp_file_name, dw_desired_access, dw_share_mode, lp_security_attributes, dw_creation_disposition, dw_flags_and_attributes, h_template_file);
	}

	std::string str = lpcwstr_to_string(lp_file_name);
	auto redirected = utils::redirect(str.c_str());

	if (!redirected.empty())
	{
		std::wstring redirected_w = utf8_to_wstring(redirected);
		if (!redirected_w.empty())
			return original_create_file_w(redirected_w.c_str(), dw_desired_access, dw_share_mode, lp_security_attributes, dw_creation_disposition, dw_flags_and_attributes, h_template_file);
	}

	return original_create_file_w(lp_file_name, dw_desired_access, dw_share_mode, lp_security_attributes, dw_creation_disposition, dw_flags_and_attributes, h_template_file);
}

extern "C" BOOL move_file_ex_w(LPCWSTR lp_existing_file, LPCWSTR lp_new_file_name, DWORD dw_flags)
{
	LPCWSTR existing_target = lp_existing_file;
	LPCWSTR new_target = lp_new_file_name;
	std::wstring existing_redirected_w;
	std::wstring new_redirected_w;

	if (lp_existing_file)
	{
		auto redirected = utils::redirect(lpcwstr_to_string(lp_existing_file).c_str());
		if (!redirected.empty())
		{
			existing_redirected_w = utf8_to_wstring(redirected);
			if (!existing_redirected_w.empty())
				existing_target = existing_redirected_w.c_str();
		}
	}

	if (lp_new_file_name)
	{
		auto redirected = utils::redirect(lpcwstr_to_string(lp_new_file_name).c_str());
		if (!redirected.empty())
		{
			new_redirected_w = utf8_to_wstring(redirected);
			if (!new_redirected_w.empty())
				new_target = new_redirected_w.c_str();
		}
	}

	return original_move_file_ex_w(existing_target, new_target, dw_flags);
}

extern "C" BOOL create_process_w(LPCWSTR lp_application_name, LPWSTR lp_command_line, LPSECURITY_ATTRIBUTES lp_process_attributes, LPSECURITY_ATTRIBUTES lp_thread_attributes, BOOL b_inherit_handles, DWORD dw_creation_flags, LPVOID lp_environment, LPCWSTR lp_current_directory, LPSTARTUPINFOW lp_startup_info, LPPROCESS_INFORMATION lp_process_information)
{
	if (!lp_command_line)
	{
		return original_create_process_w(lp_application_name, lp_command_line, lp_process_attributes, lp_thread_attributes, b_inherit_handles, dw_creation_flags, lp_environment, lp_current_directory, lp_startup_info, lp_process_information);
	}

	std::wstring command_line{lp_command_line};

	// When the updater "restarts" Discord, it doesn't seem to pass any arguments to the process.
    // So we can just check if the command contains "--" to make sure we hook the new Discord instance.
	if (command_line.find(L"--") != std::wstring::npos && command_line.find(L"--type=renderer") == std::wstring::npos)
	{
		return original_create_process_w(lp_application_name, lp_command_line, lp_process_attributes, lp_thread_attributes, b_inherit_handles, dw_creation_flags, lp_environment, lp_current_directory, lp_startup_info, lp_process_information);
	}

	fs::path dll_fs_path = get_dll_path();
	if (dll_fs_path.empty())
	{
		return original_create_process_w(lp_application_name, lp_command_line, lp_process_attributes, lp_thread_attributes, b_inherit_handles, dw_creation_flags, lp_environment, lp_current_directory, lp_startup_info, lp_process_information);
	}

	std::string dll_path = dll_fs_path.string();

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

	if (!success)
	{
		std::cerr << "[venject] Failed to create process" << std::endl;
		return success;
	}

	if (lp_process_information && lp_process_information->hThread)
		ResumeThread(lp_process_information->hThread);

	return success;
}

extern "C" int uv_fs_lstat_hook(const void *loop, const void *req, const char *path, void *cb)
{
	if (!original_uv_fs_lstat) return -1;
	if (!path) return original_uv_fs_lstat(loop, req, path, cb);

	auto redirected = utils::redirect(path);
	const char *target = redirected.empty() ? path : redirected.c_str();
	return original_uv_fs_lstat(loop, req, target, cb);
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
{
    if (DetourIsHelperProcess()) return TRUE;
    if (fdwReason != DLL_PROCESS_ATTACH) return TRUE;

	original_get_file_attributes_w_t = GetFileAttributesW;
	original_create_file_w = CreateFileW;
	original_move_file_ex_w = MoveFileExW;
	original_create_process_w = CreateProcessW;

    DetourRestoreAfterWith();

    if (DetourTransactionBegin() != NO_ERROR) return FALSE;

    if (DetourUpdateThread(GetCurrentThread()) != NO_ERROR)
    {
        DetourTransactionAbort();
        return FALSE;
    }

#ifdef _DEBUG
    AllocConsole();
#endif

	ATTACH(original_get_file_attributes_w_t, get_file_attributes_w)
	ATTACH(original_create_file_w, create_file_w)
	ATTACH(original_move_file_ex_w, move_file_ex_w)
	ATTACH(original_create_process_w, create_process_w)

	// uv_fs_lstat needs special treatment, vencord uses it
	// GetModuleHandleA(current_exe_filename) + GetProcAddress("uv_fs_lstat").
    {
        std::string exe_name = shared::OurExecutablePath().filename().string();

        HMODULE exe_mod = nullptr;
        if (!exe_name.empty()) 
			exe_mod = GetModuleHandleA(exe_name.c_str());
        if (!exe_mod) 
			exe_mod = GetModuleHandleA(nullptr);

        if (exe_mod)
        {
            FARPROC proc = GetProcAddress(exe_mod, "uv_fs_lstat");
            if (proc)
            {
                original_uv_fs_lstat = reinterpret_cast<uv_fs_lstat_t>(proc);
				ATTACH(original_uv_fs_lstat, uv_fs_lstat_hook)
            }
        }
    }

	if (DetourTransactionCommit() != NO_ERROR) return FALSE;
    return TRUE;
}

#endif
