#pragma once

#include "timelineitem.h"
#include "timelinelibexport.h"

namespace tl {
class TIMELINE_LIB_EXPORT TimelineZoomItem : public TimelineItem {
public:
    enum PropertyRole {
        ValueRole = userRole(0),
    };

    enum Type {
        Type = UserType + 5
    };

    TimelineZoomItem(ItemID item_id, TimelineModel* model);

    void setValue(double value);
    double value() const;

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
    friend void from_json(const nlohmann::json& j, TimelineZoomItem& item);

    double value_ { 0 };
};
} // namespace tl