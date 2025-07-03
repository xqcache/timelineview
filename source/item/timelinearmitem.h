#pragma once

#include "timelineitem.h"
#include "timelinelibexport.h"

namespace tl {
class TIMELINE_LIB_EXPORT TimelineArmItem : public TimelineItem {
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
    const char* typeName() const override;

public:
    bool load(const nlohmann::json& j) override;
    nlohmann::json save() const override;

private:
    friend void from_json(const nlohmann::json& j, TimelineArmItem& item);

    std::vector<double> angles_ {};
    bool tracking_ { false };
};
} // namespace tl