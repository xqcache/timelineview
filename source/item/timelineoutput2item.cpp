#include "timelineoutput2item.h"

namespace tl {
TimelineOutput2Item::TimelineOutput2Item(ItemID item_id, TimelineModel* model)
    : TimelineIOItem(item_id, model)
{
    palette_.setBrush(QPalette::Normal, QPalette::Base, QColor("#2E7D32"));
    palette_.setBrush(QPalette::AlternateBase, QColor("#2E7D32"));
}

int TimelineOutput2Item::type() const
{
    return Type;
}

const char* TimelineOutput2Item::typeName() const
{
    return "TimelineOutput2Item";
}
} // namespace tl
