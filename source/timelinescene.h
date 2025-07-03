#pragma once

#include "timelinedef.h"
#include "timelinelibexport.h"
#include <QGraphicsScene>

namespace tl {
class TimelineItemView;
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
    qreal itemConnViewWidth(const ItemConnID& conn_id) const;

    qreal mapToAxis(qint64 time) const;
    qreal mapToAxisX(qint64 time) const;
    qreal axisTickWidth() const;

    void fitInAxis();

signals:
    void requestSceneContextMenu();
    void requestItemContextMenu(ItemID item_id);

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