#include "timelineitemfactory.h"
#include "item/timelineaimitem.h"
#include "item/timelinearmitem.h"
#include "item/timelinetrackitem.h"
#include "itemview/timelinearmitemview.h"
#include "timelinemodel.h"

namespace tl {
std::unique_ptr<TimelineItem> TimelineItemFactory::createItem(ItemID item_id, TimelineModel* model)
{
    int item_type = TimelineModel::itemType(item_id);
    switch (item_type) {
    case TimelineArmItem::Type:
        return std::make_unique<TimelineArmItem>(item_id, model);
    case TimelineAimItem::Type:
        return std::make_unique<TimelineAimItem>(item_id, model);
    case TimelineTrackItem::Type:
        return std::make_unique<TimelineTrackItem>(item_id, model);
    default:
        TL_LOG_ERROR("{}:{} Unknown item type {}!", __FILE__, __LINE__, item_type);
        break;
    }
    return nullptr;
}

std::unique_ptr<TimelineItemView> TimelineItemFactory::createItemView(ItemID item_id, TimelineScene* scene)
{
    int item_type = TimelineModel::itemType(item_id);
    switch (item_type) {
    case TimelineArmItem::Type:
        return std::make_unique<TimelineArmItemView>(item_id, scene);
    case TimelineAimItem::Type:
        return std::make_unique<TimelineArmItemView>(item_id, scene);
    case TimelineTrackItem::Type:
        return std::make_unique<TimelineArmItemView>(item_id, scene);
    default:
        TL_LOG_ERROR("{}:{} Unknown item type {}!", __FILE__, __LINE__, item_type);
        break;
    }
    return nullptr;
}
} // namespace tl