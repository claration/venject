#include "linux.hpp"

#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

std::vector<DiscordInstall> FindDiscords() {
    std::vector<DiscordInstall> result;

    const fs::path home = std::getenv("HOME");

    const auto add = [&](const fs::path& installPath,
                         const std::string& executableName,
                         const std::string& branch,
                         bool flatpak) {
        std::error_code ec;

        // New layout: app-*/resources/app
		fs::path executable;

		if (fs::is_directory(installPath, ec)) {
			for (const auto& entry : fs::directory_iterator(installPath, ec)) {
				if (ec)
					break;

				if (!entry.is_directory())
					continue;

				const auto resources = entry.path() / "resources";

				std::error_code dir_ec;
				if (!fs::is_directory(resources, dir_ec))
					continue;

				const auto exe = entry.path() / executableName;

				std::error_code exe_ec;
				if (fs::is_regular_file(exe, exe_ec)) {
					executable = exe;
					break;
				}
			}
		}

        if (!executable.empty()) {
            const auto resources =
                executable.parent_path() / "resources";

            result.push_back({
                .path = executable,
                .branch = branch,
                .isPatched = fs::exists(
                    resources / "_app.asar",
                    ec
                ),
                .isFlatpak = flatpak,
                .isSystemElectron = false
            });

            return;
        }

        // Old layout: resources/app
        const auto resources = installPath / "resources";

        if (fs::is_directory(resources, ec)) {
            const auto exe = installPath / executableName;

            if (fs::exists(exe, ec)) {
                result.push_back({
                    .path = exe,
                    .branch = branch,
                    .isPatched = fs::exists(
                        resources / "_app.asar",
                        ec
                    ),
                    .isFlatpak = flatpak,
                    .isSystemElectron = false
                });

                return;
            }
        }

        // System Electron.
        if (fs::exists(installPath / "app.asar", ec)) {
            const auto exe = installPath / executableName;

            if (fs::exists(exe, ec)) {
                result.push_back({
                    .path = exe,
                    .branch = branch,
                    .isPatched = fs::exists(
                        installPath / "_app.asar.unpacked",
                        ec
                    ),
                    .isFlatpak = flatpak,
                    .isSystemElectron = true
                });
            }
        }
    };

    // Standard installations.
    for (const auto& [name, executable, branch] : {
        std::tuple{"discord", "Discord", "stable"},
        std::tuple{"discordcanary", "DiscordCanary", "canary"},
        std::tuple{"discordptb", "DiscordPTB", "ptb"}
    }) {
        for (const auto& base : {
            fs::path{"/usr/share"},
            fs::path{"/usr/lib64"},
            fs::path{"/opt"},
            home / ".local/share",
            home / ".dvm",
            fs::path{"/var/lib/flatpak/app"},
            home / ".local/share/flatpak/app"
        }) {
            const auto path = base / name;

            if (fs::exists(path))
                add(path, executable, branch, false);
        }
    }

    // Native Discord.
    add(home / ".config/discord",
        "Discord", "Discord Stable", false);

    add(home / ".config/discordcanary",
        "DiscordCanary", "Discord Canary", false);

    add(home / ".config/discordptb",
        "DiscordPTB", "Discord PTB", false);

    // Flatpak Discord.
    add(
        home / ".var/app/com.discordapp.Discord/config/discord",
        "Discord",
        "Discord Stable",
        true
    );

    add(
        home / ".var/app/com.discordapp.DiscordCanary/config/discord",
        "DiscordCanary",
        "Discord Canary",
        true
    );

    add(
        home / ".var/app/com.discordapp.DiscordPTB/config/discord",
        "DiscordPTB",
        "Discord PTB",
        true
    );

    return result;
}

fs::path DistPath() {
	const fs::path home = std::getenv("HOME");
    return (home / ".config/Vencord/dist");
}

fs::path PatcherPath()
{
    return DistPath() / "patcher.js";
}

#include <unistd.h>
#include <limits.h>

fs::path ExecutablePath()
{
    char buffer[PATH_MAX];

    const auto len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);

    if (len == -1)
        return {};

    buffer[len] = '\0';

    return fs::path(buffer);
}

fs::path HookPath()
{
    return ExecutablePath().parent_path() / "libvencord_hook.so";
}
