#include "timelinevideoitem.h"
#include <QCoreApplication>

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

void TimelineVideoItem::setVideoInfo(const VideoInfo& media_info)
{
    video_info_ = media_info;
    setDirty(true);
    notifyPropertyChanged(static_cast<int>(VideoInfoRole) | static_cast<int>(ToolTipRole));
}

void TimelineVideoItem::setPath(const QString& path)
{
    if (video_info_.path == path) {
        return;
    }
    video_info_.path = path;
    setDirty(true);
    notifyPropertyChanged(static_cast<int>(VideoInfoRole) | static_cast<int>(ToolTipRole));
}

bool TimelineVideoItem::setProperty(int role, const QVariant& value)
{
    switch (role) {
    case static_cast<int>(VideoInfoRole):
        setVideoInfo(value.value<VideoInfo>());
        return true;
    default:
        break;
    }
    return TimelineItem::setProperty(role, value);
}

std::optional<QVariant> TimelineVideoItem::property(int role) const
{
    switch (role) {
    case static_cast<int>(VideoInfoRole):
        return QVariant::fromValue(video_info_);
    case static_cast<int>(PathRole):
        return QVariant::fromValue(video_info_.path);
    default:
        break;
    }
    return TimelineItem::property(role);
}

QString TimelineVideoItem::toolTip() const
{
    QString content = TimelineItem::toolTip();
    content += QCoreApplication::translate("TimelineArmItem", "\nFrame Delay: %1\n").arg(duration_);
    content += TimelineMediaUtil::mediaInfoString(video_info_);
    return content;
}

bool TimelineVideoItem::load(const nlohmann::json& j)
{
    try {
        j.get_to(*this);
        return true;
    } catch (const nlohmann::json::exception& except) {
        TL_LOG_ERROR("Failed to load {} item. Exception: {}", typeName(), except.what());
    }
    return false;
}

nlohmann::json TimelineVideoItem::save() const
{
    nlohmann::json j = TimelineItem::save();
    j["video_info"] = video_info_;
    return j;
}

void from_json(const nlohmann::json& j, tl::TimelineVideoItem& item)
{
    j.get_to<TimelineItem>(static_cast<TimelineItem&>(item));
    j["video_info"].get_to(item.video_info_);
}

} // namespace tl