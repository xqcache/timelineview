#include "timelineitemfactory.h"
#include "item/timelineaimitem.h"
#include "item/timelinearmitem.h"
#include "item/timelineaudioitem.h"
#include "item/timelinefocusitem.h"
#include "item/timelinetrackitem.h"
#include "item/timelinevideoitem.h"
#include "item/timelinezoomitem.h"
#include "itemview/timelinearmitemview.h"
#include "itemview/timelineaudioitemview.h"
#include "itemview/timelinevideoitemview.h"
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
    case TimelineFocusItem::Type:
        return std::make_unique<TimelineFocusItem>(item_id, model);
    case TimelineZoomItem::Type:
        return std::make_unique<TimelineZoomItem>(item_id, model);
    case TimelineVideoItem::Type:
        return std::make_unique<TimelineVideoItem>(item_id, model);
    case TimelineAudioItem::Type:
        return std::make_unique<TimelineAudioItem>(item_id, model);
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
    case TimelineFocusItem::Type:
        return std::make_unique<TimelineArmItemView>(item_id, scene);
    case TimelineZoomItem::Type:
        return std::make_unique<TimelineArmItemView>(item_id, scene);
    case TimelineVideoItem::Type:
        return std::make_unique<TimelineVideoItemView>(item_id, scene);
    case TimelineAudioItem::Type:
        return std::make_unique<TimelineAudioItemView>(item_id, scene);
    default:
        TL_LOG_ERROR("{}:{} Unknown item type {}!", __FILE__, __LINE__, item_type);
        break;
    }
    return nullptr;
}
} // namespace tl