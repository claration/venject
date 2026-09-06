#pragma once

#include <QString>

struct UpdateInfo {
    QString latestHash;
    QString installedHash;
    bool upToDate;
};

UpdateInfo CheckForUpdates();
bool DownloadLatest(const QString& outputDir);
