#pragma once

namespace platform {

#ifdef Q_OS_LINUX

bool createShortcut(
	const QString &name,
	const QString &fileName,
	const QString &targetPath,
	const QString &arguments = "",
	const QString &iconPath = ""
);

bool deleteShortcut(const QString &fileName);

#endif

}
