

#include "paths.hpp"

#ifdef __APPLE__
#error "Apple is not supported"
#elif defined(__linux__)

#include <cstdlib>
#include <filesystem>
#include <fstream> // std::ofstream
#include <unordered_set>
#include <sstream>
#include <unistd.h> // readlink
#include <limits.h> // PATH_MAX, setbuffer

#include "../globals.h"

namespace fs = std::filesystem;

namespace paths {

std::vector<DiscordInstall> FindDiscords()
{
    std::vector<DiscordInstall> result;
    std::unordered_set<std::string> seen;

    const char* homeEnv = std::getenv("HOME");
    fs::path home = homeEnv ? homeEnv : "";

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
        {"/opt/discord", {"Discord", "Discord Stable", false}},
        {home / ".config/discord", {"Discord", "Discord Stable", false}},
        {home / ".config/discordcanary", {"DiscordCanary", "Discord Canary", false}},
        {home / ".config/discordptb", {"DiscordPTB", "Discord PTB", false}},
        {home / ".var/app/com.discordapp.Discord/config/discord", {"Discord", "Discord Stable", true}},
        {home / ".var/app/com.discordapp.DiscordCanary/config/discord", {"DiscordCanary", "Discord Canary", true}},
        {home / ".var/app/com.discordapp.DiscordPTB/config/discord", {"DiscordPTB", "Discord PTB", true}}
    };

    for (const auto& [base, info] : checks) {
        std::error_code ec;
        if (!fs::exists(base, ec)) continue;

        auto [exeName, branch, flatpak] = info;
        fs::path bestExe;

        if (fs::is_directory(base, ec)) {
            for (auto& entry : fs::directory_iterator(base, ec)) {
                if (!entry.path().filename().string().starts_with("app-")) continue;

                fs::path exe = entry.path() / exeName;
                if (fs::exists(exe, ec) && exe > bestExe) bestExe = exe;
            }
        }

        if (!bestExe.empty()) {
            add(bestExe, branch, flatpak, false);
        } else {
            fs::path exe = base / exeName;
            if (fs::exists(exe, ec)) add(exe, branch, flatpak, fs::exists(base / "app.asar", ec));
        }
    }

    return result;
}

fs::path ModDistPath() 
{
    auto xdg = std::getenv("XDG_CONFIG_HOME");
    auto home = fs::path(std::getenv("HOME")) / ".config";

    fs::path path = xdg ? xdg : home;
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
    char buffer[PATH_MAX];

    const auto len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len == -1) return {};

    buffer[len] = '\0';

    return fs::path(buffer);
}

fs::path OurHookLibraryPath()
{
    return OurExecutablePath().parent_path() / "libvencord_hook.so";
}

}

#endif // __LINUX__
