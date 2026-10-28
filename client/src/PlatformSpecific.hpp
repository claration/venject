#pragma once

namespace platform {

#if defined(Q_OS_LINUX) || defined(Q_OS_WIN32)

bool createShortcut(
	const QString &name,
	const QString &fileName,
	const QString &targetExecutablePath,
	const QString &targetArguments = "",
	const QString &iconPath = ""
);

bool deleteShortcut(const QString &fileName);

#endif

}
