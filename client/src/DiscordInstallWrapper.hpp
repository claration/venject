#pragma once
#include <QObject>
#include <QtQmlIntegration>
#include <QList>
#include "DiscordInstall.hpp"

class DiscordInstallWrapper : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

public:
	explicit DiscordInstallWrapper(QObject *parent = nullptr) : QObject(parent) {}

	Q_INVOKABLE QList<DiscordInstall*> getInstances() {
		return DiscordInstall::getInstances();
	}
};
