#include "launch.hpp"
#include "paths/linux.hpp"
#include "dl.hpp"

#include <QProcess>
#include <QProcessEnvironment>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStringList>

Launcher::Launcher(QObject* parent): QObject(parent){}

void Launcher::launch(
	const QString& path, 
	const QStringList& args,
	const QStringList& argv
) {
	QJsonArray jsonArgs;

    for (const auto& arg : argv)
        jsonArgs.append(arg);

    QProcess process;

    auto environment = QProcessEnvironment::systemEnvironment();
    environment.insert("DISABLE_UPDATER_AUTO_PATCHING", "true");
	environment.insert("MODLOADER_MOD_ENTRYPOINT", QString::fromStdString(PatcherPath().string()));
	environment.insert("MODLOADER_EXECUTABLE", QString::fromStdString(ExecutablePath().string()));
	environment.insert("MODLOADER_PROCESS_ARGV", QJsonDocument(jsonArgs).toJson(QJsonDocument::Compact));
	environment.insert("MODLOADER_WM_CLASS", QString::fromStdString("vencord-canary"));
	environment.insert("LD_PRELOAD", QString::fromStdString(HookPath().string()));

    process.setProcessEnvironment(environment);
    process.setProgram(path);
	process.setArguments(args);
    process.startDetached();
}

QVariantList Launcher::findDiscords()
{
    QVariantList discordList;

    for (const auto& discord : FindDiscords()) {
        QVariantMap item;

        item["path"] = QString::fromStdString(discord.path.string());
        item["branch"] = QString::fromStdString(discord.branch);
        item["isPatched"] = discord.isPatched;
        item["isFlatpak"] = discord.isFlatpak;
        item["isSystemElectron"] = discord.isSystemElectron;

        discordList.append(item);
    }

    return discordList;
}

QVariantMap Launcher::checkForUpdates()
{
    const auto info = CheckForUpdates();

    return {
        {"latestHash", info.latestHash},
        {"installedHash", info.installedHash},
        {"upToDate", info.upToDate}
    };
}

bool Launcher::updateVencord()
{
    return DownloadLatest(
        QDir::home().filePath(".config/Vencord/dist")
    );
}
