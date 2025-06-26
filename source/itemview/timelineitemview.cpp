#include "timelineitemview.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include <QGraphicsDropShadowEffect>
#include <QPainter>

namespace tl {
TimelineItemView::TimelineItemView(ItemID item_id, TimelineScene* scene)
    : item_id_(item_id)
{
    scene->addItem(this);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setZValue(0);
    bounding_rect_ = calcBoundingRect();
    updateX();
    updateY();

    {
        QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);
        effect->setColor(Qt::black);
        effect->setBlurRadius(20);
        effect->setOffset(0);
        setGraphicsEffect(effect);
    }
}

void TimelineItemView::fitInAxis()
{
    auto* scene = qobject_cast<TimelineScene*>(this->scene());
    if (!scene->model()) [[unlikely]] {
        return;
    }
    auto* item = static_cast<TimelineItem*>(scene->model()->item(item_id_));
    if (!item) [[unlikely]] {
        return;
    }
    bounding_rect_ = calcBoundingRect();

    qreal x = scene->mapToAxisX(item->startTime());
    if (!qFuzzyCompare(x, this->x())) {
        prepareGeometryChange();
        setX(x);
    } else if (item->duration() > 0) {
        prepareGeometryChange();
    }
}

QRectF TimelineItemView::boundingRect() const
{
    return bounding_rect_;
}

void TimelineItemView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    auto* item = model()->item(item_id_);
    if (!item) [[unlikely]] {
        return;
    }

    drawBase(painter, item);
    drawDuration(painter, item);

    // painter->drawText(boundary, Qt::AlignCenter, QString::number(item_model->data<TLFrameItemData>().number()));
}

qreal TimelineItemView::itemMargin() const
{
    return qMin(bounding_rect_.width(), bounding_rect_.height()) * 0.1;
}

TimelineModel* TimelineItemView::model() const
{
    return qobject_cast<TimelineScene*>(scene())->model();
}

TimelineScene& TimelineItemView::sceneRef()
{
    return *qobject_cast<TimelineScene*>(scene());
}

const TimelineScene& TimelineItemView::sceneRef() const
{
    return *qobject_cast<const TimelineScene*>(scene());
}

QRectF TimelineItemView::calcBoundingRect() const
{
    if (!model()) [[unlikely]] {
        return {};
    }
    auto* item = model()->item(item_id_);
    if (!item) [[unlikely]] {
        return {};
    }
    auto duration = item->duration();
    qreal tick_width = sceneRef().axisTickWidth();
    qreal width = duration > 0 ? sceneRef().mapToAxis(duration) + tick_width : tick_width;
    qreal height = model()->itemHeight();
    qreal x = -tick_width / 2.0;
    return QRectF(x, 0, width, height);
}

void TimelineItemView::updateX()
{
    auto* item = model()->item(item_id_);
    if (!item) {
        return;
    }
    auto new_x = sceneRef().mapToAxisX(item->startTime());
    if (!qFuzzyCompare(new_x, x())) {
        setX(new_x);
    }
}

void TimelineItemView::updateY()
{
    qreal new_y = model()->itemY(item_id_);
    if (qFuzzyCompare(new_y, y())) {
        return;
    }
    prepareGeometryChange();
    setY(new_y);
}

bool TimelineItemView::onItemChanged(int role)
{
    if (role & TimelineItem::StartTimeRole) {
        updateX();
        return true;
    }

    if (role & TimelineItem::DurationRole) {
        prepareGeometryChange();
        update();
        return true;
    }

    if (role & TimelineItem::NumberRole) {
        update();
        return true;
    }
    return false;
}

bool TimelineItemView::onItemOperateFinished(TimelineItem::OperationRole op_role, const QVariant& param)
{
    return false;
}

int TimelineItemView::type() const
{
    return Type;
}

void TimelineItemView::drawBase(QPainter* painter, const TimelineItem* item)
{
    painter->save();
    QPen pen = painter->pen();
    pen.setWidth(isSelected() ? 2 : 1);
    pen.setColor(isSelected() ? Qt::yellow : Qt::white);
    painter->setPen(pen);
    painter->setBrush(item->palette().brush(QPalette::ColorRole::Base));

    QRectF base_rect(-sceneRef().axisTickWidth() / 2.0, 0, sceneRef().axisTickWidth(), bounding_rect_.height());
    qreal item_margin = itemMargin();
    base_rect.adjust(item_margin, item_margin, -item_margin, -item_margin);
    painter->drawRoundedRect(base_rect, 2, 2);
    painter->drawText(base_rect, Qt::AlignCenter, QString::number(item->number()));
    painter->restore();
}

void TimelineItemView::drawDuration(QPainter* painter, const TimelineItem* item)
{
    const auto& delay = item->duration();
    if (delay == 0) {
        return;
    }

    painter->save();

    QRectF bounding_rect = boundingRect();
    qreal item_margin = itemMargin();
    qreal tick_pixels = sceneRef().axisTickWidth();
    qreal delay_pixels = sceneRef().mapToAxis(delay);
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
        painter->setBrush(item->palette().brush(QPalette::ColorRole::Base));
        // 绘制delay frame矩形
        painter->drawRoundedRect(boundary, 2, 2);
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
