#include "timelineitemview.h"
#include "item/timelineitem.h"
#include "timelinescene.h"
#include <QPainter>

namespace tl {
TimelineItemView::TimelineItemView(ItemID item_id, TimelineScene* scene)
    : item_id_(item_id)
{
    scene->addItem(this);
}

QRectF TimelineItemView::boundingRect() const
{
    return { 0, 0, 10, 10 };
}

void TimelineItemView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->fillRect(boundingRect(), Qt::red);
}

bool TimelineItemView::onItemChanged(int role)
{
    return true;
}
} // namespace tl
