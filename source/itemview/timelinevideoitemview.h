#pragma once

#include "itemview/timelinearmitemview.h"

namespace tl {

class TimelineVideoItemView : public TimelineItemView {
public:
    using TimelineItemView::TimelineItemView;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

} // namespace tl