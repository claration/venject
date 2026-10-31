#include "DiscordInstall.hpp"

#include <utility>

DiscordInstall::DiscordInstall(
	QObject	   	  *parent,
	const QString &exe,
	const QDir 	  &folder,
	QString		  unique_identifier,
	QString		  branch,
	const bool	  is_flatpak,
	const bool	  is_system_flatpak,
	const bool	  is_patched)
	: QObject(parent),
	  _exe(exe),
	  _folder(folder),
	  _uniqueIdentifier(std::move(unique_identifier)),
	  _branch(std::move(branch)),
	  _isFlatpak(is_flatpak),
	  _isSystemFlatpak(is_system_flatpak),
	  _isPatched(is_patched)
{
}

QString DiscordInstall::exe() const
{
	return this->_exe;
}

QDir DiscordInstall::folder() const
{
	return this->_folder;
}

QString DiscordInstall::uniqueIdentifier() const
{
	return this->_uniqueIdentifier;
}

QString DiscordInstall::branch() const
{
	return this->_branch;
}

bool DiscordInstall::isFlatpak() const
{
	return this->_isFlatpak;
}

bool DiscordInstall::isSystemFlatpak() const
{
	return this->_isSystemFlatpak;
}

bool DiscordInstall::isPatched() const
{
	return this->_isPatched;
}

QList<DiscordInstall*> DiscordInstall::getInstances()
{
	QList<DiscordInstall*> result;
	QSet<QString>          seen;

	QString home = QDir::homePath();
	if (home.isEmpty()) return result;

	auto parseVersion = [](const QString &name) -> QList<int> {
		if (!name.startsWith(u"app-"))
			return {};

		QStringList parts = name.mid(4).split(u'.');
		QList<int>  version;

		for (const QString &part : parts) {
			bool ok = false;
			int n = part.toInt(&ok);
			if (!ok || n < 0)
				return {};
			version.append(n);
		}

		return version;
	};

	auto add = [&](
		const QString &exePath,
		const QString &branch,
		bool flatpak,
		bool isSystemFlatpak,
		const QString &uniqueIdentifier)
	{
		QFileInfo exeInfo(exePath);
		if (!exeInfo.exists() || (!flatpak && exeInfo.isSymLink()))
			return false;

		QString resourcesDir = exeInfo.absolutePath() + "/resources";
		QFileInfo asarInfo(resourcesDir + "/app.asar");
		if (!asarInfo.isFile())
			return false;

		QFileInfo patchedAsarInfo(resourcesDir + "/_app.asar");
		bool isPatched = patchedAsarInfo.isFile();

		QString normalizedPath = QDir::cleanPath(exeInfo.absoluteFilePath());
		if (seen.contains(normalizedPath))
			return false;

		seen.insert(normalizedPath);

		QDir installDir(exeInfo.absolutePath());

		result.append(new DiscordInstall(
			nullptr,
			exePath,
			installDir,
			uniqueIdentifier,
			branch,
			flatpak,
			isSystemFlatpak,
			isPatched
		));
		return true;
	};

	struct CheckConfig {
		QString     base;
		QString     exeName;
		bool        flatpak;
		bool        isSystemFlatpak;
	};

	QList<CheckConfig> checks = {
#if defined(Q_OS_WIN)
		{home + "/AppData/Local/Discord", "Discord", false, false},
		{home + "/AppData/Local/DiscordCanary", "DiscordCanary", false, false},
		{home + "/AppData/Local/DiscordPTB", "DiscordPTB", false, false}
#else
		{home + "/.config/discord", "Discord", false, false},
		{home + "/.config/discordcanary", "DiscordCanary", false, false},
		{home + "/.config/discordptb", "DiscordPTB", false, false},

		{home + "/.var/app/com.discordapp.Discord/config/discord", "Discord", true, false},
		{home + "/.var/app/com.discordapp.DiscordCanary/config/discord", "DiscordCanary", true, false},
		{home + "/.var/app/com.discordapp.DiscordPTB/config/discord", "DiscordPTB", true, false},

		{"/opt/discord", "Discord", false, false},
		{"/opt/discordcanary", "DiscordCanary", false, false},
		{"/opt/discordptb", "DiscordPTB", false, false},

		{"/usr/share/discord", "Discord", false, false},
		{"/usr/share/discordcanary", "DiscordCanary", false, false},
		{"/usr/share/discordptb", "DiscordPTB", false, false},

		{"/usr/lib64/discord", "Discord", false, false},
		{"/usr/lib64/discordcanary", "Discord", false, false},
		{"/usr/lib64/discordptb", "Discord", false, false},

		{home + "/.local/share/discord", "Discord", false, false},
		{home + "/.dvm/discord", "Discord", false, false},

		{"/var/lib/flatpak/app/com.discordapp.Discord/current/active/files/discord", "Discord", true, true},
		{"/var/lib/flatpak/app/com.discordapp.DiscordCanary/current/active/files/discord-canary", "DiscordCanary", true, true},
		{"/var/lib/flatpak/app/com.discordapp.DiscordPTB/current/active/files/discord-ptb", "DiscordPTB", true, true},

		{home + "/.local/share/flatpak/app/com.discordapp.Discord/current/active/files/discord", "Discord", true, false},
		{home + "/.local/share/flatpak/app/com.discordapp.Discord/current/active/files/discord-canary", "DiscordCanary", true, false},
		{home + "/.local/share/flatpak/app/com.discordapp.Discord/current/active/files/discord-ptb", "DiscordPTB", true, false},
#endif
	};

	for (const auto &check : checks) {
		QDir baseDir(check.base);
		if (!baseDir.exists())
			continue;

		struct VersionEntry {
			QList<int> version;
			QString    dirPath;
		};
		QList<VersionEntry> versions;

		QDirIterator it(baseDir.absolutePath(), QDir::Dirs | QDir::NoDotAndDotDot);
		while (it.hasNext()) {
			it.next();
			QString    dirName = it.fileInfo().fileName();
			QList<int> ver = parseVersion(dirName);
			if (!ver.isEmpty()) {
				versions.append({std::move(ver), it.filePath()});
			}
		}

		bool found = false;
		if (!versions.isEmpty()) {
			std::ranges::sort(versions, [](const auto &a, const auto &b) {
				return b.version < a.version;
			});

			for (const auto &verEntry : versions) {
				QString exe = verEntry.dirPath + "/" + check.exeName;
#if defined(Q_OS_WIN)
				exe += ".exe";
#endif
				if (add(exe, check.exeName, check.flatpak, check.isSystemFlatpak, baseDir.absolutePath())) {
					found = true;
					break;
				}
			}
		}

		// Fallback
		if (!found) {
			QString exe = baseDir.absolutePath() + "/" + check.exeName;
#if defined(Q_OS_WIN)
			exe += ".exe";
#endif
			add(exe, check.exeName, check.flatpak, check.isSystemFlatpak, baseDir.absolutePath());
		}
	}

	return result;
}

DiscordInstall* DiscordInstall::getInstanceByUniqueIdentifier(const QString &uniqueIdentifier)
{
	qWarning() << uniqueIdentifier;
    if (uniqueIdentifier.isEmpty()) return nullptr;

    QList<DiscordInstall*> instances = getInstances();
    DiscordInstall* foundInstance = nullptr;

    for (DiscordInstall *inst : instances) {
        if (inst && inst->uniqueIdentifier() == uniqueIdentifier) {
            if (!foundInstance) {
                foundInstance = inst;
            } else {
                inst->deleteLater();
            }
        } else {
            if (inst) {
                inst->deleteLater();
            }
        }
    }

    if (foundInstance) {
        foundInstance->setParent(nullptr);
    }

    return foundInstance;
}
