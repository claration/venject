#include "network.hpp"

#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

#include "globals.h"
#include "paths/paths.hpp"
#include "../../shared/shared.hpp"

namespace
{

UpdateResult fail(const QString &error)
{
	return {
		.success = false,
		.error	 = error
	};
}

QNetworkReply *get(
	QNetworkAccessManager &manager,
	const QUrl			  &url,
	int					   redirectCount = 0)
{
	if (redirectCount > 5) return nullptr;

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::UserAgentHeader, MOD_NAME);

	auto *reply = manager.get(request);

	QEventLoop loop;
	QTimer	   timer;

	timer.setSingleShot(true);
	timer.start(NETWORK_TIMEOUT);

	QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

	loop.exec();

	if (!reply->isFinished()) {
		reply->abort();
		return reply;
	}

	const auto status = reply->attribute(
		QNetworkRequest::HttpStatusCodeAttribute
	).toInt();

	if (status == 301 || status == 302 || status == 303 || status == 307 || status == 308) {
		QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
		if (redirectUrl.isRelative()) {
			redirectUrl = url.resolved(redirectUrl);
		}

		reply->deleteLater();

		if (redirectUrl.isValid()) {
			return get(manager, redirectUrl, redirectCount + 1);
		}
	}

	return reply;
}

bool successful(QNetworkReply *reply)
{
	if (!reply || reply->error() != QNetworkReply::NoError)
		return false;

	const auto status = reply->attribute(
		QNetworkRequest::HttpStatusCodeAttribute
	).toInt();

	return status >= 200 && status < 300;
}

UpdateResult parseRelease(
	const QByteArray &data,
	const QString	 &installed)
{
	QJsonParseError error;

	const auto json = QJsonDocument::fromJson(data, &error);
	if (error.error != QJsonParseError::NoError)
		return fail(error.errorString());
	if (!json.isObject()) return fail("Invalid GitHub response");

	const auto name	 = json.object()["name"].toString().trimmed();
	const auto index = name.lastIndexOf(' ');
	if (index < 0) return fail("Invalid release name");

	const auto latest = name.mid(index + 1).trimmed();
	if (latest.isEmpty()) return fail("Invalid release hash");

	return {
		.success	   = true,
		.latestHash	   = latest,
		.installedHash = installed,
		.upToDate	   = latest == installed};
}

UpdateResult downloadAsset(
	QNetworkAccessManager &manager,
	const QString		  &name,
	const QUrl			  &url,
	const QDir			  &dir)
{
	if (!url.isValid() || url.scheme() != "https")
		return fail("Invalid download URL for " + name);

	auto *reply = get(manager, url);

	if (!successful(reply)) {
		const auto error = reply ? reply->errorString() : "Unknown network error";
		if (reply) reply->deleteLater();
		return fail(QString("Failed to download %1: %2").arg(name, error));
	}

	const auto data = reply->readAll();
	reply->deleteLater();
	if (data.isEmpty()) 
		return fail("Downloaded file is empty: " + name);

	QFile file(dir.filePath(name));

	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return fail(QString("Failed to write %1: %2").arg(name, file.errorString()));

	if (file.write(data) != data.size())
		return fail(QString("Failed to write %1: %2").arg(name, file.errorString()));

	return {
		.success = true
	};
}

} // namespace

namespace network
{

QString installedHash()
{
	QFile file(shared::ModEntrypointPath());
	if (!file.open(QIODevice::ReadOnly)) return "None";

	const auto line = QString::fromUtf8(file.readLine()).trimmed();
	if (!line.startsWith(MOD_VERSION_PREFIX)) return "None";

	return line
		.mid(QString(MOD_VERSION_PREFIX).size())
		.trimmed();
}

UpdateResult CheckForUpdates()
{
	const auto installed = installedHash();

	QNetworkAccessManager manager;

	auto *reply = get(manager, QUrl(MOD_RELEASE_GH));

	if (!successful(reply)) {
		const auto error = reply ? reply->errorString() : "Unknown network error";
		if (reply) reply->deleteLater();
		return fail(error);
	}

	const auto data = reply->readAll();
	reply->deleteLater();

	auto result = parseRelease(data, installed);
	return result;
}

UpdateResult DownloadLatest()
{
	QNetworkAccessManager manager;

	auto *reply = get(manager, QUrl(MOD_RELEASE_GH));

	if (!successful(reply)) {
		const auto error = reply ? reply->errorString() : "Unknown network error";
		if (reply) reply->deleteLater();
		return fail(error);
	}

	const auto data = reply->readAll();
	reply->deleteLater();

	const auto json = QJsonDocument::fromJson(data);
	if (!json.isObject()) return fail("Invalid GitHub response");

	const auto assets = json.object()["assets"].toArray();
	if (assets.isEmpty()) return fail("No release assets found");

	QDir dir(shared::ModDistPath());
	if (!dir.exists() && !dir.mkpath("."))
		return fail("Failed to create target directory: " + dir.path());

	static const QStringList required = {
		"patcher.js",
		"preload.js",
		"renderer.js",
		"renderer.css"};

	QSet<QString> found;

	for (const auto &value : assets) {
		const auto asset = value.toObject();

		const auto name = asset["name"].toString();
		if (!required.contains(name)) continue;

		const auto downloadUrlStr = asset["browser_download_url"].toString();
		const QUrl url(downloadUrlStr);

		const auto result = downloadAsset(manager, name, url, dir);
		if (!result.success) {
			return result;
		}

		found.insert(name);
	}

	for (const auto &name : required) {
		if (!found.contains(name))
			return fail("Missing required asset: " + name);
	}

	return {
		.success = true
	};
}

} // namespace network
