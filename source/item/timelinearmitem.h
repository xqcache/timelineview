#pragma once

#include "timelineitem.h"
#include "timelinelibexport.h"

namespace tl {
class TIMELINE_LIB_EXPORT TimelineArmItem : public TimelineItem {
public:
    enum PropertyRole {
        JointAnglesRole = userRole(0),
        TrackingAimRole = userRole(1),
    };

    enum Type {
        Type = UserType + 1
    };

    using TimelineItem::TimelineItem;

    void setAngles(const QList<double>& angles);
    void setAngles(const std::vector<double>& angles);
    void setTrackingAim(bool tracking, int flag = -1);

    inline bool isTrackingAim() const;

    inline const std::vector<double>& angles() const;

    int type() const override;
    const char* typeName() const override;

    QList<PropertyElement> editableProperties() const override;
    QString toolTip() const override;
    bool setProperty(int role, const QVariant& data) override;
    std::optional<QVariant> property(int role) const override;

public:
    bool load(const nlohmann::json& j) override;
    nlohmann::json save() const override;

private:
    friend void from_json(const nlohmann::json& j, TimelineArmItem& item);

    std::vector<double> angles_ {};
    bool tracking_ { false };
};

inline const std::vector<double>& TimelineArmItem::angles() const
{
    return angles_;
}

inline bool TimelineArmItem::isTrackingAim() const
{
    return tracking_;
}

} // namespace tl