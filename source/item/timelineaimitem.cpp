#include "timelineaimitem.h"

namespace tl {
void from_json(const nlohmann::json& j, tl::TimelineAimItem& item)
{
    j.get_to<TimelineItem>(static_cast<TimelineItem&>(item));
    j["position"].get_to(item.position_);
}
int TimelineAimItem::type() const
{
    return TimelineAimItem::Type;
}

bool TimelineAimItem::load(const nlohmann::json& j)
{
    try {
        j.get_to(*this);
        return true;
    } catch (const nlohmann::json::exception& except) {
        TL_LOG_ERROR("Failed to load {} item. Exception: {}", typeName(), except.what());
    }
    return false;
}

nlohmann::json TimelineAimItem::save() const
{
    nlohmann::json j = TimelineItem::save();
    j["position"] = position_;
    return j;
}

const char* TimelineAimItem::typeName() const
{
    return "Aim";
}

} // namespace tl