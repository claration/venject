#pragma once

#include <QObject>
#include <QtQmlIntegration>
#include <QDir>
#include <QString>
#include <QVariantMap>
#include <QList>

class DiscordInstall;

class LaunchInstance : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString id READ id CONSTANT)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QString groupName READ groupName WRITE setGroupName NOTIFY groupNameChanged)
	Q_PROPERTY(QString iconPath READ iconPath WRITE setIconPath NOTIFY iconPathChanged)
	Q_PROPERTY(QString discordUniqueIdentifier READ discordUniqueIdentifier CONSTANT)
	Q_PROPERTY(QString folderPath READ folderPath CONSTANT)
	Q_PROPERTY(QString logPath READ logPath CONSTANT)
	Q_PROPERTY(QString logPathRead READ logPathRead CONSTANT)
	Q_PROPERTY(QString notes READ notes WRITE setNotes NOTIFY notesChanged)
	Q_PROPERTY(QString entryPointPath READ entryPointPath WRITE setEntryPointPath NOTIFY entryPointPathChanged)
	Q_PROPERTY(QString launchArguments READ launchArguments WRITE setLaunchArguments NOTIFY launchArgumentsChanged)
	Q_PROPERTY(QVariantMap environmentVariables READ environmentVariables WRITE setEnvironmentVariables NOTIFY environmentVariablesChanged)
	Q_PROPERTY(bool enableLogging READ enableLogging WRITE setEnableLogging NOTIFY enableLoggingChanged)
    Q_PROPERTY(bool enableLoggingWrap READ enableLoggingWrap WRITE setEnableLoggingWrap NOTIFY enableLoggingWrapChanged)

	Q_PROPERTY(QString updaterGithubUrl READ updaterGithubUrl WRITE setUpdaterGithubUrl NOTIFY updaterGithubUrlChanged)
	Q_PROPERTY(QString updaterFiles READ updaterFiles WRITE setUpdaterFiles NOTIFY updaterFilesChanged)
	Q_PROPERTY(QString updaterFileOutputPath READ updaterFileOutputPath WRITE setUpdaterFileOutputPath NOTIFY updaterFileOutputPathChanged)


	QML_ELEMENT
	QML_UNCREATABLE("LaunchInstance instances cannot be created directly in QML")

	QString     _id;
	QString     _name;
	QString     _groupName;
	QString     _iconPath;
	QString     _discordUniqueIdentifier;
	QString     _notes;
	QString     _entryPointPath;
	QString     _launchArguments;
	QVariantMap _environmentVariables;
	bool        _enableLogging = true;
	bool        _enableLoggingWrap = false;
	QString     _updaterGithubUrl;
	QString     _updaterFiles;
	QString     _updaterFileOutputPath;

public:
	explicit LaunchInstance(
		QObject *parent = nullptr,
		QString  id = QString(),
		QString  discordUniqueIdentifier = QString());

	[[nodiscard]] QString id() const;

	[[nodiscard]] QString name() const;
	void setName(const QString &name);

	[[nodiscard]] QString groupName() const;
	void setGroupName(const QString &groupName);

	[[nodiscard]] QString iconPath() const;
	void setIconPath(const QString &path);

	[[nodiscard]] QString discordUniqueIdentifier() const;
	[[nodiscard]] QString folderPath() const;
	[[nodiscard]] QString logPath() const;
	[[nodiscard]] QString logPathRead() const;

	[[nodiscard]] QString notes() const;
	void setNotes(const QString &notes);

	[[nodiscard]] QString entryPointPath() const;
	void setEntryPointPath(const QString &path);

	[[nodiscard]] QString launchArguments() const;
	void setLaunchArguments(const QString &args);

	[[nodiscard]] QVariantMap environmentVariables() const;
	void setEnvironmentVariables(const QVariantMap &env);

	[[nodiscard]] bool enableLogging() const;
	void setEnableLogging(bool enable);

	[[nodiscard]] bool enableLoggingWrap() const;
	void setEnableLoggingWrap(bool wrap);

	[[nodiscard]] QString updaterGithubUrl() const;
	void setUpdaterGithubUrl(const QString &url);

	[[nodiscard]] QString updaterFiles() const;
	void setUpdaterFiles(const QString &files);

	[[nodiscard]] QString updaterFileOutputPath() const;
	void setUpdaterFileOutputPath(const QString &path);

	Q_INVOKABLE void launch();
	Q_INVOKABLE bool createShortcut() const;
	Q_INVOKABLE bool saveInstance() const;
	Q_INVOKABLE bool removeInstance() const;

	Q_INVOKABLE static QList<LaunchInstance*> getInstances();
	Q_INVOKABLE [[nodiscard]] static LaunchInstance* getInstanceById(const QString &id);
	static void launchById(const QString &id, const QStringList &argv);
	Q_INVOKABLE static QString getSupportedIconTypes();
	Q_INVOKABLE static QString getIconType();

signals:
	void nameChanged();
	void groupNameChanged();
	void iconPathChanged();
	void notesChanged();
	void entryPointPathChanged();
	void launchArgumentsChanged();
	void environmentVariablesChanged();
	void enableLoggingChanged();
	void enableLoggingWrapChanged();
	void updaterGithubUrlChanged();
	void updaterFilesChanged();
	void updaterFileOutputPathChanged();

	void launchFinished(int exitCode, const QString &errorString);
};
