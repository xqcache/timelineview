#pragma once

#include "itemview/timelineitemview.h"

namespace tl {

class TimelineVideoItemView : public TimelineItemView {
public:
    using TimelineItemView::TimelineItemView;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    bool onItemChanged(int role) override;

    void refreshCache() override;


private:
    QRectF calcBoundingRect() const override;

    void updateThumbnails();
    void updatePainterThumbnail();

private:
    QList<QImage> thumbnails_;
    QImage painter_thumbnail_;
};

} // namespace tl