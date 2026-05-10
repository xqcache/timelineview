#include "timelineoutput1item.h"

namespace tl {
TimelineOutput1Item::TimelineOutput1Item(ItemID item_id, TimelineModel* model)
    : TimelineIOItem(item_id, model)
{
    palette_.setBrush(QPalette::Normal, QPalette::Base, QColor("#6D4C41"));
    palette_.setBrush(QPalette::AlternateBase, QColor("#6D4C41"));
}

int TimelineOutput1Item::type() const
{
    return Type;
}

const char* TimelineOutput1Item::typeName() const
{
    return "TimelineOutput1Item";
}
} // namespace tl
