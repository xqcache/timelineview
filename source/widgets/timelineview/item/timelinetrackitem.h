#pragma once

#include "item/timelineitem.h"

namespace tl {
class TimelineTrackItem : public TimelineItem {
public:
    enum PropertyRole {
        PositionRole = userRole(0),
    };

    enum Type {
        Type = UserType + 2
    };

    using TimelineItem::TimelineItem;

    int type() const override;
    const char* typeName() const override;

public:
    bool load(const nlohmann::json& j) override;
    nlohmann::json save() const override;

private:
    friend void from_json(const nlohmann::json& j, TimelineTrackItem& item);

    double position_ { 0 };
};
} // namespace tl