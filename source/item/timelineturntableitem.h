#pragma once

#include "timelinetrackitem.h"

#include "timelinelibexport.h"
namespace tl {
class TIMELINE_LIB_EXPORT TimelineTurntableItem : public TimelineTrackItem {
public:
    enum Type {
        Type = UserType + 8
    };

    TimelineTurntableItem(ItemID item_id, TimelineModel* model);

public:
    int type() const override;
    const char* typeName() const override;
};
} // namespace tl