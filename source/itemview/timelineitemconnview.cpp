#include "timelineitemconnview.h"
#include "timelineitemview.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include <QGraphicsDropShadowEffect>
#include <QPainter>

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

    auto* from_item = scene.model()->item(conn_id.from);
    auto* from_item_view = scene.itemView(conn_id.from);
    if (!from_item || !from_item_view) {
        TL_LOG_ERROR("{}:{} Failed to construct TLFrameItemConnPrimitive, from_item or from_graph_item is nullptr!", __func__, __LINE__);
    }

    qreal item_margin = from_item_view->itemMargin();
    setX(scene.mapToAxisX(from_item->destination()) + scene.axisTickWidth() / 2.0 - item_margin);
    setY(from_item_view->y());
}

void TimelineItemConnView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    qreal center_y = boundingRect().height() / 2.0;
    qreal triangle_edge = qMax(center_y * 0.15, 5.0);
    qreal left = 0;
    qreal right = boundingRect().width();

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
    qreal label_max_width = boundingRect().width() - triangle_edge * 2;
    label_rect.setWidth(label_rect.width() + 10);
    {
        painter->setPen(QColor("#006064"));
        // 绘制Label
        if (label_rect.width() < label_max_width) {
            label_rect.moveTop((boundingRect().height() - label_rect.height()) / 2.0);
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
    auto* from_graph_item = scene_.itemView(conn_id_.from);
    if (!from_graph_item) {
        TL_LOG_ERROR("{}:{} Failed to construct TLFrameItemConnPrimitive, from_item or from_graph_item is nullptr!", __func__, __LINE__);
    }
    qreal item_margin = from_graph_item->itemMargin();
    auto result = QRectF(0, 0, scene_.itemConnViewWidth(conn_id_) + 2 * item_margin, scene_.model()->itemHeight());
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
    qreal x = scene_.mapToAxisX(from_item->destination()) + scene_.axisTickWidth() / 2.0 - item_margin;
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

    setY(from_item_view->y());
}

int TimelineItemConnView::type() const
{
    return Type;
}

} // namespace tl