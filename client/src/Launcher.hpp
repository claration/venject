#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <qqmlintegration.h>

class Launcher : public QObject
{
	Q_OBJECT
	QML_SINGLETON
	QML_ELEMENT

  public:
	explicit Launcher(QObject *parent = nullptr);

	Q_INVOKABLE void		 launchSelf(const QString &path, const QStringList &args = {}, const QString &entry_point_path = {});
	Q_INVOKABLE void		 launch(const QString &path, const QStringList &args = {}, const QStringList &argv = {});
	Q_INVOKABLE QVariantList findDiscords();
	Q_INVOKABLE QVariantMap	 checkForUpdates();
	Q_INVOKABLE QVariantMap	 updateVencord();
};
