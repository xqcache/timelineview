#pragma once

#include "nlohmann/json.hpp"
#include "timelinelibexport.h"
#include <QImage>
#include <QList>

namespace tl {

class TIMELINE_LIB_EXPORT TimelineMediaUtil {
public:
    struct VideoInfo {
        QString path;
        QSize size;
        double fps { 0 };
        double duration { 0 };
        int frame_count { 0 };
    };

    struct AudioInfo {
        QString path;
        double duration { 0 };
        int frame_count { 0 };
    };

    static std::optional<VideoInfo> loadVideo(const QString& path);
    static std::optional<AudioInfo> loadAudio(const QString& path, double fps);
    static QList<QImage> loadVideoThumbnails(const QString& path, int height, int step = 1);
    static QList<int16_t> loadAudioWaveform(const QString& path);
    static QImage drawWaveform(const QList<int16_t>& pcm_data, int left, int right, int width, int height, bool enabled = true);
    static QString mediaInfoString(const VideoInfo& info);
    static QString audioInfoString(const AudioInfo& info);
};

TIMELINE_LIB_EXPORT void from_json(const nlohmann::json& j, TimelineMediaUtil::AudioInfo& audio_info);
TIMELINE_LIB_EXPORT void to_json(nlohmann::json& j, const TimelineMediaUtil::AudioInfo& audio_info);

TIMELINE_LIB_EXPORT void from_json(const nlohmann::json& j, TimelineMediaUtil::VideoInfo& video_info);
TIMELINE_LIB_EXPORT void to_json(nlohmann::json& j, const TimelineMediaUtil::VideoInfo& video_info);

} // namespace tl
