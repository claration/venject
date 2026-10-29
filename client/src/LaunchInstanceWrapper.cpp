#include "LaunchInstanceWrapper.hpp"

QList<LaunchInstance *> LaunchInstanceWrapper::getInstances()
{
	return LaunchInstance::getInstances();
}

LaunchInstance *LaunchInstanceWrapper::createInstance(
	const QString &name,
	const QString &groupName,
	const QString &iconPath,
	const QString &discordUniqueIdentifier,
	const QString &preset)
{
	QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);

	auto *instance = new LaunchInstance(nullptr, id, discordUniqueIdentifier);
	instance->setName(name);
	instance->setGroupName(groupName);
	instance->setIconPath(iconPath);

	if (preset == "vencord") {
		QString patcherPath = QDir(instance->folderPath()).filePath("dist/patcher.js");
		instance->setEntryPointPath(patcherPath);

		QVariantMap env = instance->environmentVariables();
		env.insert("VENCORD_USER_DATA_DIR", instance->folderPath());
		instance->setEnvironmentVariables(env);

		QString updaterPath = QDir(instance->folderPath()).filePath("dist");
		instance->setUpdaterFileOutputPath(updaterPath);
		instance->setUpdaterGithubUrl("https://api.github.com/repos/Vendicated/Vencord/releases/latest");
		instance->setUpdaterFiles("patcher.js, preload.js, renderer.js, renderer.css");
	} else if (preset == "equicord") {
		QString patcherPath = QDir(instance->folderPath()).filePath("dist/patcher.js");
		instance->setEntryPointPath(patcherPath);

		QVariantMap env = instance->environmentVariables();
		env.insert("EQUICORD_USER_DATA_DIR", instance->folderPath());
		instance->setEnvironmentVariables(env);

		QString updaterPath = QDir(instance->folderPath()).filePath("dist");
		instance->setUpdaterFileOutputPath(updaterPath);
		instance->setUpdaterGithubUrl("https://api.github.com/repos/Equicord/Equicord/releases/latest");
		instance->setUpdaterFiles("patcher.js, preload.js, renderer.js, renderer.css");
	}

	instance->saveInstance();
	return instance;
}

QString LaunchInstanceWrapper::getSupportedIconTypes() {
	return LaunchInstance::getSupportedIconTypes();
}

QString LaunchInstanceWrapper::getIconType() {
	return LaunchInstance::getIconType();
}
