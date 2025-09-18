#pragma once

#include "timelinedef.h"
#include "timelinelibexport.h"
#include <QWidget>

namespace Ui {
class TimelineRanger;
}

namespace tl {

class TimelineRangeSlider;

struct TimelineRangerPrivate;
class TIMELINE_LIB_EXPORT TimelineRanger : public QWidget {
    Q_OBJECT

public:
    TimelineRanger(QWidget* parent = nullptr);
    ~TimelineRanger() noexcept override;

    TimelineRangeSlider* slider() const;

    // 以帧模式控制范围
    void setFormat(FrameFormat fmt);
    void setFrameMinimum(qint64 minimum);
    void setFrameMaximum(qint64 maximum);
    void setFps(double fps);
    double fps() const;

signals:
    void fpsChanged(double fps);

private:
    void initUi();
    void setupSignals();

private:
    Ui::TimelineRanger* ui_ { nullptr };
    TimelineRangerPrivate* d_ { nullptr };
};

} // namespace tl