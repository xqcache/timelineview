#pragma once

#include <QWidget>

namespace tl {

struct TimelineAxisPrivate;

class TimelineAxis : public QWidget {
    Q_OBJECT
public:
    explicit TimelineAxis(QWidget* parent);
    ~TimelineAxis() noexcept override;

    qint64 currentTime() const;
    QString currentTimeString() const;

    void setPlayheadSnapToTicks(bool snap);
    void setPlayheadHeight(qreal height);

    void movePlayhead(qint64 ms);
    void scrollPlayhead(qreal x);

    void setTickUnit(qint64 tick_unit);
    void setTickWidth(qreal pixels);
    qreal tickWidth() const;

    void setRulerRange(qint64 min, qint64 max);
    void setRulerMaximum(qint64 max);
    void setRulerMinimum(qint64 min);
    qint64 rulerRangeInterval() const;
    qreal rulerLength() const;
    qreal rulerMinimumX() const;
    qreal rulerMaximumX() const;

    qreal mapToAxis(qint64 time, const std::optional<qint64>& special_offset = std::nullopt) const;
    qreal mapToAxisX(qint64 time) const;

signals:
    void playheadPressed();
    void playheadReleased();
    void rulerScaled();
    void rulerScrolled(qint64 start_time);
    // 范围发生变化
    void rangeChanged(qint64 min, qint64 max);

protected:
    bool event(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    qreal playheadWidth() const;
    qreal playheadHeight() const;
    qreal playheadPaintWidth() const;
    QPainterPath playheadShape() const;
    QPainterPath playheadTail() const;
    QRectF playheadLabelRect(qreal playhead_x, const QString& text = "") const;

    QString timeString(qint64 ms) const;

    qint64 visualTimeMax() const;
    qint64 visualTimeMin() const;
    qint64 visualTickCount() const;

    qint64 calcTimeByTickUnit(qint64 tick_unit) const;

    bool handleMousePressEvent(QMouseEvent* event);
    bool handleMouseMoveEvent(QMouseEvent* event);
    bool handleMouseReleaseEvent(QMouseEvent* event);

    void drawRuler(QPainter& painter) const;
    void drawPlayhead(QPainter& painter) const;
    void drawRangeIndicator(QPainter& painter) const;

    void updatePlayhead(qreal new_x);
    void updateRulerArea();
    void updateAxis();

private:
    TimelineAxisPrivate* d_ { nullptr };
};

} // namespace tl
