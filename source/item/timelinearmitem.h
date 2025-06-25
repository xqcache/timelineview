#pragma once

#include "item/timelineitem.h"

namespace tl {
class TimelineArmItem : public TimelineItem {
public:
    enum PropertyRole {
        JointAnglesRole = userRole(0),
        TrackingAimRole = userRole(0),
    };

    enum Type {
        Type = UserType + 1
    };

    using TimelineItem::TimelineItem;

    int type() const override;

public:
    bool load(const nlohmann::json& j) override;
    nlohmann::json save() override;

private:
    friend void to_json(nlohmann::json& j, const TimelineArmItem& item);
    friend void from_json(const nlohmann::json& j, TimelineArmItem& item);

    std::vector<double> angles_ {};
    bool tracking_ { false };
};
} // namespace tl