#include "timelineioitemview.h"
#include "item/timelineitem.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include <QPainter>

namespace tl {

void TimelineIOItemView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    auto bounding_rect = boundingRect();
    if (bounding_rect.isEmpty()) {
        return;
    }
    const auto item_id = itemId();
    auto* item = model()->item(item_id);
    if (!item) [[unlikely]] {
        return;
    }

    bounding_rect.adjust(0, 1, 0, -1);
    QPen pen = painter->pen();
    pen.setWidth(isSelected() ? 2 : 1);
    pen.setColor(isSelected() ? Qt::yellow : Qt::white);
    painter->setPen(pen);
    painter->setBrush(item->isEnabled() ? item->palette().brush(QPalette::Base) : item->palette().brush(QPalette::Disabled, QPalette::Base));

    if (bounding_rect.width() > 4) {
        painter->drawRoundedRect(bounding_rect, 2, 2);
    } else {
        painter->drawRect(bounding_rect);
    }
}

QRectF TimelineIOItemView::calcBoundingRect() const
{
    QRectF result;
    if (!model()) [[unlikely]] {
        return result;
    }
    auto* item = model()->item(item_id_);
    if (!item) [[unlikely]] {
        return result;
    }

    if (!isInView()) {
        return result;
    }
    auto duration = item->duration();
    qreal tick_width = sceneRef().axisTickWidth();
    qreal width = duration > 0 ? sceneRef().mapFrameToAxis(duration) : tick_width / 3.0;
    qreal height = model()->itemHeight();
    result = QRectF(0, 0, width, height);
    return result;
}

} // namespace tl