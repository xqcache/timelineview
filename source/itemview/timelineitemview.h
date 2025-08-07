#pragma once

#include "timelinedef.h"
#include <QGraphicsObject>

namespace tl {
class TimelineModel;
class TimelineItem;
class TimelineScene;
class TimelineItemView : public QGraphicsObject {
    Q_OBJECT
public:
    explicit TimelineItemView(ItemID item_id, TimelineScene* scene);
    inline ItemID itemId() const;

    virtual void fitInAxis();

    QRectF boundingRect() const override;

    qreal itemMargin() const;

    TimelineModel* model() const;
    TimelineScene& sceneRef();
    const TimelineScene& sceneRef() const;

    virtual void updateX();
    virtual void updateY();

    virtual bool onItemChanged(int role);
    virtual bool onItemOperateFinished(int op_role, const QVariant& param);

    enum {
        Type = UserType + 1,
    };
    int type() const override;

    virtual bool isInView() const;

signals:
    void requestMoveItem(ItemID item_id, qint64 frame_no);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

protected:
    virtual QRectF calcBoundingRect() const;

    ItemID item_id_ { kInvalidItemID };
    mutable QRectF bounding_rect_;
};

inline ItemID TimelineItemView::itemId() const
{
    return item_id_;
}

} // namespace tl