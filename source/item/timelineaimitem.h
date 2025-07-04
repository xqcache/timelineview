#pragma once

#include "timelineitem.h"
#include "timelinelibexport.h"

namespace tl {
class TIMELINE_LIB_EXPORT TimelineAimItem : public TimelineItem {
public:
    enum PropertyRole {
        PositionRole = userRole(0),
    };

    enum Type {
        Type = UserType + 3
    };

    using TimelineItem::TimelineItem;

    int type() const override;
    const char* typeName() const override;

public:
    bool load(const nlohmann::json& j) override;
    nlohmann::json save() const override;

private:
    friend void from_json(const nlohmann::json& j, TimelineAimItem& item);

    std::array<double, 3> position_ { 0, 0, 0 };
};
} // namespace tl