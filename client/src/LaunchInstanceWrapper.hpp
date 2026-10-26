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

	Q_INVOKABLE QList<LaunchInstance*> getInstances() {
		return LaunchInstance::getInstances();
	}

	Q_INVOKABLE LaunchInstance* createInstance(
		const QString &name,
		const QString &groupName,
		const QString &discordUniqueIdentifier,
		const QString &preset
	) {
		QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);

		auto *instance = new LaunchInstance(nullptr, id, discordUniqueIdentifier);
		instance->setName(name);
		instance->setGroupName(groupName);

		if (preset == "vencord") {
			QString patcherPath = QDir(instance->folderPath()).filePath("dist/patcher.js");
			instance->setEntryPointPath(patcherPath);

			QVariantMap env = instance->environmentVariables();
			env.insert("VENCORD_USER_DATA_DIR", instance->folderPath());
			instance->setEnvironmentVariables(env);
		}

		instance->saveInstance();
		return instance;
	}
};
