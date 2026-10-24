#pragma once

#include <QObject>
#include <QtQmlIntegration>

class DiscordInstall : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QDir path READ path NOTIFY pathChanged)
	Q_PROPERTY(QDir folder READ folder NOTIFY folderChanged)
	Q_PROPERTY(QDir)
	QML_ELEMENT

	QDir	_path;
	QDir	_folder;
	QString _uniqueIdentifier;
	QString _branch;
	bool	_isFlatpak = false;

  public:
	DiscordInstall(
		QObject	   *parent,
		const QDir &path,
		const QDir &folder,
		QString		unique_identifier,
		QString		branch,
		bool		is_flatpak);

	[[nodiscard]] QDir	  path() const;
	[[nodiscard]] QDir	  folder() const;
	[[nodiscard]] QString uniqueIdentifier() const;
	[[nodiscard]] QString branch() const;
	[[nodiscard]] bool	  isFlatpak() const;

  signals:
	void pathChanged();
	void folderChanged();
	void uniqueIdentifierChanged();
	void branchChanged();
	void isFlatpakChanged();
};
