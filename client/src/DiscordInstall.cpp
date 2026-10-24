#include "DiscordInstall.hpp"

#include <utility>

DiscordInstall::DiscordInstall(
	QObject	   *parent,
	const QDir &path,
	const QDir &folder,
	QString		unique_identifier,
	QString		branch,
	const bool	is_flatpak)
	: QObject(parent),
	  _path(path),
	  _folder(folder),
	  _uniqueIdentifier(std::move(unique_identifier)),
	  _branch(std::move(branch)),
	  _isFlatpak(is_flatpak)
{
}
QDir DiscordInstall::path() const
{
	return this->_path;
}

QDir DiscordInstall::folder() const
{
	return this->_folder;
}

QString DiscordInstall::uniqueIdentifier() const
{
	return this->_uniqueIdentifier;
}

QString DiscordInstall::branch() const
{
	return this->_branch;
}

bool DiscordInstall::isFlatpak() const
{
	return this->_isFlatpak;
}