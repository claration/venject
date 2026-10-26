#include "LaunchInstanceUpdater.hpp"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>


LaunchInstanceUpdater::LaunchInstanceUpdater(const QString &outDir,
											const QString &githubApiUrl,
											const QStringList &fileNamesToDownload,
											QObject *parent)
	: QObject(parent),
	m_outDir(outDir),
	m_githubApiUrl(githubApiUrl),
	m_fileNamesToDownload(fileNamesToDownload) {}

void LaunchInstanceUpdater::checkForUpdates() {
	QNetworkRequest request{QUrl(m_githubApiUrl)};
	request.setHeader(QNetworkRequest::UserAgentHeader, "LaunchInstanceUpdater-Client");
	request.setRawHeader("Accept", "application/vnd.github.v3+json");

	m_currentReply = m_networkManager.get(request);
	connect(m_currentReply, &QNetworkReply::finished, this, &LaunchInstanceUpdater::handleReleaseCheckReply);
}

void LaunchInstanceUpdater::handleReleaseCheckReply()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	if (!reply)
		return;

	qDebug() << "handleReleaseCheckReply";

	const QNetworkReply::NetworkError networkError = reply->error();
	const QString networkErrorString = reply->errorString();
	const QByteArray data = reply->readAll();

	reply->deleteLater();
	m_currentReply = nullptr;

	if (networkError != QNetworkReply::NoError) {
		qWarning() << "Failed to check for updates:" << networkErrorString;
		emit updateFinished(false);
		return;
	}

	QJsonParseError parseError;
	const QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

	if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
		qWarning() << "Invalid JSON response from GitHub Releases API:"
				<< parseError.errorString();
		emit updateFinished(false);
		return;
	}

	const QJsonObject obj = doc.object();
	const QString tagName = obj.value("tag_name").toString();

	m_latestReleaseName = obj.value("name").toString();
	if (m_latestReleaseName.isEmpty())
		m_latestReleaseName = tagName;

	if (tagName.isEmpty()) {
		qWarning() << "Could not find 'tag_name' in release response.";
		emit updateFinished(false);
		return;
	}

	const QString localVersion = getLocalVersion();

	qDebug() << "Local version:" << localVersion << "| Latest release:" << m_latestReleaseName;

	if (localVersion == m_latestReleaseName) {
		qDebug() << "Application is already up to date.";
		emit updateFinished(true);
		return;
	}

	m_assetsToDownload.clear();

	const QJsonArray assetsArray = obj.value("assets").toArray();

	for (const QJsonValue &assetValue : assetsArray) {
		const QJsonObject assetObj = assetValue.toObject();

		const QString name = assetObj.value("name").toString();
		const QString downloadUrl =
			assetObj.value("browser_download_url").toString();

		if (m_fileNamesToDownload.contains(name) && !downloadUrl.isEmpty()) {
			m_assetsToDownload.append({name, QUrl(downloadUrl)});
		}
	}

	if (m_assetsToDownload.isEmpty()) {
		qWarning() << "No matching release assets found.";

		QMessageBox box(
			QMessageBox::Warning,
			"Update Error",
			"None of the specified files were found in the latest release assets.",
			QMessageBox::Ok,
			QApplication::activeWindow()
		);
		box.exec();

		emit updateFinished(false);
		return;
	}

	if (localVersion.isEmpty()) {
		qDebug() << "Local version is empty; downloading automatically.";

		m_downloadIndex = 0;
		startNextDownload();
		return;
	}

	qDebug() << "Showing update prompt";

	QMessageBox box(
		QMessageBox::Question,
		"Update Available",
		QString(
			"A new release is available (%1).\n\n"
			"Do you want to download and install the update now?"
		).arg(m_latestReleaseName),
		QMessageBox::Yes | QMessageBox::No,
		QApplication::activeWindow()
	);

	box.setDefaultButton(QMessageBox::Yes);
	box.setEscapeButton(QMessageBox::No);

	const QMessageBox::StandardButton choice =
		static_cast<QMessageBox::StandardButton>(box.exec());

	if (choice == QMessageBox::Yes) {
		m_downloadIndex = 0;
		startNextDownload();
	} else {
		emit updateFinished(true);
	}
}

QString LaunchInstanceUpdater::getLocalVersion() const {
	QFile file(QDir(m_outDir).filePath("version.txt"));
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream in(&file);
		return in.readLine().trimmed();
	}
	return QString();
}

void LaunchInstanceUpdater::saveLocalVersion(const QString &version) {
	QDir().mkpath(m_outDir);
	QFile file(QDir(m_outDir).filePath("version.txt"));
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&file);
		out << version;
	}
}

void LaunchInstanceUpdater::startNextDownload() {
	if (m_downloadIndex >= m_assetsToDownload.size()) {
		saveLocalVersion(m_latestReleaseName);
		QMessageBox::information(nullptr, "Update Complete", "Selected files have been successfully updated!");
		emit updateFinished(true);
		return;
	}

	const auto &asset = m_assetsToDownload.at(m_downloadIndex);
	QNetworkRequest request{asset.url};
	request.setHeader(QNetworkRequest::UserAgentHeader, "LaunchInstanceUpdater-Client");

	m_currentReply = m_networkManager.get(request);
	connect(m_currentReply, &QNetworkReply::finished, this, &LaunchInstanceUpdater::handleFileDownloadReply);
}

void LaunchInstanceUpdater::handleFileDownloadReply() {
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	if (!reply) return;

	QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	if (!redirectionTarget.isNull()) {
		reply->deleteLater();
		QUrl redirectUrl = redirectionTarget.toUrl();
		QNetworkRequest request{redirectUrl};
		request.setHeader(QNetworkRequest::UserAgentHeader, "LaunchInstanceUpdater-Client");
		m_currentReply = m_networkManager.get(request);
		connect(m_currentReply, &QNetworkReply::finished, this, &LaunchInstanceUpdater::handleFileDownloadReply);
		return;
	}

	reply->deleteLater();
	m_currentReply = nullptr;

	if (reply->error() != QNetworkReply::NoError) {
		QMessageBox::critical(nullptr, "Update Error", QString("Failed to download file: %1").arg(reply->errorString()));
		emit updateFinished(false);
		return;
	}

	const auto &asset = m_assetsToDownload.at(m_downloadIndex);
	QString targetPath = QDir(m_outDir).filePath(asset.name);

	QDir().mkpath(m_outDir);

	QFile file(targetPath);
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(nullptr, "Update Error", QString("Failed to write to local file: %1").arg(targetPath));
		emit updateFinished(false);
		return;
	}

	file.write(reply->readAll());
	file.close();

	m_downloadIndex++;
	startNextDownload();
}
