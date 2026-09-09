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

extern "C" int __xstat64(int version, const char *path, struct stat64 *out)
{
	if (!original_xstat64)
		original_xstat64 = resolve<xstat64_t>("__xstat64");

	auto redirected = utils::redirect(path);

	return original_xstat64(
		version,
		redirected.empty() ? path : redirected.c_str(),
		out);
}

extern "C" int open64(const char *path, int flags, ...)
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

extern "C" int openat64(int dirfd, const char *path, int flags, ...)
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

__attribute__((constructor)) void init()
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

// TODO: use detours instead of dobby for windows
// use this as a reference https://github.com/MeguminSama/Electron-Hook/blob/main/src/windows/hooks.rs

#endif
