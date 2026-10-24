#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QtQmlIntegration>

class Launcher : public QObject
{
	Q_OBJECT
	QML_SINGLETON
	QML_ELEMENT

  public:
	explicit Launcher(QObject *parent = nullptr);

	Q_INVOKABLE void		 launchSelf(const QString &path, const QString &discord_args = "", const QString &entry_point_path = "", const QString &data_dir_path = "");
	Q_INVOKABLE void		 launch(const QString &path, const QString &args = "", const QString &entry = "", const QString &data = "", const QStringList &argv = {});
	Q_INVOKABLE QVariantList findDiscords();
	Q_INVOKABLE QVariantMap	 checkForUpdates();
	Q_INVOKABLE QVariantMap	 updateVencord();

	Q_INVOKABLE QString uniqueLogPathForFile(const QString &id);
	Q_INVOKABLE QString readTextFile(const QString &id);

	Q_INVOKABLE void createShortcut(const QString &uniqueIdentifier, const QString &branch, bool autoStart = false);
};
