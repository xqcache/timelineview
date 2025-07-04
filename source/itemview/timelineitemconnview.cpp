#include "timelineitemconnview.h"
#include "item/timelineitem.h"
#include "timelineitemview.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include "timelineview.h"
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace tl {

TimelineItemConnView::TimelineItemConnView(const ItemConnID& conn_id, TimelineScene& scene)
    : conn_id_(conn_id)
    , scene_(scene)
    , font_metrics_(scene.font())
{
    scene.addItem(this);
    setZValue(1);

    {
        QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);
        effect->setColor(Qt::black);
        effect->setBlurRadius(20);
        effect->setOffset(0);
        setGraphicsEffect(effect);
    }

    updateX();
    updateY();
}

void TimelineItemConnView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const auto& bounding_rect = calcBoundingRect();
    if (bounding_rect.isEmpty()) {
        return;
    }

    qreal center_y = bounding_rect.height() / 2.0;
    qreal triangle_edge = qMax(center_y * 0.15, 5.0);
    qreal left = bounding_rect.left();
    qreal right = bounding_rect.right();

    painter->setBrush(QColor("#006064"));
    {
        // 绘制start frame右侧小三角
        painter->save();
        painter->setPen(Qt::white);
        QPainterPath path;
        path.moveTo(left + triangle_edge, center_y);
        path.lineTo(left, center_y - triangle_edge);
        path.lineTo(left, center_y + triangle_edge);
        path.lineTo(left + triangle_edge, center_y);
        painter->drawPath(path);
        painter->restore();
    }

    {
        // 绘制delay frame左侧的小三角
        painter->save();
        painter->setPen(Qt::white);
        QPainterPath path;
        path.moveTo(right - triangle_edge, center_y);
        path.lineTo(right, center_y - triangle_edge);
        path.lineTo(right, center_y + triangle_edge);
        path.lineTo(right - triangle_edge, center_y);
        painter->drawPath(path);
        painter->restore();
    }

    QString label = tr("Run");
    QRectF label_rect = painter->fontMetrics().boundingRect(label);
    qreal label_max_width = bounding_rect.width() - triangle_edge * 2;
    label_rect.setWidth(label_rect.width() + 10);
    {
        painter->setPen(QColor("#006064"));
        // 绘制Label
        if (label_rect.width() < label_max_width) {
            label_rect.moveTop((bounding_rect.height() - label_rect.height()) / 2.0);
            label_rect.moveLeft(left + triangle_edge + (label_max_width - label_rect.width()) / 2.0);
            painter->drawText(label_rect, Qt::AlignCenter, label);
        }
    }
    {
        // 绘制连线
        if (label_rect.width() < label_max_width) {
            painter->drawLine(left + triangle_edge, center_y, label_rect.left(), center_y);
            painter->drawLine(label_rect.right(), center_y, right - triangle_edge, center_y);
        } else {
            painter->drawLine(left + triangle_edge, center_y, right - triangle_edge, center_y);
        }
    }
}

QRectF TimelineItemConnView::boundingRect() const
{
    return calcBoundingRect();
}

QRectF TimelineItemConnView::calcBoundingRect() const
{
    QRectF result { 0, 0, 0, 0 };

    auto* from_item_view = scene_.itemView(conn_id_.from);
    if (!from_item_view) {
        TL_LOG_ERROR("{}:{} Failed to construct TLFrameItemConnPrimitive, from_item or from_graph_item is nullptr!", __func__, __LINE__);
    }

    auto* to_graph_item = scene_.itemView(conn_id_.to);
    if (!to_graph_item) {
        TL_LOG_ERROR("{}:{} Failed to construct TLFrameItemConnPrimitive, to_item or to_graph_item is nullptr!", __func__, __LINE__);
    }

    qreal item_margin = from_item_view->itemMargin();
    qreal width = scene_.itemConnViewWidth(conn_id_) + 2 * item_margin;
    qreal view_x = scene_.view()->mapFromSceneX(x());

    if (view_x < 0) {
        width += view_x;
        result.moveLeft(qAbs(view_x));
    }

    if (view_x + width > scene_.view()->width()) {
        width = scene_.view()->width() - view_x;
    }

    result.setWidth(width);
    result.setHeight(scene_.model()->itemHeight());
    return result;
}

void TimelineItemConnView::fitInAxis()
{
    if (!scene_.model()) [[unlikely]] {
        return;
    }

    auto* from_item = scene_.model()->item(conn_id_.from);
    auto* from_item_view = scene_.itemView(conn_id_.from);
    if (!from_item || !from_item_view) {
        TL_LOG_ERROR("{}:{} Failed to construct TLFrameItemConnPrimitive, from_item or from_graph_item is nullptr!", __func__, __LINE__);
    }

    qreal item_margin = from_item_view->itemMargin();
    qreal x = scene_.mapFrameToAxisX(from_item->destination()) + scene_.axisTickWidth() / 2.0 - item_margin;
    prepareGeometryChange();
    if (!qFuzzyCompare(x, this->x())) {
        setX(x);
    }
}

void TimelineItemConnView::updateX()
{
    auto* from_item = scene_.model()->item(conn_id_.from);
    auto* from_item_view = scene_.itemView(conn_id_.from);
    if (!from_item || !from_item_view) {
        TL_LOG_ERROR("{}:{} Failed to construct TLFrameItemConnPrimitive, from_item or from_graph_item is nullptr!", __func__, __LINE__);
    }

    qreal item_margin = from_item_view->itemMargin();
    qreal x = scene_.mapFrameToAxisX(from_item->destination()) + scene_.axisTickWidth() / 2.0 - item_margin;
    prepareGeometryChange();
    if (!qFuzzyCompare(x, this->x())) {
        setX(x);
    }
}

void TimelineItemConnView::updateY()
{
    auto* from_item = scene_.model()->item(conn_id_.from);
    auto* from_item_view = scene_.itemView(conn_id_.from);
    if (!from_item || !from_item_view) {
        TL_LOG_ERROR("{}:{} Failed to construct TLFrameItemConnPrimitive, from_item or from_graph_item is nullptr!", __func__, __LINE__);
    }
    qreal y = from_item_view->y();
    prepareGeometryChange();
    if (!qFuzzyCompare(y, this->y())) {
        setY(y);
    }
}

int TimelineItemConnView::type() const
{
    return Type;
}

void TimelineItemConnView::updateGeometry()
{
    prepareGeometryChange();
    update();
}

} // namespace tl