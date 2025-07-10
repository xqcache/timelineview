#include "timelinevideoitem.h"

namespace tl {

TimelineVideoItem::TimelineVideoItem(ItemID item_id, TimelineModel* model)
    : TimelineItem(item_id, model)
{
}

int TimelineVideoItem::type() const
{
    return Type;
}

const char* TimelineVideoItem::typeName() const
{
    return "VideoItem";
}

void TimelineVideoItem::setMediaInfo(const MediaInfo& media_info)
{
    media_info_ = media_info;
    setDirty(true);
    notifyPropertyChanged(static_cast<int>(MediaInfoRole) | static_cast<int>(ToolTipRole));
}

void TimelineVideoItem::setPath(const QString& path)
{
    if (media_info_.path == path) {
        return;
    }
    media_info_.path = path;
    setDirty(true);
    notifyPropertyChanged(static_cast<int>(MediaInfoRole) | static_cast<int>(ToolTipRole));
}

bool TimelineVideoItem::setProperty(int role, const QVariant& value)
{
    switch (role) {
    case static_cast<int>(MediaInfoRole):
        setMediaInfo(value.value<MediaInfo>());
        return true;
    case static_cast<int>(PathRole):
        setPath(value.toString());
        return true;
    default:
        break;
    }
    return TimelineItem::setProperty(role, value);
}

std::optional<QVariant> TimelineVideoItem::property(int role) const
{
    switch (role) {
    case static_cast<int>(MediaInfoRole):
        return QVariant::fromValue(media_info_);
    case static_cast<int>(PathRole):
        return QVariant::fromValue(media_info_.path);
    default:
        break;
    }
    return TimelineItem::property(role);
}

QString TimelineVideoItem::toolTip() const
{
    return TimelineMediaUtil::mediaInfoString(media_info_);
}

bool TimelineVideoItem::load(const nlohmann::json& j)
{
    return TimelineItem::load(j);
}

nlohmann::json TimelineVideoItem::save() const
{
    return TimelineItem::save();
}

} // namespace tl