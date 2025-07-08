#include "timelinearmitemview.h"
#include "item/timelineitem.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include "timelineview.h"
#include <QPainter>
#include <QPen>

namespace tl {

void TimelineArmItemView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const auto& bounding_rect = boundingRect();
    if (bounding_rect.isEmpty()) {
        return;
    }

    const auto item_id = itemId();
    auto* item = model()->item(item_id);
    if (!item) [[unlikely]] {
        return;
    }
    drawBase(painter, item);

    int item_row = TimelineModel::itemRow(item_id);
    bool is_head = item_id == model()->headItem(item_row);
    bool is_tail = item_id == model()->tailItem(item_row);

    const qreal item_height = bounding_rect.height();
    const qreal item_margin = itemMargin();
    const qint64 item_duration = item->duration();
    QRectF boundary(-sceneRef().axisTickWidth() / 2.0, 0, sceneRef().axisTickWidth(), item_height);
    boundary.adjust(item_margin, item_margin, -item_margin, -item_margin);

    const qreal triangle_edge = qMax(bounding_rect.height() / 2.0 * 0.15, 5.0);
    if (is_head) {
        painter->save();
        painter->setPen(item->palette().color(QPalette::Base));
        painter->drawText(boundary.right() + 2, item_height / 2 - 5, tr("Start"));
        painter->restore();
    } else if (is_tail && item_duration == 0) {
        painter->save();
        painter->setPen(item->palette().color(QPalette::Base));
        painter->drawText(boundary.left() - triangle_edge - painter->fontMetrics().boundingRect(tr("End")).width() - 2, item_height / 2 - 5, tr("End"));
        painter->restore();
    }
    if (once_update_param_.isValid()) {
        once_update_param_ = QVariant();
    }
    drawDuration(painter, item);

    if (is_tail && item_duration > 0) {
        const qreal spacing_end = sceneRef().mapFrameToAxis(item->duration()) - sceneRef().axisTickWidth() / 2.0 + item_margin;
        painter->save();
        painter->setPen(item->palette().color(QPalette::ColorRole::Text));
        painter->drawText(spacing_end - triangle_edge - painter->fontMetrics().boundingRect(tr("End")).width() - 2, bounding_rect.height() / 2 - 5, tr("End"));
        painter->restore();
    }
}

bool TimelineArmItemView::onItemOperateFinished(int op_role, const QVariant& param)
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

void TimelineArmItemView::drawBase(QPainter* painter, const TimelineItem* item)
{
    painter->save();
    QPen pen = painter->pen();
    pen.setWidth(isSelected() ? 2 : 1);
    pen.setColor(isSelected() ? Qt::yellow : Qt::white);
    painter->setPen(pen);
    painter->setBrush(item->isEnabled() ? item->palette().brush(QPalette::Base) : item->palette().brush(QPalette::Disabled, QPalette::Base));

    QRectF base_rect(-sceneRef().axisTickWidth() / 2.0, 0, sceneRef().axisTickWidth(), bounding_rect_.height());
    qreal item_margin = itemMargin();
    base_rect.adjust(item_margin, item_margin, -item_margin, -item_margin);
    painter->drawRoundedRect(base_rect, 2, 2);
    painter->setPen(item->isEnabled() ? item->palette().color(QPalette::Text) : item->palette().color(QPalette::Disabled, QPalette::Text));
    painter->drawText(base_rect, Qt::AlignCenter, QString::number(item->number()));
    painter->restore();
}

void TimelineArmItemView::drawDuration(QPainter* painter, const TimelineItem* item)
{
    const auto& delay = item->duration();
    if (delay == 0) {
        return;
    }

    painter->save();

    QRectF bounding_rect = boundingRect();
    qreal item_margin = itemMargin();
    qreal tick_pixels = sceneRef().axisTickWidth();
    qreal delay_pixels = sceneRef().mapFrameToAxis(delay);
    qreal spacing_begin = tick_pixels / 2.0 - item_margin;
    qreal spacing_end = delay_pixels - tick_pixels / 2.0 + item_margin;
    qreal center_y = bounding_rect.height() / 2.0;

    QRectF boundary(bounding_rect.left(), 0, tick_pixels, bounding_rect.height());
    boundary.adjust(item_margin, item_margin, -item_margin, -item_margin);
    boundary.moveLeft(delay_pixels - tick_pixels / 2.0 + item_margin);

    {
        QPen pen = painter->pen();
        pen.setWidth(isSelected() ? 2 : 1);
        pen.setColor(isSelected() ? Qt::yellow : Qt::white);
        painter->setPen(pen);
        painter->setBrush(item->isEnabled() ? item->palette().brush(QPalette::ColorRole::Base)
                                            : item->palette().brush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base));
        // 绘制delay frame矩形
        painter->drawRoundedRect(boundary, 2, 2);
        painter->setPen(item->isEnabled() ? item->palette().color(QPalette::Text) : item->palette().color(QPalette::Disabled, QPalette::Text));
        painter->drawText(boundary, Qt::AlignCenter, QString::number(item->number()));
    }

    painter->setPen(item->palette().color(QPalette::ColorRole::Text));
    qreal triangle_edge = qMax(center_y * 0.15, 5.0);
    {
        // 绘制start frame右侧小三角
        painter->save();
        painter->setPen(Qt::white);
        QPainterPath path;
        path.moveTo(spacing_begin + triangle_edge, center_y);
        path.lineTo(spacing_begin, center_y - triangle_edge);
        path.lineTo(spacing_begin, center_y + triangle_edge);
        path.lineTo(spacing_begin + triangle_edge, center_y);
        painter->drawPath(path);
        painter->restore();
    }

    QString label = tr("Delay");
    QRectF label_rect = painter->fontMetrics().boundingRect(label);
    label_rect.setWidth(label_rect.width() + 10);
    qreal label_max_width = delay_pixels - triangle_edge * 2 - tick_pixels;
    {
        // 绘制Label
        if (label_rect.width() < label_max_width) {
            label_rect.moveTop((boundary.height() - label_rect.height()) / 2.0);
            label_rect.moveLeft(spacing_begin + triangle_edge + (label_max_width - label_rect.width()) / 2.0);
            painter->drawText(label_rect, Qt::AlignCenter, label);
        }
    }

    {
        // 绘制start和delay frame中间的虚线
        painter->save();
        QPen pen = painter->pen();
        pen.setStyle(Qt::DashLine);
        pen.setCapStyle(Qt::RoundCap);
        painter->setPen(pen);
        if (label_rect.width() < label_max_width) {
            painter->drawLine(spacing_begin + triangle_edge, center_y, label_rect.left(), center_y);
            painter->drawLine(label_rect.right(), center_y, spacing_end, center_y);
        } else {
            painter->drawLine(spacing_begin + triangle_edge, center_y, spacing_end, center_y);
        }
        painter->restore();
    }

    {
        // 绘制delay frame左侧的小三角
        painter->save();
        painter->setPen(Qt::white);
        QPainterPath path;
        path.moveTo(spacing_end - triangle_edge, center_y);
        path.lineTo(spacing_end, center_y - triangle_edge);
        path.lineTo(spacing_end, center_y + triangle_edge);
        path.lineTo(spacing_end - triangle_edge, center_y);
        painter->drawPath(path);
        painter->restore();
    }

    {
        int item_row = TimelineModel::itemRow(item_id_);
        if (item_id_ == model()->tailItem(item_row)) {
            painter->save();
            painter->setPen(item->palette().color(QPalette::ColorRole::Text));
            painter->drawText(
                spacing_end - triangle_edge - painter->fontMetrics().boundingRect(tr("End")).width() - 2, bounding_rect.height() / 2 - 5, tr("End"));
            painter->restore();
        }
    }
    painter->restore();
}

} // namespace tl