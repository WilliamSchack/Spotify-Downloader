#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <string>

class DownloadManager {
    public:
        void Download(const std::string& url) const;
};

#endif