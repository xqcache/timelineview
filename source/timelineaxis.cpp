#include "timelineaxis.h"
#include <QDateTime>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QTimeZone>
#include <QWheelEvent>

namespace tl {
struct TimelineAxisPrivate {
    struct Playhead {
        // 光标大小
        qreal width_ratio = 0.8;
        qreal height = 40;
        // 光标位置
        QPointF pos { 0, 0 };
        bool snap_tick { false };
    } playhead;

    struct Ruler {
        QRect rect;
        qreal tick_width { 40 };
        qint64 time_unit { 1000 };
        qint64 time_offset { 0 };
        qint64 minimum = 0;
        qint64 maximum = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::minutes(1)).count();
        QString label_format { "HH:mm:ss.zzz" };
        // 刻度标签间隔（每隔x个刻度显示一个刻度标签）
        int label_interval { 5 };
    } ruler;

    bool mouse_pressed { false };
};

TimelineAxis::TimelineAxis(QWidget* parent)
    : QWidget(parent)
    , d_(new TimelineAxisPrivate)
{
}

TimelineAxis::~TimelineAxis() noexcept
{
    delete d_;
}

void TimelineAxis::setPlayheadSnapToTicks(bool snap)
{
    d_->playhead.snap_tick = snap;
    qreal new_x = d_->playhead.pos.x();
    if (snap) {
        new_x = qRound64(new_x / d_->ruler.tick_width) * d_->ruler.tick_width;
    }
    updatePlayhead(new_x);
}

void TimelineAxis::setPlayheadHeight(qreal height)
{
    if (height < 10 || qFuzzyCompare(d_->playhead.height, height)) {
        return;
    }
    d_->playhead.height = height;
    updateAxis();
}

void TimelineAxis::movePlayhead(qint64 ms)
{
    assert(0 && "暂未实现");
}

qint64 TimelineAxis::currentTime() const
{
    return calcTimeByTickUnit(d_->ruler.time_unit);
}

QString TimelineAxis::currentTimeString() const
{
    return timeString(currentTime());
}

void TimelineAxis::scrollPlayhead(qreal x)
{
    qint64 v = qRound64(x / d_->ruler.tick_width) * d_->ruler.time_unit;
    d_->ruler.time_offset = v;
    updateRulerArea();
    emit rulerScrolled(d_->ruler.time_offset);
}

void TimelineAxis::setTickUnit(qint64 tick_unit)
{
    if (d_->ruler.time_unit == tick_unit) {
        return; // 没有变化
    }
    auto new_time = calcTimeByTickUnit(tick_unit);
    if (tick_unit < d_->ruler.tick_width) {
        return;
    }
    auto old_time = calcTimeByTickUnit(d_->ruler.time_unit);
    d_->ruler.time_unit = tick_unit;
    d_->ruler.time_offset += old_time - new_time;
    updateRulerArea();
    emit rulerScaled();
    emit rulerScrolled(d_->ruler.time_offset);
}

void TimelineAxis::setTickWidth(qreal pixels)
{
    if (pixels < d_->ruler.time_unit) {
        return;
    }
    if (qFuzzyCompare(d_->ruler.tick_width, pixels)) {
        return;
    }
    d_->ruler.tick_width = pixels;
    updateRulerArea();
}

qreal TimelineAxis::tickWidth() const
{
    return d_->ruler.tick_width;
}

void TimelineAxis::setRulerRange(qint64 min, qint64 max)
{
    if (min >= max) {
        return;
    }
    bool range_changed = false;

    if (min != d_->ruler.minimum) {
        d_->ruler.minimum = min;
        range_changed = true;
    }

    if (max != d_->ruler.maximum) {
        d_->ruler.maximum = max;
        range_changed = true;
    }

    if (range_changed) {
        emit rangeChanged(min, max);
    }
}

void TimelineAxis::setRulerMaximum(qint64 max)
{
    setRulerRange(d_->ruler.minimum, max);
}

void TimelineAxis::setRulerMinimum(qint64 min)
{
    setRulerRange(min, d_->ruler.maximum);
}

qint64 TimelineAxis::rulerRangeInterval() const
{
    return d_->ruler.maximum - d_->ruler.minimum;
}

qreal TimelineAxis::rulerLength() const
{
    return static_cast<qreal>(rulerRangeInterval()) * d_->ruler.tick_width / d_->ruler.time_unit + d_->ruler.tick_width;
}

qreal TimelineAxis::mapToAxis(qint64 time, const std::optional<qint64>& special_offset) const
{
    return static_cast<qreal>(time - special_offset.value_or(d_->ruler.time_offset)) / d_->ruler.time_unit * d_->ruler.tick_width;
}

qreal TimelineAxis::mapToAxisX(qint64 time) const
{
    return mapToAxis(time) + d_->ruler.tick_width / 2.0;
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

void TimelineAxis::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    if (event->region().intersects(d_->ruler.rect)) {
        drawRuler(painter);
    }

    drawPlayhead(painter);
}

void TimelineAxis::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    d_->ruler.rect = QRect(0, 0, width(), playheadHeight());
}

void TimelineAxis::wheelEvent(QWheelEvent* event)
{
    QWidget::wheelEvent(event);
    if (event->angleDelta().y() > 0) {
        setTickUnit(d_->ruler.time_unit + 100);
    } else {
        setTickUnit(d_->ruler.time_unit - 100);
    }
}

qreal TimelineAxis::playheadWidth() const
{
    return d_->ruler.tick_width * d_->playhead.width_ratio;
}

qreal TimelineAxis::playheadHeight() const
{
    return d_->playhead.height;
}

qreal TimelineAxis::playheadPaintWidth() const
{
    return playheadWidth() * 0.6;
}

QPainterPath TimelineAxis::playheadShape() const
{
    QPainterPath shape;
    auto playhead_paint_width = playheadPaintWidth();
    auto playhead_height = playheadHeight();
    qreal top_margin = playhead_height * 0.2;
    qreal left_margin = (d_->ruler.tick_width - playhead_paint_width) / 2.0;
    shape.moveTo(left_margin, top_margin);
    shape.lineTo(left_margin, playhead_height * 0.8);
    shape.lineTo(left_margin + playhead_paint_width / 2, playhead_height);
    shape.lineTo(left_margin + playhead_paint_width, playhead_height * 0.8);
    shape.lineTo(left_margin + playhead_paint_width, top_margin);
    shape.lineTo(left_margin, top_margin);
    shape.translate(d_->playhead.pos);
    return shape;
}

QPainterPath TimelineAxis::playheadTail() const
{
    QPainterPath shape;
    qreal left_margin = d_->ruler.tick_width / 2;
    shape.moveTo(left_margin, playheadHeight());
    shape.lineTo(left_margin, height());
    shape.translate(d_->playhead.pos);
    return shape;
}

QRectF TimelineAxis::playheadLabelRect(qreal playhead_x, const QString& text) const
{
    qreal playhead_margin = (d_->ruler.tick_width - playheadPaintWidth()) / 2.0;
    qreal playhead_right = playhead_x + playhead_margin + playheadPaintWidth();
    const auto font_metrics = fontMetrics();

    QString label = text;
    if (label.isEmpty()) {
        QDateTime::fromMSecsSinceEpoch(currentTime(), QTimeZone::utc()).toString(d_->ruler.label_format);
    }
    QRectF value_rect = QRectF(QPointF(0, font_metrics.height() / 2.0), font_metrics.boundingRect(label).size());
    value_rect.setWidth(value_rect.width() + 4);
    if (playhead_right + value_rect.width() + 2 > width()) {
        value_rect.moveLeft(playhead_x + playhead_margin - value_rect.width() - 4);
    } else {
        value_rect.moveLeft(playhead_right + 2);
    }
    return value_rect;
}

QString TimelineAxis::timeString(qint64 ms) const
{
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(std::abs(ms), QTimeZone::utc());
    if (ms < 0) {
        return "-" + dt.toString(d_->ruler.label_format);
    }
    return dt.toString(d_->ruler.label_format);
}

qint64 TimelineAxis::visualTimeMax() const
{
    return visualTimeMin() + visualTickCount() * d_->ruler.time_unit;
}

qint64 TimelineAxis::visualTimeMin() const
{
    return d_->ruler.time_offset;
}

qint64 TimelineAxis::visualTickCount() const
{
    return static_cast<qint64>(width() - playheadWidth()) / static_cast<qint64>(d_->ruler.tick_width);
}

qint64 TimelineAxis::calcTimeByTickUnit(qint64 tick_unit) const
{
    return qRound64(d_->playhead.pos.x() * tick_unit / d_->ruler.tick_width) + d_->ruler.time_offset;
}

qreal TimelineAxis::rulerMinimumX() const
{
    return (d_->ruler.minimum - d_->ruler.time_offset) * d_->ruler.tick_width / d_->ruler.time_unit;
}

qreal TimelineAxis::rulerMaximumX() const
{
    return (d_->ruler.maximum - d_->ruler.time_offset) * d_->ruler.tick_width / d_->ruler.time_unit;
}

bool TimelineAxis::handleMousePressEvent(QMouseEvent* event)
{
    if (!playheadShape().contains(event->position())) {
        return false;
    }
    d_->mouse_pressed = true;
    update(0, playheadHeight(), width(), height() - playheadHeight());
    emit playheadPressed();
    return true;
}

bool TimelineAxis::handleMouseMoveEvent(QMouseEvent* event)
{
    if (!d_->mouse_pressed) {
        return false;
    }
    qreal new_x = event->position().x() - d_->ruler.tick_width / 2;
    qreal old_x = d_->playhead.pos.x();
    qreal x_offset = event->position().x() - old_x;
    qint64 v_offset = qRound64(x_offset * d_->ruler.time_unit / d_->ruler.tick_width);
    qint64 new_tick_offset = d_->ruler.time_offset;

    if (d_->playhead.snap_tick) {
        new_x = qRound64(new_x * d_->ruler.tick_width / d_->ruler.tick_width);
    }

    if (new_x > (width() - d_->ruler.tick_width / 2.0)) {
        new_tick_offset += v_offset;
        new_tick_offset = qMin(new_tick_offset, d_->ruler.maximum - visualTickCount() * d_->ruler.time_unit);
    } else if (new_x < -d_->ruler.tick_width / 2) {
        new_tick_offset += v_offset;
        new_tick_offset = qMax(new_tick_offset, d_->ruler.minimum);
    }

    if (new_tick_offset != d_->ruler.time_offset) {
        d_->ruler.time_offset = new_tick_offset;
        emit rulerScrolled(new_tick_offset);
        updateRulerArea();
    }

    updatePlayhead(new_x);

    return true;
}

bool TimelineAxis::handleMouseReleaseEvent(QMouseEvent* event)
{
    if (d_->mouse_pressed) {
        d_->mouse_pressed = false;
        emit playheadReleased();
        return true;
    }
    return false;
}

void TimelineAxis::drawRuler(QPainter& painter) const
{
    painter.save();
    // 1. 绘制刻度区域背景色
    painter.setPen(Qt::black);
    painter.fillRect(d_->ruler.rect, QColor("#bdbdbd"));
    painter.drawLine(d_->ruler.rect.bottomLeft(), d_->ruler.rect.bottomRight());

    // 2. 绘制刻度线和标签
    QPen pen = painter.pen();
    pen.setWidth(2);
    pen.setColor(Qt::black);
    painter.setPen(pen);

    qint64 msces_per_label = d_->ruler.label_interval * d_->ruler.time_unit;
    qreal label_per_pixles = d_->ruler.tick_width * d_->ruler.label_interval;
    qreal label_left_spacing = d_->ruler.tick_width / 2.0;

    qint64 time = qMax(d_->ruler.minimum, d_->ruler.time_offset);
    for (qreal x = qMax(rulerMinimumX(), 0.0) + label_left_spacing; x < width(); x += label_per_pixles, time += msces_per_label) {
        if (time < d_->ruler.minimum) {
            continue;
        }
        if (time > d_->ruler.maximum) {
            break;
        }

        // 绘制刻度线
        painter.drawLine(x, playheadHeight(), x, playheadHeight() * 0.9);
        // 绘制刻度标签
        QString label = timeString(time);
        qreal label_width = painter.fontMetrics().horizontalAdvance(label);
        qreal label_x = x - label_width / 2;
        label_x = qMax(label_x, 0.0);
        label_x = qMin(label_x, width() - label_width);
        painter.drawText(label_x, playheadHeight() * 0.8, label);
    }
    painter.restore();
    drawRangeIndicator(painter);
}

void TimelineAxis::drawPlayhead(QPainter& painter) const
{
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    QColor fill_color = Qt::gray;
    fill_color.setAlpha(200);
    painter.setBrush(fill_color);
    painter.drawPath(playheadShape());
    painter.drawPath(playheadTail());
    painter.restore();

    // 绘制当前光标值
    QString value_text = timeString(currentTime());
    painter.drawText(playheadLabelRect(d_->playhead.pos.x(), value_text), Qt::AlignCenter | Qt::AlignTop, value_text);
}

void TimelineAxis::drawRangeIndicator(QPainter& painter) const
{
    painter.save();
    QPen pen = painter.pen();
    pen.setWidth(4);
    pen.setColor(Qt::red);
    painter.setPen(pen);
    if (d_->ruler.minimum >= visualTimeMin() && d_->ruler.minimum <= visualTimeMax()) {
        qreal min_line_x = rulerMinimumX() + d_->ruler.tick_width / 2.0;
        painter.drawLine(min_line_x, 0, min_line_x, playheadHeight() * 0.8);
    }

    if (d_->ruler.maximum >= visualTimeMin() && d_->ruler.maximum <= visualTimeMax()) {
        qreal max_line_x = rulerMaximumX() + d_->ruler.tick_width / 2.0;
        painter.drawLine(max_line_x, 0, max_line_x, playheadHeight() * 0.8);
    }
    painter.restore();
}

void TimelineAxis::updatePlayhead(qreal new_x)
{
    new_x = qMax(new_x, qMax(rulerMinimumX(), 0.0));
    new_x = qMin(new_x, qMin(rulerMaximumX(), visualTickCount() * d_->ruler.tick_width));
    qreal old_x = d_->playhead.pos.x();
    if (qFuzzyCompare(new_x, old_x)) {
        return; // 没有移动
    }
    d_->playhead.pos.setX(new_x);

    QString value_text = timeString(currentTime());
    QRegion dirty_regoin;
    dirty_regoin += QRectF(old_x, 0, playheadWidth(), height()).toRect();
    dirty_regoin += QRectF(new_x, 0, playheadWidth(), height()).toRect();
    dirty_regoin += playheadLabelRect(old_x, value_text).toRect();
    dirty_regoin += playheadLabelRect(new_x, value_text).toRect();
    update(dirty_regoin);
}

void TimelineAxis::updateRulerArea()
{
    update(d_->ruler.rect);
}

void TimelineAxis::updateAxis()
{
    QRegion dirty_region;
    dirty_region += d_->ruler.rect;
    dirty_region += QRectF(d_->playhead.pos.x(), playheadHeight(), playheadWidth(), height() - playheadHeight()).toRect();
    update(dirty_region);
}

} // namespace tl