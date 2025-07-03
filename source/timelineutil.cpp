#include "timelineutil.h"
#include <QStringList>

namespace tl {

QString TimelineUtil::formatTimeCode(qint64 value, double fps)
{
    qint64 sces = value / qRound64(fps);

    qint64 hours = sces / 3600;
    qint64 minutes = (sces % 3600) / 60;
    qint64 seconds = sces % 60;
    qint64 frames = value % qRound64(fps);
    return QString("%1:%2:%3:%4").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')).arg(frames, 2, 10, QChar('0'));
}

qint64 TimelineUtil::parseTimeCode(const QString& text, double fps)
{
    if (text.isEmpty()) {
        return -1;
    }
    QStringList parts = text.split(":");
    if (parts.size() != 4) {
        return -1;
    }
    bool ok = false;
    qint64 secs = 0;
    qint64 unit = 3600;
    for (size_t i = 0; i < parts.size() - 1 && unit > 0; ++i) {
        qint64 value = parts[i].toLongLong(&ok) * unit;
        if (!ok) {
            return -1;
        }
        secs += value;
        unit /= 60;
    }

    qint64 frames = secs * fps + parts[3].toInt(&ok);
    if (!ok) {
        return -1;
    }
    return frames;
}

} // namespace tl