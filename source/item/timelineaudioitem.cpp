#include "timelineaudioitem.h"
#include <QCoreApplication>

namespace tl {

TimelineAudioItem::TimelineAudioItem(ItemID item_id, TimelineModel* model)
    : TimelineItem(item_id, model)
{
}

int TimelineAudioItem::type() const
{
    return Type;
}

const char* TimelineAudioItem::typeName() const
{
    return "AudioItem";
}

void TimelineAudioItem::setAudioInfo(const AudioInfo& audio_info)
{
    audio_info_ = audio_info;
    setDirty(true);
    notifyPropertyChanged(static_cast<int>(AudioInfoRole) | static_cast<int>(ToolTipRole));
}

void TimelineAudioItem::setPath(const QString& path)
{
    if (audio_info_.path == path) {
        return;
    }
    audio_info_.path = path;
    setDirty(true);
    notifyPropertyChanged(static_cast<int>(AudioInfoRole) | static_cast<int>(ToolTipRole));
}

bool TimelineAudioItem::setProperty(int role, const QVariant& value)
{
    switch (role) {
    case static_cast<int>(AudioInfoRole):
        setAudioInfo(value.value<AudioInfo>());
        return true;
    case static_cast<int>(PathRole):
        setPath(value.toString());
        return true;
    default:
        break;
    }
    return TimelineItem::setProperty(role, value);
}

std::optional<QVariant> TimelineAudioItem::property(int role) const
{
    switch (role) {
    case static_cast<int>(AudioInfoRole):
        return QVariant::fromValue(audio_info_);
    case static_cast<int>(PathRole):
        return QVariant::fromValue(audio_info_.path);
    default:
        break;
    }
    return TimelineItem::property(role);
}

QString TimelineAudioItem::toolTip() const
{
    QString content = TimelineItem::toolTip() + "\n";
    content += TimelineMediaUtil::audioInfoString(audio_info_);
    return content;
}

bool TimelineAudioItem::load(const nlohmann::json& j)
{
    return TimelineItem::load(j);
}

nlohmann::json TimelineAudioItem::save() const
{
    return TimelineItem::save();
}

} // namespace tl