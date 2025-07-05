#pragma once

#include "timelinedef.h"
#include <QFontMetricsF>
#include <QGraphicsObject>

namespace tl {
class TimelineScene;
class TimelineItemConnView : public QGraphicsObject {
    Q_OBJECT
public:
    explicit TimelineItemConnView(const ItemConnID& conn_id, TimelineScene& scene);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    QRectF boundingRect() const override;

    void fitInAxis();
    void updateX();
    void updateY();
    void updateGeometry();

public:
    enum {
        Type = UserType + 100
    };

    int type() const override;

private:
    QRectF calcBoundingRect() const;

private:
    ItemConnID conn_id_;
    TimelineScene& scene_;
    QFontMetricsF font_metrics_;
};
} // namespace tl