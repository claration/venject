#include "launch.hpp"

#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStringList>

#include "network.hpp"
#include "paths/paths.hpp"
#include "../../shared/shared.hpp"

Launcher::Launcher(QObject *parent) : QObject(parent)
{
}

void Launcher::launch_self(
	const QString	  &path,
	const QStringList &discord_args,
	const QString	  &entry_point_path)
{
	QProcess process;

	QStringList args;
	args.append("-l");
	args.append(path);

	for (const QString &discord_arg : discord_args) {
		args.append("-a");
		args.append(discord_arg);
	}

	if (!entry_point_path.isEmpty()) {
		args.append("-o");
		args.append(entry_point_path);
	}

	process.setProgram(QString::fromStdString(shared::OurExecutablePath().string()));
	process.setArguments(args);
	process.startDetached();
}

void Launcher::launch(
	const QString	  &path, // discord executable
	const QStringList &args, // discord args (-o)
	const QStringList &argv) // mod loader full argv
{
	QJsonArray jsonArgs;

	for (const auto &arg : argv)
		jsonArgs.append(arg);

	// TODO: use detours instead of qprocesses for launching processes
	// injected with arbritrary .dll files
	QProcess process;

	auto environment = QProcessEnvironment::systemEnvironment();
	environment.insert("DISABLE_UPDATER_AUTO_PATCHING", "true");
	environment.insert("CLIENT_EXECUTABLE_PATH", QString::fromStdString(shared::OurExecutablePath().string()));
	environment.insert("CLIENT_JS_PATH", QString::fromStdString(shared::ModEntrypointPath().string()));
	environment.insert("CLIENT_ARGV_JSON", QJsonDocument(jsonArgs).toJson(QJsonDocument::Compact));
	environment.insert("LD_PRELOAD", QString::fromStdString(paths::OurHookLibraryPath().string()));

	process.setProcessEnvironment(environment);
	process.setProgram(path);
	process.setArguments(args);
	process.startDetached();
}

QVariantList Launcher::findDiscords()
{
	QVariantList discordList;

	for (const auto &discord : paths::FindDiscords()) {
		QVariantMap item;

		item["path"]			 = QString::fromStdString(discord.path.string());
		item["branch"]			 = QString::fromStdString(discord.branch);
		item["isFlatpak"]		 = discord.isFlatpak;

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
		{"upToDate", result.upToDate}};
}

QVariantMap Launcher::updateVencord()
{
	const auto result = network::DownloadLatest();

	return {
		{"success", result.success},
		{"error", result.error}};
}
