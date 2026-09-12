#include "utils.hpp"

#include <fcntl.h> // O_WRONLY, O_CREAT, O_TRUNC, open
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h> // readlink

#include "globals.h"
#include <unistd.h>

#include "../../shared/shared.hpp"

namespace utils
{

std::string write_payload_to_tmp()
{
    const auto path = shared::ModDistPath() / (MOD_NAME + std::string(".asar"));

    if (!std::filesystem::exists(path)) {
        std::ofstream file(path, std::ios::binary);
        if (!file) return {};

        file.write(
            reinterpret_cast<const char *>(g_payload_asar.data),
            static_cast<std::streamsize>(g_payload_asar.data_len));
        if (!file) return {};
    }

    return path.string();
}

std::string redirect_asar_path(const std::string& path)
{
    for (char sep : {'/', '\\'}) {
        const std::string original = "resources" + std::string(1, sep) + "_app.asar";

        if (path.find(original) != std::string::npos) {
            const std::string from = std::string(1, sep) + "_app.asar";
            const std::string to = std::string(1, sep) + "app.asar";

            auto result = path;
            std::size_t pos = 0;

            while ((pos = result.find(from, pos)) != std::string::npos) {
                result.replace(pos, from.size(), to);
                pos += to.size();
            }

            return result;
        }
    }

    std::filesystem::path app_folder = shared::OurExecutablePath().parent_path();
	
    if (!app_folder.empty()) {
        std::string app_folder_str = app_folder.string();
        for (char sep : {'/', '\\'}) {
            const std::string needle = app_folder_str + std::string(1, sep) + "resources" + std::string(1, sep) + "app.asar";

            if (path.find(needle) != std::string::npos) {
                return write_payload_to_tmp();
            }
        }
    }

    return {};
}

std::string redirect(const char *path)
{
    if (!path) return {};
    return redirect_asar_path(path);
}

} // namespace utils
