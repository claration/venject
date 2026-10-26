#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class LaunchInstanceUpdater : public QObject {
	Q_OBJECT
public:
	LaunchInstanceUpdater(const QString &outDir,
						const QString &githubApiUrl,
						const QStringList &fileNamesToDownload,
						QObject *parent = nullptr);

	void checkForUpdates();

signals:
	void updateFinished(bool success);

private slots:
	void handleReleaseCheckReply();
	void handleFileDownloadReply();

private:
	QString getLocalVersion() const;
	void saveLocalVersion(const QString &version);
	void startNextDownload();

	QString m_outDir;
	QString m_githubApiUrl;
	QStringList m_fileNamesToDownload;

	QNetworkAccessManager m_networkManager;
	QNetworkReply *m_currentReply = nullptr;

	struct AssetInfo {
		QString name;
		QUrl url;
	};
	QList<AssetInfo> m_assetsToDownload;
	int m_downloadIndex = 0;
	QString m_latestReleaseName;
};
