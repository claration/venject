#pragma once

#include <QObject>
#include <QtQmlIntegration>
#include <QDir>
#include <QString>
#include <QList>

class DiscordInstall : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString exe READ exe NOTIFY exeChanged)
	Q_PROPERTY(QDir folder READ folder NOTIFY folderChanged)
	Q_PROPERTY(QString uniqueIdentifier READ uniqueIdentifier CONSTANT)
	Q_PROPERTY(QString branch READ branch CONSTANT)
	Q_PROPERTY(bool isFlatpak READ isFlatpak CONSTANT)
	Q_PROPERTY(bool isPatched READ isPatched CONSTANT)
	QML_ELEMENT
	QML_UNCREATABLE("DiscordInstall instances cannot be created directly in QML")

	QString _exe;
	QDir    _folder;
	QString _uniqueIdentifier;
	QString _branch;
	bool    _isPatched = false;
	bool    _isFlatpak = false;

public:
	explicit DiscordInstall(
		QObject       *parent,
		const QString &exe,
		const QDir    &folder,
		QString        unique_identifier,
		QString        branch,
		bool           is_flatpak,
		bool           is_patched);

	[[nodiscard]] QString exe() const;
	[[nodiscard]] QDir    folder() const;
	[[nodiscard]] QString uniqueIdentifier() const;
	[[nodiscard]] QString branch() const;
	[[nodiscard]] bool    isPatched() const;
	[[nodiscard]] bool    isFlatpak() const;

	Q_INVOKABLE static QList<DiscordInstall*> getInstances();
	Q_INVOKABLE [[nodiscard]] static DiscordInstall* getInstanceByUniqueIdentifier(const QString &uniqueIdentifier);

signals:
	void exeChanged();
	void folderChanged();
	void uniqueIdentifierChanged();
	void branchChanged();
	void isFlatpakChanged();
	void isPatchedChanged();
};
