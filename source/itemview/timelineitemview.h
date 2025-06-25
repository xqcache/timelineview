#pragma once

#include "timelinedef.h"
#include <QGraphicsObject>

namespace tl {
class TimelineScene;
class TimelineItemView : public QGraphicsObject {
    Q_OBJECT
public:
    explicit TimelineItemView(ItemID item_id, TimelineScene* scene);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    virtual bool onItemChanged(int role);

    inline ItemID itemId() const;

private:
    ItemID item_id_ { kInvalidItemID };
};

inline ItemID TimelineItemView::itemId() const
{
    return item_id_;
}

} // namespace tl