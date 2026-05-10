#pragma once

#include "timelineioitem.h"

namespace tl {
class TIMELINE_LIB_EXPORT TimelineOutput5Item : public TimelineIOItem {
public:
    enum Type {
        Type = Begin + 5,
    };

    TimelineOutput5Item(ItemID item_id, TimelineModel* model);

public:
    int type() const override;
    const char* typeName() const override;
};
} // namespace tl
