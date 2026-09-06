#include "dl.hpp"

#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

namespace {
    constexpr auto ReleaseUrl =
        "https://api.github.com/repos/Vendicated/Vencord/releases/latest";

    QNetworkReply* get(
        QNetworkAccessManager& manager,
        const QUrl& url
    ) {
        QNetworkRequest request(url);

        request.setHeader(
            QNetworkRequest::UserAgentHeader,
            "VencordInstaller"
        );

        auto* reply = manager.get(request);

        QEventLoop loop;

        QObject::connect(
            reply,
            &QNetworkReply::finished,
            &loop,
            &QEventLoop::quit
        );

        loop.exec();

        return reply;
    }

	QString installedHash()
	{
		QFile file(
			QDir::home().filePath(
				".config/Vencord/dist/patcher.js"
			)
		);

		if (!file.open(QIODevice::ReadOnly))
			return "None";

		const auto line =
			QString::fromUtf8(file.readLine()).trimmed();

		if (!line.startsWith("// Vencord "))
			return "None";

		return line.mid(QString("// Vencord ").length());
	}

}

#include <QDebug>


UpdateInfo CheckForUpdates()
{
    QNetworkAccessManager manager;

    auto* reply = get(
        manager,
        QUrl(ReleaseUrl)
    );

    const auto installed = installedHash();

    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();

        return {
            "Unknown",
            installed,
            false
        };
    }

    const auto json =
        QJsonDocument::fromJson(reply->readAll());

    reply->deleteLater();

    if (!json.isObject()) {
        return {
            "Unknown",
            installed,
            false
        };
    }

    const auto name =
        json.object()["name"].toString();

    const auto latestHash =
        name.mid(name.lastIndexOf(' ') + 1);

	const bool upToDate =
        latestHash == installed;

	qDebug() << "Installed hash:" << installed;
    qDebug() << "Latest hash:" << latestHash;
    qDebug() << "Up to date:" << upToDate;

    return {
        latestHash,
        installed,
        upToDate,
    };
}

#include <QSet>
#include <QDebug>

bool WritePackageJson(const QString& outputDir)
{
    QFile file(
        QDir(outputDir).filePath("package.json")
    );

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    return file.write("{}") == 2;
}


bool DownloadLatest(const QString& outputDir)
{
	if (!WritePackageJson(outputDir)) return false;

    QNetworkAccessManager manager;

    auto* reply = get(
        manager,
        QUrl(ReleaseUrl)
    );

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Failed to fetch release:"
                 << reply->errorString();

        reply->deleteLater();
        return false;
    }

    const auto json =
        QJsonDocument::fromJson(reply->readAll());

    reply->deleteLater();

    if (!json.isObject()) {
        qDebug() << "Invalid GitHub response";
        return false;
    }

    QDir dir(outputDir);

    if (!dir.exists() && !dir.mkpath(".")) {
        qDebug() << "Failed to create output directory:"
                 << outputDir;
        return false;
    }

    const QSet<QString> required = {
        "patcher.js",
        "preload.js",
        "renderer.js",
        "renderer.css"
    };

    QSet<QString> downloaded;

    for (const auto& value : json.object()["assets"].toArray()) {
        const auto asset = value.toObject();

        const auto name =
            asset["name"].toString();

        if (!required.contains(name))
            continue;

        const auto url =
            asset["browser_download_url"].toString();

        qDebug() << "Downloading:" << name;

        auto* assetReply = get(
            manager,
            QUrl(url)
        );

        if (assetReply->error() != QNetworkReply::NoError) {
            qDebug() << "Failed to download"
                     << name << ":"
                     << assetReply->errorString();

            assetReply->deleteLater();
            return false;
        }

        QFile file(dir.filePath(name));

        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "Failed to create:"
                     << file.fileName();

            assetReply->deleteLater();
            return false;
        }

        const auto data = assetReply->readAll();

        if (file.write(data) != data.size()) {
            qDebug() << "Failed to write:"
                     << file.fileName();

            file.close();
            assetReply->deleteLater();
            return false;
        }

        file.close();
        assetReply->deleteLater();

        downloaded.insert(name);

        qDebug() << "Downloaded:" << name;
    }

    qDebug() << "Downloaded"
             << downloaded.size()
             << "of"
             << required.size()
             << "required files";

    for (const auto& name : required) {
        if (!downloaded.contains(name)) {
            qDebug() << "Missing:" << name;
            return false;
        }
    }

    return true;
}
