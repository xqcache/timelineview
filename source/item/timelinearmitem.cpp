#include "timelinearmitem.h"

namespace tl {

void from_json(const nlohmann::json& j, tl::TimelineArmItem& item)
{
    j.get_to<TimelineItem>(static_cast<TimelineItem&>(item));
    j["joint_angles"].get_to(item.angles_);
    j["tracking"].get_to(item.tracking_);
}

void to_json(nlohmann::json& j, const TimelineArmItem& item)
{
    j = static_cast<const TimelineItem&>(item);
    j["joint_angles"] = item.angles_;
    j["tracking"] = item.tracking_;
}

int TimelineArmItem::type() const
{
    return TimelineArmItem::Type;
}

bool TimelineArmItem::load(const nlohmann::json& j)
{
    try {
        j.get_to(*this);
        return true;
    } catch (const nlohmann::json::exception& except) {
        TL_LOG_ERROR("Failed to load item. Exception: {}", except.what());
    }
    return false;
}

nlohmann::json TimelineArmItem::save()
{
    return *this;
}
} // namespace tl