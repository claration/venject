#pragma once
#include <QObject>
#include <QtQmlIntegration>
#include <QList>
#include "LaunchInstance.hpp"

class LaunchInstanceWrapper : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

public:
	explicit LaunchInstanceWrapper(QObject *parent = nullptr) : QObject(parent) {}

	Q_INVOKABLE QList<LaunchInstance*> getInstances();

	Q_INVOKABLE LaunchInstance * createInstance(
		const QString &name,
		const QString &groupName,
		const QString &iconPath,
		const QString &discordUniqueIdentifier,
		const QString &preset
	);

	Q_INVOKABLE QString getSupportedIconTypes();
	Q_INVOKABLE QString getIconType();
};
