#pragma once

#include <QWidget>

namespace tl {

class TimelineView;
struct TimelineAxisPrivate;

class TimelineAxis : public QWidget {
    Q_OBJECT
public:
    explicit TimelineAxis(TimelineView* view);
    ~TimelineAxis() noexcept override;

    void setFps(qint64 fps);
    void setMaximum(qint64 value);
    void setMinimum(qint64 value);

    qint64 minimum() const;
    qint64 maximum() const;

    qint64 frame() const;
    qreal frameWidth() const;

    void setPlayheadHeight(qreal height);

    qreal mapToAxis(qint64 frame_count) const;
    qreal mapToAxisX(qint64 frame_no) const;
    void moveToFrame(qint64 frame_no);

protected:
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    bool handleMousePressEvent(QMouseEvent* event);
    bool handleMouseMoveEvent(QMouseEvent* event);
    bool handleMouseReleaseEvent(QMouseEvent* event);

    void drawPlayhead(QPainter& painter);
    void drawRuler(QPainter& painter);

    void updatePlayheadX(qreal x, bool force = false);
    void updateRulerArea();

    qreal innerWidth() const;
    qreal maxTickLabelWidth() const;
    qreal tickUnit() const;
    qreal tickWidth() const;
    qreal tickCount() const;
    qint64 frameCount() const;

    void updateTickWidth();

    QString valueToText(qint64 value) const;
    static QString formatTimeCode(qint64 value, double fps);

private:
    TimelineAxisPrivate* d_ { nullptr };
};

} // namespace tl
