#include "PlatformSpecific.hpp"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>

namespace platform {

#ifdef Q_OS_LINUX

bool createShortcut(
	const QString &name,
	const QString &fileName,
	const QString &targetPath,
	const QString &arguments,
	const QString &iconPath)
{
	if (fileName.isEmpty() || targetPath.isEmpty()) return false;

	QString appsDir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
	if (appsDir.isEmpty()) return false;

	QDir().mkpath(appsDir);

	QString fullPath = appsDir + "/" + "vencord-launcher-" + fileName + ".desktop";

	QFile file(fullPath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

	QTextStream out(&file);
	out << "[Desktop Entry]\n"
		<< "Type=Application\n"
		<< "Name=" << name << "\n"
		<< "Exec=" << targetPath << (arguments.isEmpty() ? "" : " " + arguments) << "\n";

	if (!iconPath.isEmpty()) out << "Icon=" << iconPath << "\n";

	out << "Terminal=false\n"
		<< "Categories=Network;\n";
	file.close();

	QFile::setPermissions(
		fullPath,
		QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadUser | QFile::ExeUser
	);

	return true;
}

bool deleteShortcut(const QString &fileName)
{
	if (fileName.isEmpty()) return false;

	QString appsDir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
	if (appsDir.isEmpty()) return false;

	QString fullPath = appsDir + "/" + "vencord-launcher-" + fileName + ".desktop";

	QFile file(fullPath);
	if (file.exists()) return file.remove();

	return true;
}

#endif

}
