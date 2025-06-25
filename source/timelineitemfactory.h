#pragma once

#include "item/timelineitem.h"
#include "itemview/timelineitemview.h"

namespace tl {
class TimelineScene;

class TimelineItemFactory {
public:
    std::unique_ptr<TimelineItem> createItem(ItemID item_id, TimelineModel* model);
    std::unique_ptr<TimelineItemView> createItemView(ItemID item_id, TimelineScene* scene);
};
} // namespace tl