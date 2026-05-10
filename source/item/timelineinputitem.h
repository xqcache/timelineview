#pragma once

#include "timelineioitem.h"

namespace tl {
class TIMELINE_LIB_EXPORT TimelineInputItem : public TimelineIOItem {
public:
    enum Type {
        Type = Begin,
    };

    TimelineInputItem(ItemID item_id, TimelineModel* model);

public:
    int type() const override;
    const char* typeName() const override;
};
} // namespace tl
