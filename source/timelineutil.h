#pragma once

#include "timelinelibexport.h"
#include <QString>

namespace tl {
class TIMELINE_LIB_EXPORT TimelineUtil {
public:
    static QString formatTimeCode(qint64 value, double fps);
    static QString formatTimeString(qint64 frame_no, double fps, bool keep_msecs = true);
    static qint64 parseTimeCode(const QString& text, double fps);
    static qint64 parseTimeString(const QString& text, double fps, bool keep_msecs = true);
    // 帧数转换为ms
    static qint64 frameToTime(qint64 frame_no, double fps);
    // 帧数转换为s
    static qreal frameToSecs(qint64 frame_no, double fps);
    // ms转换为帧数
    static qint64 msecsToFrame(qint64 ms, double fps);
    static qint64 secsToFrame(qreal secs, double fps);

    static int getMaxScreenWidth();
};
} // namespace tl