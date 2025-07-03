#pragma once

#include "timelinelibexport.h"
#include <QString>

namespace tl {
class TIMELINE_LIB_EXPORT TimelineUtil {
public:
    static QString formatTimeCode(qint64 value, double fps);
    static qint64 parseTimeCode(const QString& text, double fps);
};
} // namespace tl