#include "timelineinputitem.h"

namespace tl {
TimelineInputItem::TimelineInputItem(ItemID item_id, TimelineModel* model)
    : TimelineIOItem(item_id, model)
{
    palette_.setBrush(QPalette::Normal, QPalette::Base, QColor("#455A64"));
    palette_.setBrush(QPalette::AlternateBase, QColor("#455A64"));
}

int TimelineInputItem::type() const
{
    return Type;
}

const char* TimelineInputItem::typeName() const
{
    return "TimelineInputItem";
}
} // namespace tl