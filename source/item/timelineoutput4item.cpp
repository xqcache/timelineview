#include "timelineoutput4item.h"

namespace tl {
TimelineOutput4Item::TimelineOutput4Item(ItemID item_id, TimelineModel* model)
    : TimelineIOItem(item_id, model)
{
    palette_.setBrush(QPalette::Normal, QPalette::Base, QColor("#00838F"));
    palette_.setBrush(QPalette::AlternateBase, QColor("#00838F"));
}

int TimelineOutput4Item::type() const
{
    return Type;
}

const char* TimelineOutput4Item::typeName() const
{
    return "TimelineOutput4Item";
}
} // namespace tl
