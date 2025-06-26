#include "timelinescene.h"
#include "item/timelineitem.h"
#include "itemview/timelineitemview.h"
#include "timelineitemfactory.h"
#include "timelinemodel.h"
#include "timelineview.h"
#include <QGraphicsSceneContextMenuEvent>

namespace tl {

struct TimelineScenePrivate {
    TimelineView* view { nullptr };
    TimelineModel* model { nullptr };
    std::unordered_map<ItemID, std::unique_ptr<TimelineItemView>> item_views;
};

TimelineScene::TimelineScene(TimelineModel* model, QObject* parent)
    : QGraphicsScene(parent)
    , d_(new TimelineScenePrivate)
{
    d_->model = model;
    connect(model, &TimelineModel::itemCreated, this, &TimelineScene::onItemCreated);
    connect(model, &TimelineModel::itemChanged, this, &TimelineScene::onItemChanged);
    connect(model, &TimelineModel::itemRemoved, this, &TimelineScene::onItemRemoved);
    connect(model, &TimelineModel::itemOperateFinished, this, &TimelineScene::onItemOperateFinished);
    connect(model, &TimelineModel::requestUpdateItemY, this, &TimelineScene::onUpdateItemYRequested);

    connect(model, &TimelineModel::itemConnCreated, this, &TimelineScene::onItemConnCreated);
    connect(model, &TimelineModel::itemConnRemoved, this, &TimelineScene::onItemConnRemoved);
}

TimelineScene::~TimelineScene() noexcept
{
    delete d_;
}

void TimelineScene::setView(TimelineView* view)
{
    d_->view = view;
}

TimelineModel* TimelineScene::model() const
{
    return d_->model;
}

TimelineItemView* TimelineScene::itemView(ItemID item_id) const
{
    auto it = d_->item_views.find(item_id);
    if (it != d_->item_views.end()) {
        return it->second.get();
    }
    return nullptr;
}

qreal TimelineScene::mapToAxis(qint64 time) const
{
    if (!d_->view) {
        return 0.0;
    }
    return d_->view->mapToAxis(time);
}

qreal TimelineScene::mapToAxisX(qint64 time) const
{
    if (!d_->view) {
        return 0.0;
    }
    return d_->view->mapToAxisX(time);
}

qreal TimelineScene::axisTickWidth() const
{
    if (!d_->view) {
        return 0.0;
    }
    return d_->view->axisTickWidth();
}

void TimelineScene::fitInAxis()
{
    for (const auto& [_, item] : d_->item_views) {
        item->fitInAxis();
    }
}

void TimelineScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    // 将事件传输给GraphicsItem
    QPointF pos = event->scenePos();
    auto* item = itemAt(pos, QTransform());
    if (!item) {
        emit requestSceneContextMenu();
        return;
    }
    auto* object = item->toGraphicsObject();
    if (!object) {
        emit requestSceneContextMenu();
        return;
    }

    if (object->type() == TimelineItemView::Type) {
        auto* item_view = static_cast<TimelineItemView*>(object);
        clearSelection();
        item_view->setSelected(true);
        emit requestItemContextMenu(item_view->itemId());
        return;
    }
    emit requestSceneContextMenu();
}

void TimelineScene::onItemCreated(ItemID item_id)
{
    d_->item_views[item_id] = model()->itemFactory()->createItemView(item_id, this);
}

void TimelineScene::onItemChanged(ItemID item_id, int role)
{
    auto* item_view = itemView(item_id);
    if (!item_view) {
        return;
    }
    item_view->onItemChanged(role);
}

void TimelineScene::onItemRemoved(ItemID item_id)
{
    auto item_it = d_->item_views.find(item_id);
    if (item_it == d_->item_views.end()) {
        return;
    }
    d_->item_views.erase(item_it);
}

void TimelineScene::onUpdateItemYRequested(ItemID item_id)
{
    auto* item_view = itemView(item_id);
    if (!item_view) {
        return;
    }
    item_view->updateY();
}

void TimelineScene::onItemConnCreated(const ItemConnID& conn)
{
}

void TimelineScene::onItemConnRemoved(const ItemConnID& conn)
{
    
}

void TimelineScene::onItemOperateFinished(ItemID item_id, TimelineItem::OperationRole role, const QVariant& param)
{
    auto* item_view = itemView(item_id);
    if (!item_view) {
        return;
    }
    item_view->onItemOperateFinished(role, param);
}

} // namespace tl
