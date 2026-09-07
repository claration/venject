#include "launch.hpp"

#include <QProcess>
#include <QProcessEnvironment>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStringList>

#include "paths/paths.hpp"
#include "network.hpp"

Launcher::Launcher(QObject* parent): QObject(parent){}

void Launcher::launch_self(
	const QString& path,
	const QStringList& discord_args,
	const QString& entry_point_path)
{
    QProcess process;

    QStringList args;
    args.append("-l");
    args.append(path);

	for (const QString& discord_arg : discord_args)
    {
        args.append("-a");
        args.append(discord_arg);
    }

	if (!entry_point_path.isEmpty())
    {
        args.append("-o");
        args.append(entry_point_path);
    }

    process.setProgram(QString::fromStdString(paths::OurExecutablePath().string()));
    process.setArguments(args);
    process.startDetached();
}

void Launcher::launch(
	const QString& path, 		// discord executable
	const QStringList& args, 	// discord args (-o)
	const QStringList& argv) 	// mod loader full argv 
{
	QJsonArray jsonArgs;

    for (const auto& arg : argv)
        jsonArgs.append(arg);

    QProcess process;

    auto environment = QProcessEnvironment::systemEnvironment();
    environment.insert("DISABLE_UPDATER_AUTO_PATCHING", "true");
	environment.insert("MODLOADER_MOD_ENTRYPOINT", QString::fromStdString(paths::ModEntrypointPath().string()));
	environment.insert("MODLOADER_EXECUTABLE", QString::fromStdString(paths::OurExecutablePath().string()));
	environment.insert("MODLOADER_PROCESS_ARGV", QJsonDocument(jsonArgs).toJson(QJsonDocument::Compact));
	environment.insert("MODLOADER_WM_CLASS", QString::fromStdString("vencord-canary"));
	environment.insert("LD_PRELOAD", QString::fromStdString(paths::OurHookLibraryPath().string()));

    process.setProcessEnvironment(environment);
    process.setProgram(path);
	process.setArguments(args);
    process.startDetached();
}

QVariantList Launcher::findDiscords()
{
    QVariantList discordList;

    for (const auto& discord : paths::FindDiscords()) {
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
    const auto result = network::CheckForUpdates();

    return {
        {"success", result.success},
        {"error", result.error},
        {"latestHash", result.latestHash},
        {"installedHash", result.installedHash},
        {"upToDate", result.upToDate}
    };
}

QVariantMap Launcher::updateVencord()
{
    const auto result = network::DownloadLatest();

    return {
        {"success", result.success},
        {"error", result.error}
    };
}
