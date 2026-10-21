#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <qqmlintegration.h>



// read text fiel
#include <QFile>
#include <QTextStream>

class Launcher : public QObject
{
	Q_OBJECT
	QML_SINGLETON
	QML_ELEMENT

  public:
	explicit Launcher(QObject *parent = nullptr);

	Q_INVOKABLE void		 launchSelf(const QString &path, const QString &args = "", const QString &entry = "");
	Q_INVOKABLE void		 launch(const QString &path, const QString &args = "", const QString &entry = "", const QStringList &argv = {});
	Q_INVOKABLE QVariantList findDiscords();
	Q_INVOKABLE QVariantMap	 checkForUpdates();
	Q_INVOKABLE QVariantMap	 updateVencord();

    Q_INVOKABLE QString readTextFile(const QString &path) {
        QString cleanPath = path;
        if (cleanPath.startsWith("file://")) {
            cleanPath.remove(0, 7);
        }
        
        QFile file(cleanPath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            return in.readAll();
        }
        return QString();
    }
};
