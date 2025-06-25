#pragma once

#include <QWidget>

namespace tl {

struct TimelineAxisPrivate;

class TimelineAxis : public QWidget {
    Q_OBJECT
public:
    explicit TimelineAxis(QWidget* parent);
    ~TimelineAxis() noexcept override;

    void setPlayheadSnapToTicks(bool snap);
    qint64 playheadTime() const;
    QString playheadTimeString() const;
    void movePlayhead(qint64 ms);

    void setTickUnit(qint64 tick_unit);

signals:
    void timeOffsetChanged(qint64 offset);

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

    bool handleMousePressEvent(QMouseEvent* event);
    bool handleMouseMoveEvent(QMouseEvent* event);
    bool handleMouseReleaseEvent(QMouseEvent* event);

    void drawRuler(QPainter& painter) const;
    void drawPlayhead(QPainter& painter) const;

    void updatePlayhead(qreal new_x);

private:
    TimelineAxisPrivate* d_ { nullptr };
};

} // namespace tl
