#pragma once

#include "timelineioitem.h"

namespace tl {
class TIMELINE_LIB_EXPORT TimelineOutput2Item : public TimelineIOItem {
public:
    enum Type {
        Type = Begin + 2,
    };

    TimelineOutput2Item(ItemID item_id, TimelineModel* model);

public:
    int type() const override;
    const char* typeName() const override;
};
} // namespace tl
