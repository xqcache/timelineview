#pragma once

#include "timelineitemview.h"

namespace tl {
class TimelineArmItemView : public TimelineItemView {
    Q_OBJECT
public:
    using TimelineItemView::TimelineItemView;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    bool onItemOperateFinished(int op_role, const QVariant& param) override;

protected:
    QRectF calcBoundingRect() const override;

    void drawBase(QPainter* painter, const TimelineItem* item);
    void drawDuration(QPainter* painter, const TimelineItem* item);

private:
    QVariant once_update_param_;
};
} // namespace tl