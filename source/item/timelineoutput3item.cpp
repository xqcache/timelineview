#include "timelineoutput3item.h"

namespace tl {
TimelineOutput3Item::TimelineOutput3Item(ItemID item_id, TimelineModel* model)
    : TimelineIOItem(item_id, model)
{
    palette_.setBrush(QPalette::Normal, QPalette::Base, QColor("#C62828"));
    palette_.setBrush(QPalette::AlternateBase, QColor("#C62828"));
}

int TimelineOutput3Item::type() const
{
    return Type;
}

const char* TimelineOutput3Item::typeName() const
{
    return "TimelineOutput3Item";
}
} // namespace tl
