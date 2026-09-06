#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct DiscordInstall {
    std::filesystem::path path;
    std::string branch;
    bool isPatched;
    bool isFlatpak;
    bool isSystemElectron;
};

std::vector<DiscordInstall> FindDiscords();
fs::path DistPath();
fs::path PatcherPath();
fs::path ExecutablePath();
fs::path HookPath();
