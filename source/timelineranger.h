#pragma once

#include "timelineeditlibexport.h"
#include <QWidget>

namespace Ui {
class TimelineRanger;
}

namespace tl {

class TimelineRangeSlider;

struct TimelineRangerPrivate;
class TIMELINEEDIT_LIB_EXPORT TimelineRanger : public QWidget {
    Q_OBJECT

public:
    TimelineRanger(QWidget* parent = nullptr);
    ~TimelineRanger() noexcept override;

    TimelineRangeSlider* slider() const;

    // 以帧模式控制范围
    void setFrameMode(bool on);
    void setFrameRange(qint64 minimum, qint64 maximum);
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