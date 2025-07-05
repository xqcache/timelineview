#pragma once

#include "timelineitem.h"
#include "timelinelibexport.h"

namespace tl {
class TIMELINE_LIB_EXPORT TimelineTrackItem : public TimelineItem {
public:
    enum PropertyRole {
        PositionRole = userRole(0),
    };

    enum Type {
        Type = UserType + 2
    };

    TimelineTrackItem(ItemID item_id, TimelineModel* model);

    void setPosition(double position);
    double position() const;

public:
    int type() const override;
    const char* typeName() const override;
    QString toolTip() const override;
    bool load(const nlohmann::json& j) override;
    nlohmann::json save() const override;
    QList<TimelineItem::PropertyElement> editableProperties() const override;
    bool setProperty(int role, const QVariant& value) override;
    std::optional<QVariant> property(int role) const override;

private:
    friend void from_json(const nlohmann::json& j, TimelineTrackItem& item);

    double position_ { 0 };
};
} // namespace tl