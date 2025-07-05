#include "timelineaxis.h"
#include "timelineutil.h"
#include "timelineview.h"
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>

namespace tl {

struct TimelineAxisPrivate {
    TimelineView* view { nullptr };

    struct Playhead {
        qreal height { 40 };
        qreal x { 0 };
    } playhead;

    struct Ruler {
        QMarginsF margins { 20, 0, 20, 0 };
        qreal frame_width { 0 };
        qreal tick_width { 0 };
        qreal minimum { 0 };
        qreal maximum { 1 };
        int sub_ticks { 5 };
    } ruler;

    bool frame_mode { false };
    double fps { 24.0 };

    bool pressed { false };
};

TimelineAxis::TimelineAxis(TimelineView* view)
    : QWidget(view)
    , d_(new TimelineAxisPrivate)
{
    d_->view = view;
    d_->ruler.tick_width = maxTickLabelWidth();
    d_->ruler.frame_width = innerWidth() / frameCount();
    setMouseTracking(true);
}

TimelineAxis::~TimelineAxis() noexcept
{
    delete d_;
}

bool TimelineAxis::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        if (handleMousePressEvent(static_cast<QMouseEvent*>(event))) {
            event->accept();
            return true;
        }
        break;
    case QEvent::MouseButtonRelease:
        if (handleMouseReleaseEvent(static_cast<QMouseEvent*>(event))) {
            event->accept();
            return true;
        }
        break;
    case QEvent::MouseMove:
        if (handleMouseMoveEvent(static_cast<QMouseEvent*>(event))) {
            event->accept();
            return true;
        }
        break;
    default:
        break;
    }

    return QWidget::event(event);
}

void TimelineAxis::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateTickWidth();
}

void TimelineAxis::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawPlayhead(painter);
    drawRuler(painter);
}

void TimelineAxis::leaveEvent(QEvent* event)
{
    setCursor(Qt::ArrowCursor);
}

void TimelineAxis::drawPlayhead(QPainter& painter)
{
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::gray);
    painter.setOpacity(0.8);

    qreal x = d_->playhead.x + d_->ruler.margins.left();
    qreal w = qMax(frameWidth(), 2.0);

    painter.setPen(Qt::NoPen);
    painter.drawRect(x, 0, w, height());

    painter.setPen(QPen(Qt::red, 2));
    painter.drawLine(x, 2, x + w, 2);

    painter.restore();
    const QString label = valueToText(frame());
    auto label_rect = fontMetrics().boundingRect(label);
    label_rect.setWidth(label_rect.width() + 2);
    label_rect.moveTop(label_rect.height() * 0.2);
    label_rect.moveLeft(d_->playhead.x + d_->ruler.margins.left() + 2);

    if (label_rect.right() > width()) {
        label_rect.moveRight(x - 2);
    }

    painter.drawText(label_rect, label);
}

void TimelineAxis::drawRuler(QPainter& painter)
{
    painter.save();
    painter.setPen(QPen(Qt::black, 1));
    painter.drawLine(0, d_->playhead.height, width(), d_->playhead.height);

    QPen grid_pen = painter.pen();
    grid_pen.setStyle(Qt::DotLine);

    qreal tick_width = tickWidth();
    qint64 tick_count = tickCount() + 2;
    qint64 tick_unit = tickUnit();

    qint64 frame_no = d_->ruler.minimum;
    for (qint64 i = 0; i < tick_count && frame_no <= d_->ruler.maximum; ++i, frame_no += tick_unit) {
        qreal x = d_->ruler.margins.left() + i * tick_width;
        painter.drawLine(x, d_->playhead.height, x, d_->playhead.height * 0.8);

        painter.save();
        painter.setPen(grid_pen);
        painter.setOpacity(0.5);
        painter.drawLine(x, d_->playhead.height, x, height());
        painter.restore();

        QString time_text = valueToText(frame_no);
        painter.drawText(x + 2, d_->playhead.height - 2, time_text);
    }
    painter.restore();
}

bool TimelineAxis::handleMousePressEvent(QMouseEvent* event)
{
    if (QRectF(0, 0, width(), d_->playhead.height).contains(event->position())) {
        d_->pressed = true;
        updatePlayheadX(event->position().x() - d_->ruler.margins.left());
    }

    return false;
}

bool TimelineAxis::handleMouseMoveEvent(QMouseEvent* event)
{
    if (!d_->pressed) {
        return false;
    }

    updatePlayheadX(event->position().x() - d_->ruler.margins.left());
    return true;
}

bool TimelineAxis::handleMouseReleaseEvent(QMouseEvent* event)
{
    if (d_->pressed) {
        d_->pressed = false;
        return true;
    }
    return false;
}

void TimelineAxis::updatePlayheadX(qreal x, bool force)
{
    x = qint64(x / frameWidth()) * frameWidth();
    x = qMin(qMax(0.0, x), (frameCount() - 1) * frameWidth());

    if (qFuzzyCompare(x, d_->playhead.x)) {
        if (force) {
            d_->playhead.x = x;
            update(x, 0, frameWidth(), height());
        }
        return;
    }
    d_->playhead.x = x;
    update();
}

void TimelineAxis::updateRulerArea()
{
    update(QRectF(0, 0, width(), d_->playhead.height).toRect());
}

void TimelineAxis::setPlayheadHeight(qreal height)
{
    d_->playhead.height = height;
    updateRulerArea();
}

qint64 TimelineAxis::minimum() const
{
    return d_->ruler.minimum;
}

qint64 TimelineAxis::maximum() const
{
    return d_->ruler.maximum;
}

void TimelineAxis::setMaximum(qint64 value)
{
    if (d_->ruler.maximum == value) {
        return;
    }

    d_->ruler.maximum = value;
    updateTickWidth();
    update();
}

void TimelineAxis::setMinimum(qint64 value)
{
    if (d_->ruler.minimum == value) {
        return;
    }
    d_->ruler.minimum = value;
    updateTickWidth();
    update();
}

qreal TimelineAxis::innerWidth() const
{
    return width() - d_->ruler.margins.left() - d_->ruler.margins.right();
}

qreal TimelineAxis::maxTickLabelWidth() const
{
    const auto& font_metrics = fontMetrics();
    return qMax(font_metrics.horizontalAdvance(valueToText(d_->ruler.minimum)), font_metrics.horizontalAdvance(valueToText(d_->ruler.maximum))) * 1.6;
}

qreal TimelineAxis::tickCount() const
{
    return innerWidth() / d_->ruler.tick_width;
}

qreal TimelineAxis::tickUnit() const
{
    return 1.0 * frameCount() / tickCount();
}

qreal TimelineAxis::tickWidth() const
{
    return d_->ruler.tick_width;
}

qreal TimelineAxis::frameWidth() const
{
    return d_->ruler.frame_width;
}

qint64 TimelineAxis::frameCount() const
{
    return d_->ruler.maximum - d_->ruler.minimum + 1;
}

QString TimelineAxis::valueToText(qint64 value) const
{
    if (d_->frame_mode) {
        return QString::number(value);
    }
    return TimelineUtil::formatTimeCode(value, d_->fps);
}

void TimelineAxis::setFps(qint64 fps)
{
    d_->fps = fps;
    update();
}

void TimelineAxis::setFrameMode(bool on)
{
    d_->frame_mode = on;
    updateTickWidth();
    update();
}

bool TimelineAxis::isFrameMode() const
{
    return d_->frame_mode;
}

qint64 TimelineAxis::frame() const
{
    return qRound64(d_->playhead.x / tickWidth() * tickUnit() + d_->ruler.minimum);
}

qreal TimelineAxis::mapFrameToAxis(qint64 frame_count) const
{
    return static_cast<qreal>(frame_count) * frameWidth();
}

qreal TimelineAxis::mapFrameToAxisX(qint64 frame_no) const
{
    return mapFrameToAxis(frame_no - d_->ruler.minimum) + d_->ruler.margins.left();
}

void TimelineAxis::updateTickWidth()
{
    d_->ruler.frame_width = innerWidth() / frameCount();
    d_->ruler.tick_width = d_->ruler.frame_width;
    if (d_->ruler.tick_width < maxTickLabelWidth()) {
        d_->ruler.tick_width *= qRound64(maxTickLabelWidth() / d_->ruler.tick_width);
    }
}

void TimelineAxis::moveToFrame(qint64 frame_no)
{
    updatePlayheadX(mapFrameToAxis(frame_no - d_->ruler.minimum), true);
}

} // namespace tl