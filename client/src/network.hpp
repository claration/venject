#pragma once

#include <QString>

struct UpdateResult
{
    bool success = false;
    QString error;

    QString latestHash;
    QString installedHash;

    bool upToDate = false;
};

namespace network
{
    UpdateResult CheckForUpdates();
    UpdateResult DownloadLatest();

    QString installedHash();
}
