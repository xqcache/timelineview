#pragma once

#include "itemview/timelinearmitemview.h"

namespace tl {
class TimelineIOItemView : public TimelineArmItemView {
    Q_OBJECT
public:
    using TimelineArmItemView::TimelineArmItemView;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

protected:
    QRectF calcBoundingRect() const override;
};

} // namespace tl