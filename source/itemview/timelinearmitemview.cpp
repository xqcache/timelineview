#include "timelinearmitemview.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include <QPainter>
#include <QPen>

namespace tl {

void TimelineArmItemView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const auto item_id = itemId();
    auto* item = model()->item(item_id);
    if (!item) [[unlikely]] {
        return;
    }
    drawBase(painter, item);

    int item_row = TimelineModel::itemRow(item_id);
    bool is_head = item_id == model()->headItem(item_row);
    bool is_tail = item_id == model()->tailItem(item_row);

    const auto& bounding_rect = boundingRect();
    const qreal item_height = bounding_rect.height();
    const qreal item_margin = itemMargin();
    const qint64 item_duration = item->duration();
    QRectF boundary(-sceneRef().axisTickWidth() / 2.0, 0, sceneRef().axisTickWidth(), item_height);
    boundary.adjust(item_margin, item_margin, -item_margin, -item_margin);

    const qreal triangle_edge = qMax(bounding_rect.height() / 2.0 * 0.15, 5.0);
    if (is_head) {
        painter->save();
        painter->setPen(item->palette().color(QPalette::Text));
        painter->drawText(boundary.right() + 2, item_height / 2 - 5, tr("Start"));
        painter->restore();
    } else if (is_tail && item_duration == 0) {
        painter->save();
        painter->setPen(item->palette().color(QPalette::Text));
        painter->drawText(boundary.left() - triangle_edge - painter->fontMetrics().boundingRect(tr("End")).width() - 2, item_height / 2 - 5, tr("End"));
        painter->restore();
    }
    if (once_update_param_.isValid()) {
        once_update_param_ = QVariant();
    }
    drawDuration(painter, item);

    if (is_tail && item_duration > 0) {
        const qreal spacing_end = sceneRef().mapToAxis(item->duration()) - sceneRef().axisTickWidth() / 2.0 + item_margin;
        painter->save();
        painter->setPen(item->palette().color(QPalette::ColorRole::Text));
        painter->drawText(spacing_end - triangle_edge - painter->fontMetrics().boundingRect(tr("End")).width() - 2, bounding_rect.height() / 2 - 5, tr("End"));
        painter->restore();
    }
}

bool TimelineArmItemView::onItemOperateFinished(TimelineItem::OperationRole op_role, const QVariant& param)
{
    switch (op_role) {
    case TimelineItem::OperationRole::OpUpdateAsHead:
        once_update_param_ = 1;
        prepareGeometryChange();
        bounding_rect_ = calcBoundingRect();
        update();
        return true;
    case TimelineItem::OperationRole::OpUpdateAsTail:
        once_update_param_ = 2;
        prepareGeometryChange();
        bounding_rect_ = calcBoundingRect();
        update();
        return true;
    default:
        break;
    }
    return false;
}

QRectF TimelineArmItemView::calcBoundingRect() const
{
    const auto item_id = itemId();
    auto rect = TimelineItemView::calcBoundingRect();

    int item_row = TimelineModel::itemRow(item_id);
    bool is_head = (item_id == model()->headItem(item_row)) || (once_update_param_.toInt() == 1);
    bool is_tail = (item_id == model()->tailItem(item_row)) || (once_update_param_.toInt() == 2);

    qreal width = rect.width();
    qreal x = rect.left();

    if (is_head) {
        auto font = sceneRef().font();
        width += QFontMetricsF(font).boundingRect(tr("Start")).width();
    } else if (is_tail) {
        auto font = sceneRef().font();
        qreal label_width = QFontMetricsF(font).boundingRect(tr("End")).width() + 6;
        width += label_width;
        x -= label_width + 3;
    }
    rect.setLeft(x);
    rect.setWidth(width);
    return rect;
}

} // namespace tl