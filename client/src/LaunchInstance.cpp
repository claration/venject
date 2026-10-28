#include "LaunchInstance.hpp"
#include "DiscordInstall.hpp"

LaunchInstance::LaunchInstance(
	QObject *parent,
	QString id,
	QString unique_identifier)
	: QObject(parent),
	_id(id.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : std::move(id)),
	_discordUniqueIdentifier(std::move(unique_identifier))
{
}

QString LaunchInstance::id() const
{
	return this->_id;
}

QString LaunchInstance::name() const
{
	return _name;
}

void LaunchInstance::setName(const QString &name)
{
	if (_name == name) return;
	_name = name;

	emit nameChanged();
}

QString LaunchInstance::groupName() const
{
	return _groupName;
}

void LaunchInstance::setGroupName(const QString &groupName)
{
	if (_groupName == groupName) return;
	_groupName = groupName;

	emit groupNameChanged();
}

QString LaunchInstance::discordUniqueIdentifier() const
{
	return this->_discordUniqueIdentifier;
}

QString LaunchInstance::folderPath() const
{
	if (_id.isEmpty()) return QString();

	QString path = QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).filePath(_id);
	QDir().mkpath(path);

	return path;
}

QString LaunchInstance::logPath() const
{
	return QDir(folderPath()).filePath("discord.log");
}

QString LaunchInstance::logPathRead() const
{
	QFile file(logPath());
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream in(&file);
		return in.readAll();
	}

	return QString();
}

QString LaunchInstance::notes() const
{
	return _notes;
}

void LaunchInstance::setNotes(const QString &notes)
{
	if (_notes == notes) return;
	_notes = notes;

	emit notesChanged();
}

QString LaunchInstance::entryPointPath() const
{
	return _entryPointPath;
}

void LaunchInstance::setEntryPointPath(const QString &path)
{
	if (_entryPointPath == path) return;
	_entryPointPath = path;
	emit entryPointPathChanged();
}

QString LaunchInstance::launchArguments() const
{
	return _launchArguments;
}

void LaunchInstance::setLaunchArguments(const QString &args)
{
	if (_launchArguments == args) return;
	_launchArguments = args;
	emit launchArgumentsChanged();
}

QVariantMap LaunchInstance::environmentVariables() const
{
	return _environmentVariables;
}

void LaunchInstance::setEnvironmentVariables(const QVariantMap &env)
{
	if (_environmentVariables == env) return;
	_environmentVariables = env;

	emit environmentVariablesChanged();
}

bool LaunchInstance::enableLogging() const
{
	return _enableLogging;
}

void LaunchInstance::setEnableLogging(bool enable)
{
	if (_enableLogging == enable) return;

	_enableLogging = enable;
	emit enableLoggingChanged();
}

bool LaunchInstance::enableLoggingWrap() const
{
	return _enableLoggingWrap;
}

void LaunchInstance::setEnableLoggingWrap(bool wrap)
{
	if (_enableLoggingWrap == wrap) return;

	_enableLoggingWrap = wrap;
	emit enableLoggingWrapChanged();
}

QString LaunchInstance::updaterGithubUrl() const
{
	return _updaterGithubUrl;
}

void LaunchInstance::setUpdaterGithubUrl(const QString &url)
{
	if (_updaterGithubUrl == url) return;
	_updaterGithubUrl = url;

	emit updaterGithubUrlChanged();
}

QString LaunchInstance::updaterFiles() const
{
	return _updaterFiles;
}

void LaunchInstance::setUpdaterFiles(const QString &files)
{
	if (_updaterFiles == files) return;
	_updaterFiles = files;

	emit updaterFilesChanged();
}

QString LaunchInstance::updaterFileOutputPath() const
{
	return _updaterFileOutputPath;
}

#include <QDir>

void LaunchInstance::setUpdaterFileOutputPath(const QString &path)
{
    if (_updaterFileOutputPath == path) return;
    if (!QDir().mkpath(path)) return;

    _updaterFileOutputPath = path;
    emit updaterFileOutputPathChanged();
}

#include "../../shared/shared.hpp" // OurExecutablePath

bool LaunchInstance::launch() const
{
	QProcess process;

	QStringList args;
	args.append("-i");
	args.append(_id);

	process.setProgram(QString::fromStdString(shared::OurExecutablePath().string()));
	process.setArguments(args);
	process.startDetached();
	return true;
}

#include "PlatformSpecific.hpp"

bool LaunchInstance::createShortcut() const {
#if defined(Q_OS_WIN32) || defined(Q_OS_LINUX)
	return platform::createShortcut(
		_name,
		_id,
		QString::fromStdString(shared::OurExecutablePath().string()),
		QString("-i %1").arg(_id),
		""
	);
#endif
	return false;
}

bool LaunchInstance::saveInstance() const
{
	if (_id.isEmpty()) return false;

	QSettings settings;
	settings.beginGroup("Instances");
	settings.beginGroup(_id);

	settings.setValue("name", _name);
	settings.setValue("groupName", _groupName);
	settings.setValue("discordUniqueIdentifier", _discordUniqueIdentifier);
	settings.setValue("notes", _notes);
	settings.setValue("entryPointPath", _entryPointPath);
	settings.setValue("environmentVariables", _environmentVariables);
	settings.setValue("updaterGithubUrl", _updaterGithubUrl);
	settings.setValue("updaterFiles", _updaterFiles);
	settings.setValue("updaterFileOutputPath", _updaterFileOutputPath);

	settings.endGroup();
	settings.endGroup();

	return true;
}

bool LaunchInstance::removeInstance() const
{
	if (_id.isEmpty()) return false;

	QSettings settings;
	settings.beginGroup("Instances");
	settings.remove(_id);
	settings.endGroup();

	QString path = folderPath();
	if (!path.isEmpty()) {
		QDir dir(path);
		if (dir.exists()) {
			dir.removeRecursively();
		}
	}

#if defined(Q_OS_WIN32) || defined(Q_OS_LINUX)
	platform::deleteShortcut(_id);
#endif

	return true;
}

QList<LaunchInstance*> LaunchInstance::getInstances()
{
	QList<LaunchInstance*> result;
	QSettings settings;

	settings.beginGroup("Instances");
	QStringList ids = settings.childGroups();

	for (const QString &id : ids) {
		settings.beginGroup(id);

		QString name = settings.value("name").toString();
		QString groupName = settings.value("groupName").toString();
		QString discordId = settings.value("discordUniqueIdentifier").toString();
		QString notes = settings.value("notes").toString();
		QString pathStr = settings.value("entryPointPath").toString();
		QVariantMap env = settings.value("environmentVariables").toMap();
		QString updaterGithubUrl = settings.value("updaterGithubUrl").toString();
		QString updaterFiles = settings.value("updaterFiles").toString();
		QString updaterFileOutputPath = settings.value("updaterFileOutputPath").toString();

		auto *instance = new LaunchInstance(nullptr, id, discordId);
		instance->setName(name);
		instance->setGroupName(groupName);
		instance->setNotes(notes);

		if (!pathStr.isEmpty())
			instance->setEntryPointPath(pathStr);

		instance->setEnvironmentVariables(env);
		instance->setUpdaterGithubUrl(updaterGithubUrl);
		instance->setUpdaterFiles(updaterFiles);
		instance->setUpdaterFileOutputPath(updaterFileOutputPath);

		result.append(instance);
		settings.endGroup();
	}

	settings.endGroup();
	return result;
}

LaunchInstance* LaunchInstance::getInstanceById(const QString &id)
{
	if (id.isEmpty())
		return nullptr;

	QSettings settings;
	settings.beginGroup("Instances");

	if (!settings.childGroups().contains(id)) {
		settings.endGroup();
		return nullptr;
	}

	settings.beginGroup(id);

	QString name = settings.value("name").toString();
	QString groupName = settings.value("groupName").toString();
	QString discordId = settings.value("discordUniqueIdentifier").toString();
	QString notes = settings.value("notes").toString();
	QString pathStr = settings.value("entryPointPath").toString();
	QVariantMap env = settings.value("environmentVariables").toMap();

	QString updaterGithubUrl = settings.value("updaterGithubUrl").toString();
	QString updaterFiles = settings.value("updaterFiles").toString();
	QString updaterFileOutputPath = settings.value("updaterFileOutputPath").toString();

	settings.endGroup();
	settings.endGroup();

	auto *instance = new LaunchInstance(nullptr, id, discordId);
	instance->setName(name);
	instance->setGroupName(groupName);
	instance->setNotes(notes);

	if (!pathStr.isEmpty())
		instance->setEntryPointPath(pathStr);

	instance->setEnvironmentVariables(env);
	instance->setUpdaterGithubUrl(updaterGithubUrl);
	instance->setUpdaterFiles(updaterFiles);
	instance->setUpdaterFileOutputPath(updaterFileOutputPath);

	return instance;
}

#ifdef Q_OS_WIN32
#include <windows.h>
#include <detours.h>
#include <ShlObj.h>
#endif

#include "LaunchInstanceUpdater.hpp"

void LaunchInstance::launchById(const QString &id, const QStringList &argv) {
	LaunchInstance *instance = LaunchInstance::getInstanceById(id);
	if (!instance) return;
	DiscordInstall *discordInstall = DiscordInstall::getInstanceByUniqueIdentifier(instance->discordUniqueIdentifier());
	if (!discordInstall) return;

	// updater
	if (!instance->updaterGithubUrl().isEmpty()) {
		QStringList _filesToDownload = instance->updaterFiles().split(", ", Qt::SkipEmptyParts);

		if (!_filesToDownload.isEmpty()) {
			LaunchInstanceUpdater updater(
				instance->updaterFileOutputPath(),
				instance->updaterGithubUrl(),
				_filesToDownload
			);
			QEventLoop loop;

			QObject::connect(&updater, &LaunchInstanceUpdater::updateFinished, &loop, [&loop](bool success) {
				Q_UNUSED(success);
				loop.quit();
			});

			updater.checkForUpdates();
			loop.exec();
		}
	}

	QJsonArray argvJson;
	for (const auto &arg : argv) argvJson.append(arg);

	auto env = QProcessEnvironment::systemEnvironment();
	env.insert("DISABLE_UPDATER_AUTO_PATCHING", "true");
	env.insert("CLIENT_EXECUTABLE_PATH", QString::fromStdString(shared::OurExecutablePath().string()));
	env.insert("HOOK_DLL_PATH", QString::fromStdString(shared::OurHookLibraryPath().string()));
	env.insert("CLIENT_ARGV_JSON", QJsonDocument(argvJson).toJson(QJsonDocument::Compact));
	env.insert("CLIENT_JS_PATH", instance->entryPointPath());

	QVariantMap _customEnv = instance->environmentVariables();
	for (auto it = _customEnv.constBegin(); it != _customEnv.constEnd(); ++it) {
		env.insert(it.key(), it.value().toString());
	}

#ifdef Q_OS_LINUX
	if (!instance->entryPointPath().isEmpty()) {
		env.insert("LD_PRELOAD", QString::fromStdString(shared::OurHookLibraryPath().string()));
	}

	QProcess process;
	process.setProcessEnvironment(env);
	process.setProgram(discordInstall->exe());
	const QStringList _arguments = instance->launchArguments().split(' ', Qt::SkipEmptyParts);
	process.setArguments(_arguments);
	process.setStandardOutputFile(instance->logPath(), QIODevice::Truncate);
	process.setStandardErrorFile(instance->logPath(), QIODevice::Append);
	process.startDetached();
	return;
#elif defined(Q_OS_WIN32)
	std::vector<wchar_t> environmentBlock;

	for (const auto &key : env.keys()) {
		const std::wstring environmentEntry =
			(key + '=' + env.value(key)).toStdWString();

		environmentBlock.insert(
			environmentBlock.end(),
			environmentEntry.begin(),
			environmentEntry.end());

		environmentBlock.push_back(L'\0');
	}

	environmentBlock.push_back(L'\0');

	std::wstring commandLine = L"\"" + discordInstall->exe().toStdWString() + L"\"";
	if (!instance->launchArguments().isEmpty()) {
		commandLine += L" " + instance->launchArguments().toStdWString();
	}

	std::vector<wchar_t> mutableCommandLine(
		commandLine.begin(), commandLine.end());

	mutableCommandLine.push_back(L'\0');

	SECURITY_ATTRIBUTES sa{};
	sa.nLength		  = sizeof(sa);
	sa.bInheritHandle = TRUE;

	HANDLE hLogFile = CreateFileW(
		instance->logPath().toStdWString().c_str(),
		FILE_WRITE_DATA | FILE_APPEND_DATA,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&sa,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (hLogFile == INVALID_HANDLE_VALUE) {
		qWarning() << "Failed to create log file for redirection:" << GetLastError();
		return;
	}

	STARTUPINFOW startupInfo{};
	startupInfo.cb		   = sizeof(startupInfo);
	startupInfo.dwFlags	   = STARTF_USESTDHANDLES;
	startupInfo.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
	startupInfo.hStdOutput = hLogFile;
	startupInfo.hStdError  = hLogFile;

	PROCESS_INFORMATION processInfo{};

	const std::wstring executablePath = discordInstall->exe().toStdWString();
	const std::string  dllPath		  = shared::OurHookLibraryPath().string();

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
			nullptr)) {
		qWarning() << "DetourCreateProcessWithDllExW failed:" << GetLastError();
		CloseHandle(hLogFile);
		return;
	}

	CloseHandle(hLogFile);
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);
#endif // Q_OS_LINUX
}
