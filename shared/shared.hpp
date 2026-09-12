#pragma once

#include <filesystem>

#define MOD_NAME "Vencord"

namespace fs = std::filesystem;

namespace shared
{
fs::path OurHomePath();
fs::path OurConfigPath();
fs::path OurExecutablePath();
fs::path ModDistPath();
fs::path ModEntrypointPath();
}
