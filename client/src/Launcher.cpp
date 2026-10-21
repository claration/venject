#include "Launcher.hpp"

#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStringList>
#include <QDebug>

#include "network.hpp"
#include "paths/paths.hpp"
#include "../../shared/shared.hpp"

#ifdef Q_OS_WIN32
# include <windows.h>
# include <detours.h>
#endif

Launcher::Launcher(QObject *parent) : QObject(parent){}

void Launcher::launchSelf(
	const QString	  &path,
	const QString     &discord_args,
	const QString	  &entry_point_path)
{
	QProcess process;

	QStringList args;
	args.append("-l");
	args.append(path);

	#ifndef Q_OS_WIN32
	const QStringList arguments = discord_args.split(' ', Qt::SkipEmptyParts);
	for (const QString &discord_arg : arguments) {
		args.append("-a");
		args.append(discord_arg);
	}
	#endif // Q_OS_WIN32

	if (!entry_point_path.isEmpty()) {
		args.append("-o");
		args.append(entry_point_path);
	}

	process.setProgram(QString::fromStdString(shared::OurExecutablePath().string()));
	process.setArguments(args);
	process.startDetached();
}

void Launcher::launch(
    const QString     &path,  // discord executable
    const QString     &args,  // discord args (-a)
    const QString     &entry, // discord entrypoint (-o)
    const QStringList &argv)  // mod loader full argv
{
    QJsonArray jsonArgs;

    for (const auto &arg : argv)
        jsonArgs.append(arg);

    auto environment = QProcessEnvironment::systemEnvironment();

    environment.insert("DISABLE_UPDATER_AUTO_PATCHING", "true");
    environment.insert("CLIENT_EXECUTABLE_PATH", QString::fromStdString(shared::OurExecutablePath().string()));

    environment.insert(
        "CLIENT_JS_PATH",
        entry.isEmpty()
            ? QString::fromStdString(shared::ModEntrypointPath().string())
            : entry);

    environment.insert("CLIENT_ARGV_JSON", QJsonDocument(jsonArgs).toJson(QJsonDocument::Compact));

	const QString logPath = uniqueLogPathForFile(path);

#ifdef Q_OS_WIN32

    environment.insert(
        "HOOK_DLL_PATH",
        QString::fromStdString(paths::OurHookLibraryPath().string()));

    std::vector<wchar_t> environmentBlock;

    for (const auto &key : environment.keys())
    {
        const std::wstring environmentEntry =
            (key + '=' + environment.value(key)).toStdWString();

        environmentBlock.insert(
            environmentBlock.end(),
            environmentEntry.begin(),
            environmentEntry.end());

        environmentBlock.push_back(L'\0');
    }

    // Environment blocks must end with two null characters.
    environmentBlock.push_back(L'\0');

    // Build the command line.
    std::wstring commandLine = L"\"" + path.toStdWString() + L"\"";

    std::vector<wchar_t> mutableCommandLine(
        commandLine.begin(), commandLine.end());

    mutableCommandLine.push_back(L'\0');

    // Open the log file. The child process inherits this handle.
    HANDLE logFile = CreateFileW(
        reinterpret_cast<LPCWSTR>(logPath.utf16()),
        FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (logFile == INVALID_HANDLE_VALUE)
    {
        qWarning() << "Failed to open log file:"
                   << logPath << GetLastError();
        return;
    }

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags |= STARTF_USESTDHANDLES;
    startupInfo.hStdOutput = logFile;
    startupInfo.hStdError = logFile;
    startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION processInfo{};

    const std::wstring executablePath = path.toStdWString();
    const std::string dllPath = paths::OurHookLibraryPath().string();

    if (!DetourCreateProcessWithDllExW(
            executablePath.c_str(),
            mutableCommandLine.data(),
            nullptr,
            nullptr,
            TRUE,
            CREATE_UNICODE_ENVIRONMENT,
            environmentBlock.data(),
            nullptr,
            &startupInfo,
            &processInfo,
            dllPath.c_str(),
            nullptr))
    {
        qWarning() << "DetourCreateProcessWithDllExW failed:"
                   << GetLastError();

        CloseHandle(logFile);
        return;
    }

    CloseHandle(logFile);

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);

#else

    environment.insert(
        "LD_PRELOAD",
        QString::fromStdString(paths::OurHookLibraryPath().string()));

    QProcess process;

    process.setProcessEnvironment(environment);
    process.setProgram(path);

    const QStringList arguments = args.split(' ', Qt::SkipEmptyParts);
    process.setArguments(arguments);

    process.setStandardOutputFile(logPath, QIODevice::Truncate);
    process.setStandardErrorFile(logPath, QIODevice::Append);

    process.startDetached();
#endif
}

QVariantList Launcher::findDiscords()
{
	QVariantList discordList;

	for (const auto &discord : paths::FindDiscords()) {
		QVariantMap item;

		item["path"]			 = QString::fromStdString(discord.path.string());
		item["branch"]			 = QString::fromStdString(discord.branch);
		item["id"]			 	 = QString::fromStdString(discord.uniqueIdentifer);
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

static inline QString cleanFilePath(const QString &path) {
    QString cleanPath = path;
    if (cleanPath.startsWith("file://")) {
        cleanPath.remove(0, 7);
    }
    return cleanPath;
}

#include <cstdint>
#include <string>

/// This is used for creating a unique tiny string
static uint16_t hashString(const std::u16string& str) {
    uint64_t h = 0x189d7;
    for (char16_t c : str) {
        h = h * 33 + static_cast<uint64_t>(c);
    }
    uint64_t mixed = (((h * 33 + (h >> 16)) 
		* 33 + (h >> 32)) * 33 + (h >> 48));
    return static_cast<uint16_t>(mixed);
}

QString Launcher::uniqueLogPathForFile(const QString &id) {
    QString cleanPath = cleanFilePath(id);
    
    uint16_t pathHash = hashString(cleanPath.toStdU16String());
    QString fileName = QString::number(pathHash);

    return QDir::temp().filePath(QString("%1.log").arg(fileName));
}


#include <QFile>
#include <QTextStream>

QString Launcher::readTextFile(const QString &id) {
    QFile file(id);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        return in.readAll();
    }

    return QString();
}
