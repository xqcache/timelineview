#pragma once

#include <QImage>
#include <QList>

namespace tl {

class TimelineMediaUtil {
public:
    struct MediaInfo {
        QList<QImage> images;
        QString path;
        QSize size;
        double fps { 0 };
        double duration { 0 };
    };

    static std::optional<MediaInfo> loadMedia(const QString& path);

    static QString mediaInfoString(const MediaInfo& info);
};

} // namespace tl
