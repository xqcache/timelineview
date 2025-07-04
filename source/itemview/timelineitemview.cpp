#include "timelineitemview.h"
#include "item/timelineitem.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include "timelineview.h"
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
    if (role & TimelineItem::StartRole) {
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

    if (role & TimelineItem::ToolTipRole) {
        setToolTip(model()->item(item_id_)->toolTip());
        return true;
    }

    return false;
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
    return model()->isItemValid(item_id_);
}

} // namespace tl
