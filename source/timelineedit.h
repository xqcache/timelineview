#pragma once

#include "timelineeditlibexport.h"

#include <QWidget>

namespace Ui {
class TimelineEdit;
}

namespace tl {

struct TimelineEditPrivate;
class TIMELINEEDIT_LIB_EXPORT TimelineEdit : public QWidget {
    Q_OBJECT

public:
    TimelineEdit(QWidget* parent = nullptr);
    ~TimelineEdit() noexcept override;

private:
    void initUi();
    void initData();
    void setupSignals();

private:
    Ui::TimelineEdit* ui_ { nullptr };
    TimelineEditPrivate* d_ { nullptr };
};

} // namespace tl