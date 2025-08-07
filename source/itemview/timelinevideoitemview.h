#pragma once

#include "itemview/timelinearmitemview.h"

namespace tl {

class TimelineVideoItemView : public TimelineItemView {
public:
    using TimelineItemView::TimelineItemView;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    void fitInAxis() override;
    bool onItemChanged(int role) override;

private:
    QRectF calcBoundingRect() const override;

    void updateThumbnail();

private:
    QImage thumbnail_;
};

} // namespace tl