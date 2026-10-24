#pragma once

#include <QtSystemDetection>

#ifdef Q_OS_WIN32

#include <QString>

namespace windows
{

bool createShortcut(
	const QString &shortcutPath,
	const QString &targetPath,
	const QString &arguments		= {},
	const QString &workingDirectory = {},
	const QString &iconPath			= {});

}
#endif