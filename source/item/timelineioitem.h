#pragma once

#include "timelineitem.h"
#include "timelinelibexport.h"

namespace tl {
class TIMELINE_LIB_EXPORT TimelineIOItem : public TimelineItem {
public:
    enum PropertyRole : int {
        DataRole = userRole(0),
    };

    enum Type {
        Type = UserType + 9,
        Begin = Type,
        End = Begin + 6
    };

    TimelineIOItem(ItemID item_id, TimelineModel* model);

    void setData(int data);
    int data() const;

public:
    int type() const override;

    bool load(const nlohmann::json& j) override;
    nlohmann::json save() const override;

    bool setProperty(int role, const QVariant& value) override;
    std::optional<QVariant> property(int role) const override;

    QList<TimelineItem::PropertyElement> editableProperties() const override;

protected:
    friend void from_json(const nlohmann::json& j, TimelineIOItem& item);
    int data_ { 0 };
};
} // namespace tl
