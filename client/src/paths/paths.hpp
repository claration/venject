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
	bool				  isFlatpak;
};

std::vector<DiscordInstall> FindDiscords();
fs::path					OurHookLibraryPath();

} // namespace paths
