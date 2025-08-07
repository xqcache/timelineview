#include "timelineitemview.h"
#include "item/timelineitem.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include "timelineview.h"
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
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
    setToolTip(model()->item(item_id)->toolTip());
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
    qreal x = scene->mapFrameToAxisX(item->start());

    if (isInView()) {
        prepareGeometryChange();
    }

    if (!qFuzzyCompare(x, this->x())) {
        setX(x);
    }
}

QRectF TimelineItemView::boundingRect() const
{
    return bounding_rect_;
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
    qreal width = duration > 0 ? sceneRef().mapFrameToAxis(duration) + tick_width : tick_width;
    qreal height = model()->itemHeight();
    result = QRectF(-tick_width / 2.0, 0, width, height);
    return result;
}

void TimelineItemView::updateX()
{
    auto* item = model()->item(item_id_);
    if (!item) {
        return;
    }
    auto new_x = sceneRef().mapFrameToAxisX(item->start());
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
    bool processed = false;
    if (role & TimelineItem::StartRole) {
        updateX();
        processed = true;
    }

    if (role & TimelineItem::DurationRole) {
        bounding_rect_ = calcBoundingRect();
        prepareGeometryChange();
        update();
        processed = true;
    }

    if (role & TimelineItem::NumberRole) {
        update();
        processed = true;
    }

    if (role & TimelineItem::EnabledRole) {
        setEnabled(model()->item(item_id_)->isEnabled());
        processed = true;
    }

    if (role & TimelineItem::ToolTipRole) {
        setToolTip(model()->item(item_id_)->toolTip());
        processed = true;
    }

    return processed;
}

bool TimelineItemView::onItemOperateFinished(int op_role, const QVariant& param)
{
    return false;
}

int TimelineItemView::type() const
{
    return Type;
}

bool TimelineItemView::isInView() const
{
    return model()->isItemInViewRange(item_id_);
}

void TimelineItemView::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    auto* item = model()->item(item_id_);
    if (!item) {
        return;
    }
    qint64 frame_no = qRound64(event->pos().x() / sceneRef().axisFrameWidth() + item->start());
    if (frame_no < model()->viewFrameMinimum()) {
        frame_no = model()->viewFrameMinimum();
    }
    if (frame_no > model()->viewFrameMaximum() - item->duration()) {
        frame_no = model()->viewFrameMaximum() - item->duration();
    }
    if (frame_no == item->start()) {
        return;
    }
    emit requestMoveItem(item_id_, frame_no);
}

} // namespace tl
