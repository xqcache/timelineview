#include "timelineturntableitem.h"

namespace tl {

TimelineTurntableItem::TimelineTurntableItem(ItemID item_id, TimelineModel* model)
    : TimelineTrackItem(item_id, model)
{
    palette_.setBrush(QPalette::Normal, QPalette::Base, QColor("#9B5E20"));
    palette_.setBrush(QPalette::AlternateBase, QColor("#9B5E20"));
}

int TimelineTurntableItem::type() const
{
    return tl::TimelineTurntableItem::Type;
}

const char* TimelineTurntableItem::typeName() const
{
    return "TimelineTurntableItem";
}

} // namespace tl