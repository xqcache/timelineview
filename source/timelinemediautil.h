#pragma once

#include <QImage>
#include <QList>

namespace tl {

class TimelineMediaUtil {
public:
    struct MediaInfo {
        QString path;
        QSize size;
        double fps { 0 };
        double duration { 0 };
        int frame_count { 0 };
    };

    static std::optional<MediaInfo> loadVideo(const QString& path);
    static std::optional<MediaInfo> loadAudio(const QString& path);
    static QList<QImage> loadThumbnails(const QString& path, int height, int step = 1);
    static QString mediaInfoString(const MediaInfo& info);
};

} // namespace tl
