#include "timelineview.h"
#include "timelineaxis.h"
#include <QMouseEvent>

namespace tl {
struct TimelineViewPrivate {
    TimelineAxis* axis { nullptr };
};

TimelineView::TimelineView(QWidget* parent)
    : QGraphicsView(parent)
    , d_(new TimelineViewPrivate)
{
    d_->axis = new TimelineAxis(this);
}

TimelineView::~TimelineView() noexcept
{
    delete d_;
}

bool TimelineView::event(QEvent* event)
{
    // qDebug() << event->type();
    return QGraphicsView::event(event);
}

void TimelineView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);

    auto viewport_geo = viewport()->geometry();
    d_->axis->setGeometry(viewport_geo.left(), 0, viewport_geo.width(), viewport_geo.bottom());
}

} // namespace tl