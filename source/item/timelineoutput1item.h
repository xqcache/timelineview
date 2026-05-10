#pragma once

#include "timelineioitem.h"

namespace tl {
class TIMELINE_LIB_EXPORT TimelineOutput1Item : public TimelineIOItem {
public:
    enum Type {
        Type = Begin + 1,
    };

    TimelineOutput1Item(ItemID item_id, TimelineModel* model);

public:
    int type() const override;
    const char* typeName() const override;
};
} // namespace tl
