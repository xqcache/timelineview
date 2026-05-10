#pragma once

#include "timelineioitem.h"

namespace tl {
class TIMELINE_LIB_EXPORT TimelineOutput4Item : public TimelineIOItem {
public:
    enum Type {
        Type = Begin + 4,
    };

    TimelineOutput4Item(ItemID item_id, TimelineModel* model);

public:
    int type() const override;
    const char* typeName() const override;
};
} // namespace tl
