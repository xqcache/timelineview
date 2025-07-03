#pragma once

#include "timelinelibexport.h"
#include <QWidget>

namespace tl {

struct TimelineRangeSliderPrivate;
class TIMELINE_LIB_EXPORT TimelineRangeSlider : public QWidget {
    Q_OBJECT

public:
    TimelineRangeSlider(QWidget* parent = nullptr);
    ~TimelineRangeSlider() noexcept override;

    QSize minimumSizeHint() const override;

    void setFps(double fps);
    double fps() const;

    void setFrameRange(qint64 minimum, qint64 maximum);
    void setFrameMaximum(qint64 maximum);
    void setFrameMinimum(qint64 minimum);
    void setFrameMaximum(const QString& text);
    void setFrameMinimum(const QString& text);
    void setViewFrameMinimum(qint64 minimum);
    void setViewFrameMaximum(qint64 maximum);

    // 设置显示模式。true 为帧模式，false 为时间码模式。
    void setFrameMode(bool on);
    bool isFrameMode() const;
    qint64 frameMaximum() const;
    qint64 frameMinimum() const;
    qint64 viewFrameMinimum() const;
    qint64 viewFrameMaximum() const;

    bool checkFrameMinimumValid(const QString& min_text) const;
    bool checkFrameMaximumValid(const QString& max_text) const;

    QString frameMaximumText() const;
    QString frameMinimumText() const;

    QString valueToText(qint64 value) const;

signals:
    void viewMinimumChanged(qint64 value);
    void viewMaximumChanged(qint64 value);
    void frameRangeChanged(qint64 minimum, qint64 maximum);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QPainterPath minHandleShape() const;
    QPainterPath maxHandleShape() const;
    QPainterPath midHandleShape() const;
    int innerWidth() const;
    int innerHeight() const;
    int sliderWidth() const;
    void setViewMinimum(qint64 value);
    void setViewMaximum(qint64 value);

    int viewRangeTextWidth(const QString& text) const;

    QString viewMinimumText() const;
    QString viewMaximumText() const;

    int viewMinimumX() const;
    int viewMaximumX() const;
    int viewIntervalWidth() const;

    qreal deltaX() const;
    qreal detalV() const;

private:
    void initUi();

private:
    TimelineRangeSliderPrivate* d_ { nullptr };
};

} // namespace tl