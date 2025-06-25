#include "timelinescene.h"
#include "item/timelineitem.h"
#include "itemview/timelineitemview.h"
#include "timelineitemfactory.h"
#include "timelinemodel.h"


namespace tl {

struct TimelineScenePrivate {
    TimelineModel* model { nullptr };
    std::unordered_map<ItemID, std::unique_ptr<TimelineItemView>> item_views;
};

TimelineScene::TimelineScene(QObject* parent)
    : QGraphicsScene(parent)
    , d_(new TimelineScenePrivate)
{
}

TimelineScene::~TimelineScene() noexcept
{
    delete d_;
}

void TimelineScene::setModel(TimelineModel* model)
{
    if (d_->model == model) {
        return;
    }

    d_->model = model;
    connect(model, &TimelineModel::itemCreated, this, &TimelineScene::onItemCreated);
}

TimelineModel* TimelineScene::model() const
{
    return d_->model;
}

void TimelineScene::onItemCreated(ItemID item_id)
{
    d_->item_views[item_id] = d_->model->itemFactory()->createItemView(item_id, this);
}

} // namespace tl
