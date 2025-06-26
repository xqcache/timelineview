#pragma once

#include "item/timelineitem.h"
#include "timelinedef.h"
#include <QGraphicsObject>

namespace tl {
class TimelineItem;
class TimelineScene;
class TimelineItemView : public QGraphicsObject {
    Q_OBJECT
public:
    explicit TimelineItemView(ItemID item_id, TimelineScene* scene);
    inline ItemID itemId() const;

    virtual void fitInAxis();

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    qreal itemMargin() const;

    TimelineModel* model() const;
    TimelineScene& sceneRef();
    const TimelineScene& sceneRef() const;

    virtual void updateX();
    virtual void updateY();

    virtual bool onItemChanged(int role);
    virtual bool onItemOperateFinished(TimelineItem::OperationRole op_role, const QVariant& param);

    enum {
        Type = UserType + 1,
    };
    int type() const override;

protected:
    virtual QRectF calcBoundingRect() const;

    void drawBase(QPainter* painter, const TimelineItem* item);
    void drawDuration(QPainter* painter, const TimelineItem* item);

    ItemID item_id_ { kInvalidItemID };
    QRectF bounding_rect_;
};

inline ItemID TimelineItemView::itemId() const
{
    return item_id_;
}

} // namespace tl