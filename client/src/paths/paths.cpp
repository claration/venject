#include "paths.hpp"

#ifdef __APPLE__
#error "Apple is not supported"
#else // __APPLE__

#include <cstdlib>
#include <filesystem>
#include <unordered_set>

#include "../globals.h"
#include "../../../shared/shared.hpp"

namespace fs = std::filesystem;

namespace paths
{

std::vector<DiscordInstall> FindDiscords()
{
	std::vector<DiscordInstall> result;
	std::unordered_set<std::string> seen;

	fs::path home = shared::OurHomePath();
	if (home.empty()) return result;

	auto parseVersion = [](const std::string &name) -> std::vector<int> {
		if (!name.starts_with("app-")) return {};

		std::vector<int> version;
		std::string value = name.substr(4);
		std::size_t start = 0;

		while (start < value.size()) {
			const auto end = value.find('.', start);
			const auto part = value.substr(
				start,
				end == std::string::npos ? std::string::npos : end - start
			);

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

	auto add = [&](
		fs::path exe,
		const std::string &branch,
		bool flatpak) 
	{
		std::error_code ec;
		if (exe.empty() || !fs::exists(exe, ec)) return;
		if (fs::is_symlink(exe, ec)) return;

		fs::path asarPath = exe.parent_path() / "resources" / "app.asar";
		if (!fs::exists(asarPath, ec)) return;

		std::string key = exe.lexically_normal().string();
		if (!seen.insert(key).second) return;

		result.push_back({
			.path = exe,
			.branch = branch,
			.isFlatpak = flatpak
		});
	};

	const std::vector<
		std::pair<fs::path,
		std::tuple<const char *, const char *, bool>>
	> checks = {
		#ifdef _WIN32
		{ home / "Discord", {"Discord", "Discord Stable", false} },
		{ home / "DiscordCanary", {"DiscordCanary", "Discord Canary", false} },
		{ home / "DiscordPTB", {"DiscordPTB", "Discord PTB", false} }
		#else
		{ "/opt/discord", {"Discord", "Discord Stable", false} },
		{ home / ".config/discord", {"Discord", "Discord Stable", false} },
		{ home / ".config/discordcanary", {"DiscordCanary", "Discord Canary", false} },
		{ home / ".config/discordptb", {"DiscordPTB", "Discord PTB", false} },
		{ home / ".var/app/com.discordapp.Discord/config/discord", {"Discord", "Discord Stable", true} },
		{ home / ".var/app/com.discordapp.DiscordCanary/config/discord", {"DiscordCanary", "Discord Canary", true} },
		{ home / ".var/app/com.discordapp.DiscordPTB/config/discord", {"DiscordPTB", "Discord PTB", true} }
		#endif
	};

	for (const auto &[base, info] : checks) {
		std::error_code ec;
		if (!fs::exists(base, ec)) continue;

		auto [exeName, branch, flatpak] = info;

		if (fs::is_directory(base, ec)) {
			for (auto &entry : fs::directory_iterator(base, ec)) {
				const auto name = entry.path().filename().string();
				const auto version = parseVersion(name);

				if (version.empty())
					continue;

				fs::path exe = entry.path() / exeName;

				#ifdef _WIN32
				exe += ".exe";
				#endif

				if (fs::exists(exe, ec)) {
					add(exe, branch, flatpak);
				}
			}
		}

		fs::path exe = base / exeName;

		#ifdef _WIN32
		exe += ".exe";
		#endif

		if (fs::exists(exe, ec)) {
			add(exe, branch, flatpak);
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
