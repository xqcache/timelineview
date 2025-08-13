#include "timelineaudioitem.h"
#include "timelinemediautil.h"
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
    try {
        j.get_to(*this);
        return true;
    } catch (const nlohmann::json::exception& except) {
        TL_LOG_ERROR("Failed to load {} item. Exception: {}", typeName(), except.what());
    }
    return false;
}

nlohmann::json TimelineAudioItem::save() const
{
    nlohmann::json j = TimelineItem::save();
    j["audio_info"] = audio_info_;
    return j;
}

void from_json(const nlohmann::json& j, tl::TimelineAudioItem& item)
{
    j.get_to<TimelineItem>(static_cast<TimelineItem&>(item));
    j["audio_info"].get_to(item.audio_info_);
}

} // namespace tl