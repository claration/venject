#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace paths
{

struct DiscordInstall {
	std::filesystem::path path;
	std::filesystem::path folder;
	std::string			  branch;
	bool				  isPatched;
	bool				  isFlatpak;
	bool				  isSystemElectron;
};

std::vector<DiscordInstall> FindDiscords();
fs::path					ModDistPath();
fs::path					ModEntrypointPath();
fs::path					OurExecutablePath();
fs::path					OurHookLibraryPath();

} // namespace paths
