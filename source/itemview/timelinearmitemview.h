#pragma once

#include "timelineitemview.h"

namespace tl {
class TimelineArmItemView : public TimelineItemView {
    Q_OBJECT
public:
    using TimelineItemView::TimelineItemView;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    bool onItemOperateFinished(TimelineItem::OperationRole op_role, const QVariant& param) override;

protected:
    QRectF calcBoundingRect() const override;

private:
    QVariant once_update_param_;
};
} // namespace tl