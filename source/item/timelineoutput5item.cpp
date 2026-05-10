#include "timelineoutput5item.h"

namespace tl {
TimelineOutput5Item::TimelineOutput5Item(ItemID item_id, TimelineModel* model)
    : TimelineIOItem(item_id, model)
{
    palette_.setBrush(QPalette::Normal, QPalette::Base, QColor("#5E35B1"));
    palette_.setBrush(QPalette::AlternateBase, QColor("#5E35B1"));
}

int TimelineOutput5Item::type() const
{
    return Type;
}

const char* TimelineOutput5Item::typeName() const
{
    return "TimelineOutput5Item";
}
} // namespace tl
