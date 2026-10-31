#pragma once

#include <QString>

namespace platform {

enum class ShortcutLocation {
	Custom,
	Applications,
	Desktop,
	Autostart
};

#if defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
QString createShortcut(
	const QString &name,
	const QString &fileName,
	const QString &targetExecutablePath,
	const QString &targetArguments,
	const QString &iconPath,
	ShortcutLocation location
);

bool deleteShortcut(const QString &fileName);
#endif

}
