#include "timelineitemfactory.h"
#include "item/timelineaimitem.h"
#include "item/timelinearmitem.h"
#include "item/timelineaudioitem.h"
#include "item/timelinefocusitem.h"
#include "item/timelineinputitem.h"
#include "item/timelineoutput1item.h"
#include "item/timelineoutput2item.h"
#include "item/timelineoutput3item.h"
#include "item/timelineoutput4item.h"
#include "item/timelineoutput5item.h"
#include "item/timelinetrackitem.h"
#include "item/timelineturntableitem.h"
#include "item/timelinevideoitem.h"
#include "item/timelinezoomitem.h"
#include "itemview/timelinearmitemview.h"
#include "itemview/timelineaudioitemview.h"
#include "itemview/timelineioitemview.h"
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
    case TimelineTurntableItem::Type:
        return std::make_unique<TimelineTurntableItem>(item_id, model);
    case TimelineInputItem::Type:
        return std::make_unique<TimelineInputItem>(item_id, model);
    case TimelineOutput1Item::Type:
        return std::make_unique<TimelineOutput1Item>(item_id, model);
    case TimelineOutput2Item::Type:
        return std::make_unique<TimelineOutput2Item>(item_id, model);
    case TimelineOutput3Item::Type:
        return std::make_unique<TimelineOutput3Item>(item_id, model);
    case TimelineOutput4Item::Type:
        return std::make_unique<TimelineOutput4Item>(item_id, model);
    case TimelineOutput5Item::Type:
        return std::make_unique<TimelineOutput5Item>(item_id, model);
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
    case TimelineTurntableItem::Type:
        return std::make_unique<TimelineArmItemView>(item_id, scene);

    // TODO: 暂时使用 TimelineArmItemView 作为输入输出项的视图，后续可以根据需要设计专门的视图类
    case TimelineInputItem::Type:
        return std::make_unique<TimelineIOItemView>(item_id, scene);
    case TimelineOutput1Item::Type:
        return std::make_unique<TimelineIOItemView>(item_id, scene);
    case TimelineOutput2Item::Type:
        return std::make_unique<TimelineIOItemView>(item_id, scene);
    case TimelineOutput3Item::Type:
        return std::make_unique<TimelineIOItemView>(item_id, scene);
    case TimelineOutput4Item::Type:
        return std::make_unique<TimelineIOItemView>(item_id, scene);
    case TimelineOutput5Item::Type:
        return std::make_unique<TimelineIOItemView>(item_id, scene);
    default:
        TL_LOG_ERROR("{}:{} Unknown item type {}!", __FILE__, __LINE__, item_type);
        break;
    }
    return nullptr;
}
} // namespace tl