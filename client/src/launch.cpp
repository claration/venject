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

#ifdef Q_OS_WIN32
#include <windows.h>
#include <detours.h>
#endif

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

	auto environment = QProcessEnvironment::systemEnvironment();
	environment.insert("DISABLE_UPDATER_AUTO_PATCHING", "true");
	environment.insert("CLIENT_EXECUTABLE_PATH", QString::fromStdString(shared::OurExecutablePath().string()));
	environment.insert("CLIENT_JS_PATH", QString::fromStdString(shared::ModEntrypointPath().string()));
	environment.insert("CLIENT_ARGV_JSON", QJsonDocument(jsonArgs).toJson(QJsonDocument::Compact));

#ifdef Q_OS_WIN32
	environment.insert("HOOK_DLL_PATH", QString::fromStdString(paths::OurHookLibraryPath().string()));

	// Build a Windows Unicode environment block.
	std::vector<wchar_t> environmentBlock;

	for (const auto &key : environment.keys())
	{
		const std::wstring entry =
			(key + '=' + environment.value(key)).toStdWString();

		environmentBlock.insert(
			environmentBlock.end(), entry.begin(), entry.end());
		environmentBlock.push_back(L'\0');
	}

	environmentBlock.push_back(L'\0');

		   // Build a mutable Windows command line.
	std::wstring commandLine =
		L"\"" + path.toStdWString() + L"\"";

	for (const auto &arg : args)
	{
		commandLine += L" \"";

		const std::wstring wideArg = arg.toStdWString();
		size_t backslashes = 0;

		for (wchar_t c : wideArg)
		{
			if (c == L'\\')
			{
				++backslashes;
			}
			else if (c == L'"')
			{
				commandLine.append(backslashes * 2 + 1, L'\\');
				commandLine += L'"';
				backslashes = 0;
			}
			else
			{
				commandLine.append(backslashes, L'\\');
				backslashes = 0;
				commandLine += c;
			}
		}

		commandLine.append(backslashes * 2, L'\\');
		commandLine += L'"';
	}

	std::vector<wchar_t> mutableCommandLine(
		commandLine.begin(), commandLine.end());
	mutableCommandLine.push_back(L'\0');

	const std::string dllPath =
		paths::OurHookLibraryPath().string();

	STARTUPINFOA startupInfo{};
	startupInfo.cb = sizeof(startupInfo);

	PROCESS_INFORMATION processInfo{};

	if (!DetourCreateProcessWithDllExA(
			path.toLocal8Bit().constData(),
			nullptr, // Replaced below with the mutable command line.
			nullptr,
			nullptr,
			FALSE,
			CREATE_UNICODE_ENVIRONMENT,
			environmentBlock.data(),
			nullptr,
			&startupInfo,
			&processInfo,
			dllPath.c_str(),
			nullptr))
	{
		qWarning() << "DetourCreateProcessWithDllExA failed:"
				   << GetLastError();
		return;
	}

	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);
#else
	QProcess process;
	environment.insert("LD_PRELOAD", QString::fromStdString(paths::OurHookLibraryPath().string()));

	process.setProcessEnvironment(environment);
	process.setProgram(path);
	process.setArguments(args);
#endif
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
