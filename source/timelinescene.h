#pragma once

#include "timelinedef.h"
#include "timelinelibexport.h"
#include <QGraphicsScene>

namespace tl {
class TimelineItemView;
class TimelineItemConnView;
class TimelineView;
class TimelineModel;
struct TimelineScenePrivate;
class TIMELINE_LIB_EXPORT TimelineScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit TimelineScene(TimelineModel* model, QObject* parent = nullptr);
    ~TimelineScene() noexcept override;

    void setView(TimelineView* view);
    TimelineModel* model() const;

    TimelineView* view() const;
    TimelineItemView* itemView(ItemID item_id) const;
    TimelineItemConnView* itemConnView(const ItemConnID& conn_id) const;
    qreal itemConnViewWidth(const ItemConnID& conn_id) const;

    qreal mapFrameToAxis(qint64 time) const;
    qreal mapFrameToAxisX(qint64 time) const;
    qreal axisToSceneX(qreal x) const;
    qreal axisTickWidth() const;
    qreal axisFrameWidth() const;

    QList<ItemID> selectedItems() const;

    void fitInAxis();

signals:
    void requestSceneContextMenu();
    void requestItemContextMenu(ItemID item_id);
    void requestMoveItem(ItemID item_id, qint64 frame_no);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

private:
    void onItemCreated(ItemID item_id);
    void onItemChanged(ItemID item_id, int role);
    void onItemRemoved(ItemID item_id);
    void onItemAboutToBeRemoved(ItemID item_id);
    void onUpdateItemYRequested(ItemID item_id);

    void onItemConnCreated(const ItemConnID& conn_id);
    void onItemConnRemoved(const ItemConnID& conn_id);

    void onItemOperateFinished(ItemID item_id, int role, const QVariant& param);

private:
    TimelineScenePrivate* d_ { nullptr };
};
} // namespace tl