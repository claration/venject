#include "paths.hpp"

#ifdef __APPLE__
#error "Apple is not supported"
#else // __APPLE__

#include <cstdlib>
#include <filesystem>
#include <unordered_set>

#include "../../../shared/shared.hpp"

namespace fs = std::filesystem;

namespace paths
{

std::vector<DiscordInstall> FindDiscords()
{
    std::vector<DiscordInstall> result;
    std::unordered_set<std::string> seen;

    const fs::path home = shared::OurHomePath();
    if (home.empty()) return result;

    auto parseVersion = [](const std::string &name) {
        std::vector<int> version;
        if (!name.starts_with("app-")) return version;

        std::istringstream stream(name.substr(4));
        std::string part;

        while (std::getline(stream, part, '.')) {
            try {
                std::size_t pos;
                int n = std::stoi(part, &pos);
                if (pos != part.size() || n < 0) return std::vector<int>{};
                version.push_back(n);
            } catch (...) {
                return std::vector<int>{};
            }
        }

        return version;
    };

    auto add = [&](fs::path exe, const std::string &branch, bool flatpak, const std::string uniqueIdentifer) {
        std::error_code ec;
        if (!fs::is_regular_file(exe, ec) || ec || fs::is_symlink(exe, ec))
            return false;

        if (!fs::is_regular_file(exe.parent_path() / "resources/app.asar", ec) || ec)
            return false;

        if (!seen.insert(exe.lexically_normal().string()).second)
            return false;

        result.push_back({
			.path = exe, 
			.uniqueIdentifer = uniqueIdentifer, 
			.branch = branch, 
			.isFlatpak = flatpak
		});
        return true;
    };

    const std::vector<std::tuple<fs::path, const char *, const char *, bool>> checks = {
		#ifdef _WIN32
        {home / "Discord", "Discord", "Stable", false},
        {home / "DiscordCanary", "DiscordCanary", "Canary", false},
        {home / "DiscordPTB", "DiscordPTB", "PTB", false}
		#else
        {home / ".config/discord", "Discord", "Stable", false},
        {home / ".config/discordcanary", "DiscordCanary", "Canary", false},
        {home / ".config/discordptb", "DiscordPTB", "PTB", false},
        {home / ".var/app/com.discordapp.Discord/config/discord", "Discord", "Stable", true},
        {home / ".var/app/com.discordapp.DiscordCanary/config/discord", "DiscordCanary", "Canary", true},
        {home / ".var/app/com.discordapp.DiscordPTB/config/discord", "DiscordPTB", "PTB", true}
		#endif
    };

    for (const auto &[base, exeName, branch, flatpak] : checks) {
        std::error_code ec;
        if (!fs::is_directory(base, ec) || ec) continue;

        std::vector<std::pair<std::vector<int>, fs::path>> versions;

        for (const auto &entry : fs::directory_iterator(base, ec)) {
            if (ec) break;
            auto version = parseVersion(entry.path().filename().string());
            if (!version.empty() && entry.is_directory(ec))
                versions.emplace_back(std::move(version), entry.path());
        }

        std::sort(versions.begin(), versions.end(), [](const auto &a, const auto &b) {
            return std::lexicographical_compare(
                b.first.begin(), b.first.end(),
                a.first.begin(), a.first.end());
        });

        bool found = false;
        for (const auto &[version, dir] : versions) {
            fs::path exe = dir / exeName;
			#ifdef _WIN32
            exe += ".exe";
			#endif
            if (add(exe, branch, flatpak, base)) {
                found = true;
                break;
            }
        }

        if (!found) {
            fs::path exe = base / exeName;
			#ifdef _WIN32
            exe += ".exe";
			#endif
            add(exe, branch, flatpak, base);
        }
    }

    return result;
}

fs::path OurHookLibraryPath()
{
	#ifdef _WIN32
	return shared::OurExecutablePath().parent_path() / "libvencord_hook.dll";
	#else
	return shared::OurExecutablePath().parent_path() / "libvencord_hook.so";
	#endif
}

} // namespace paths

#endif // __APPLE__
