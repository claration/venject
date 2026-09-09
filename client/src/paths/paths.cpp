#include "paths.hpp"

#ifdef __APPLE__
#error "Apple is not supported"
#else // __APPLE__

#include <cstdlib>
#include <filesystem>
#include <fstream> // std::ofstream
#include <unordered_set>
#include <sstream>
#ifdef _WIN32
# include <windows.h>
#else // _WIN32
# include <unistd.h> // readlink
# include <limits.h> // PATH_MAX, setbuffer
#endif // _WIN32

#include "../globals.h"

namespace fs = std::filesystem;

namespace paths {

std::vector<DiscordInstall> FindDiscords()
{
    std::vector<DiscordInstall> result;
    std::unordered_set<std::string> seen;

    #ifdef _WIN32
    const char* localAppDataEnv = std::getenv("LOCALAPPDATA");
    fs::path home = localAppDataEnv ? localAppDataEnv : "";
    #else
    const char* homeEnv = std::getenv("HOME");
    fs::path home = homeEnv ? homeEnv : "";
    #endif

    auto parseVersion = [](const std::string& name) -> std::vector<int> {
        if (!name.starts_with("app-")) return {};

        std::vector<int> version;
        std::string value = name.substr(4);
        std::size_t start = 0;

        while (start < value.size()) {
            const auto end = value.find('.', start);
            const auto part = value.substr(start, end == std::string::npos ? std::string::npos : end - start);

            try {
                version.push_back(std::stoi(part));
            } catch (...) {
                return {};
            }

            if (end == std::string::npos) break;
            start = end + 1;
        }

        return version;
    };

    auto newerVersion = [](const std::vector<int>& a, const std::vector<int>& b) {
        const auto size = std::max(a.size(), b.size());

        for (std::size_t i = 0; i < size; ++i) {
            const int av = i < a.size() ? a[i] : 0;
            const int bv = i < b.size() ? b[i] : 0;

            if (av != bv) return av > bv;
        }

        return false;
    };

    auto add = [&](fs::path exe, const std::string& branch, bool flatpak, bool systemElectron) {
        std::error_code ec;
        if (exe.empty() || !fs::exists(exe, ec)) return;

        fs::path resolved = fs::canonical(exe, ec);
        if (ec) resolved = fs::weakly_canonical(exe, ec);

        std::string key = resolved.empty() ? exe.string() : resolved.string();
        if (!seen.insert(key).second) return;

        fs::path res = exe.parent_path() / "resources";
        if (!fs::exists(res, ec)) res = exe.parent_path();

        result.push_back({
            .path = exe,
            .branch = branch,
            .isPatched = fs::exists(res / "_app.asar", ec) || fs::exists(res / "_app.asar.unpacked", ec),
            .isFlatpak = flatpak,
            .isSystemElectron = systemElectron
        });
    };

    if (home.empty()) return result;

    const std::vector<std::pair<fs::path, std::tuple<const char*, const char*, bool>>> checks = {
	#ifdef _WIN32
        {home / "Discord", {"Discord", "Discord Stable", false}},
        {home / "DiscordCanary", {"DiscordCanary", "Discord Canary", false}},
        {home / "DiscordPTB", {"DiscordPTB", "Discord PTB", false}}
    #else
        {"/opt/discord", {"Discord", "Discord Stable", false}},
        {home / ".config/discord", {"Discord", "Discord Stable", false}},
        {home / ".config/discordcanary", {"DiscordCanary", "Discord Canary", false}},
        {home / ".config/discordptb", {"DiscordPTB", "Discord PTB", false}},
        {home / ".var/app/com.discordapp.Discord/config/discord", {"Discord", "Discord Stable", true}},
        {home / ".var/app/com.discordapp.DiscordCanary/config/discord", {"DiscordCanary", "Discord Canary", true}},
        {home / ".var/app/com.discordapp.DiscordPTB/config/discord", {"DiscordPTB", "Discord PTB", true}}
    #endif
	};

    for (const auto& [base, info] : checks) {
        std::error_code ec;
        if (!fs::exists(base, ec)) continue;

        auto [exeName, branch, flatpak] = info;
        fs::path bestExe;
        std::vector<int> bestVersion;

        if (fs::is_directory(base, ec)) {
            for (auto& entry : fs::directory_iterator(base, ec)) {
                const auto name = entry.path().filename().string();
                const auto version = parseVersion(name);

                if (version.empty()) continue;

                fs::path exe = entry.path() / exeName;
                #ifdef _WIN32
                exe += ".exe";
                #endif

                if (fs::exists(exe, ec) && (bestExe.empty() || newerVersion(version, bestVersion))) {
                    bestExe = exe;
                    bestVersion = version;
                }
            }
        }

        if (!bestExe.empty()) {
            add(bestExe, branch, flatpak, false);
        } else {
            fs::path exe = base / exeName;
            #ifdef _WIN32
            exe += ".exe";
            #endif
            if (fs::exists(exe, ec)) add(exe, branch, flatpak, fs::exists(base / "app.asar", ec));
        }
    }

    return result;
}

fs::path ModDistPath() 
{
    #ifdef _WIN32
    auto appdata = std::getenv("APPDATA");
    fs::path path = appdata ? appdata : "";
    #else // _WIN32
    auto xdg = std::getenv("XDG_CONFIG_HOME");
    auto home = fs::path(std::getenv("HOME")) / ".config";

    fs::path path = xdg ? xdg : home;
    #endif // _WIN32

    path /= MOD_NAME;
    path /= "dist";

    fs::create_directories(path);
	
	auto package_json = path / "package.json";
    if (!fs::exists(package_json))
        std::ofstream(package_json) << "{}";

    return path;
}

fs::path ModEntrypointPath()
{
    return ModDistPath() / "patcher.js";
}

fs::path OurExecutablePath()
{
#ifdef _WIN32
    char buffer[MAX_PATH];

    const auto len = GetModuleFileNameA(nullptr, buffer, sizeof(buffer));
    if (len == 0) return {};

    return fs::path(buffer, buffer + len);
#else
    char buffer[PATH_MAX];

    const auto len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len == -1) return {};

    buffer[len] = '\0';

    return fs::path(buffer);
#endif
}

fs::path OurHookLibraryPath()
{
    #ifdef _WIN32
    return OurExecutablePath().parent_path() / "libvencord_hook.dll";
    #else
    return OurExecutablePath().parent_path() / "libvencord_hook.so";
    #endif
}

}

#endif // __APPLE__
