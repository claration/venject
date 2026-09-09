#include "utils.hpp"

#include <fcntl.h> // O_WRONLY, O_CREAT, O_TRUNC, open
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h> // readlink

#include "globals.h"

namespace utils
{

std::string write_payload_to_tmp()
{
	if (::access(VENJECT_ASAR_PATH, F_OK) != 0) {
		std::ofstream file(VENJECT_ASAR_PATH, std::ios::binary);
		if (!file)
			return {};

		file.write(
			reinterpret_cast<const char *>(g_payload_asar.data),
			static_cast<std::streamsize>(g_payload_asar.data_len));

		if (!file)
			return {};
	}

	return VENJECT_ASAR_PATH;
}

// we can probably elimate this logic with an environment variables lol
const std::string app_folder = []() -> std::string {
	char buf[4096];

	const auto n = readlink(
		"/proc/self/exe",
		buf,
		sizeof(buf) - 1);
	if (n <= 0)
		return {};

	buf[n] = '\0';

	std::string path(buf);

	const auto slash = path.find_last_of('/');
	if (slash == std::string::npos)
		return {};

	path.resize(slash);

	const auto parent = path.find_last_of('/');
	if (parent == std::string::npos)
		return {};

	return path.substr(parent + 1);
}();

std::string redirect_asar_path(
	const std::string &path,
	const std::string &custom_asar,
	const std::string &app_folder)
{
	for (char sep : {'/', '\\'}) {
		const std::string original = "resources" + std::string(1, sep) + "_app.asar";

		if (path.find(original) != std::string::npos) {
			const std::string from = std::string(1, sep) + "_app.asar";
			const std::string to   = std::string(1, sep) + "app.asar";

			auto		result = path;
			std::size_t pos	   = 0;

			while ((pos = result.find(from, pos)) != std::string::npos) {
				result.replace(pos, from.size(), to);
				pos += to.size();
			}

			return result;
		}
	}

	for (char sep : {'/', '\\'}) {
		const std::string needle =
			app_folder.empty()
				? "resources" + std::string(1, sep) + "app.asar"
				: app_folder + std::string(1, sep) +
					  "resources" + std::string(1, sep) + "app.asar";

		if (path.find(needle) != std::string::npos) {
			return custom_asar;
		}
	}

	return {};
}

std::string redirect(const char *path)
{
	if (!path)
		return {};

	return redirect_asar_path(
		path,
		write_payload_to_tmp(),
		app_folder);
}

} // namespace utils
