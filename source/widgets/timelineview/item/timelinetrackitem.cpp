#include "timelinetrackitem.h"

namespace tl {
void from_json(const nlohmann::json& j, tl::TimelineTrackItem& item)
{
    j.get_to<TimelineItem>(static_cast<TimelineItem&>(item));
    j["position"].get_to(item.position_);
}

int TimelineTrackItem::type() const
{
    return TimelineTrackItem::Type;
}

bool TimelineTrackItem::load(const nlohmann::json& j)
{
    try {
        j.get_to(*this);
        return true;
    } catch (const nlohmann::json::exception& except) {
        TL_LOG_ERROR("Failed to load {} item. Exception: {}", typeName(), except.what());
    }
    return false;
}

nlohmann::json TimelineTrackItem::save() const
{
    nlohmann::json j = TimelineItem::save();
    j["position"] = position_;
    return j;
}

const char* TimelineTrackItem::typeName() const
{
    return "Track";
}

} // namespace tl