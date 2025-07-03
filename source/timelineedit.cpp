#include "timelineedit.h"
#include "item/timelinearmitem.h"
#include "timelineaxis.h"
#include "timelinemodel.h"
#include "timelinerangeslider.h"
#include "timelinescene.h"
#include "ui_timelineedit.h"
#include <QAction>

namespace tl {

struct TimelineEditPrivate { };

TimelineEdit::TimelineEdit(QWidget* parent)
    : QWidget(parent)
    , ui_(new Ui::TimelineEdit)
    , d_(new TimelineEditPrivate)
{
    initUi();
    setupSignals();
    initData();
}

TimelineEdit::~TimelineEdit() noexcept
{
    delete ui_;
    delete d_;
}

void TimelineEdit::initUi()
{
    ui_->setupUi(this);

    ui_->view->setAxisPlayheadHeight(40);

    setStyleSheet(R"(
    QLineEdit {
        background-color: #2b2b2b;
        border: 1px solid #333333;
        border-radius: 3px;
        padding: 2px 4px;
        color: #ffffff;
    }
    tl--TimelineRangeSlider {
        background-color: #2b2b2b;
    }
    )");

    // TODO: Only for test
    addAction("Add", QString("Ctrl+N"), this, [this] {
        qint64 start = ui_->view->axis()->frame();
        ui_->view->model()->createItem(TimelineArmItem::Type, 0, start, 0, true);
    });
    addAction("Save", QString("Ctrl+S"), this, [this] { qDebug() << ui_->view->model()->save().dump(4).c_str(); });
}

void TimelineEdit::initData()
{
    ui_->ranger->setFrameRange(0, 14400);
    ui_->ranger->setFrameMode(false);
    ui_->ranger->slider()->setViewFrameMaximum(14400);

    ui_->view->model()->setFrameMinimum(ui_->ranger->slider()->viewFrameMinimum());
    ui_->view->model()->setFrameMaximum(ui_->ranger->slider()->viewFrameMaximum());
    ui_->view->model()->setFps(ui_->ranger->fps());
}

void TimelineEdit::setupSignals()
{
    connect(ui_->ranger->slider(), &TimelineRangeSlider::viewMinimumChanged, ui_->view->model(), &TimelineModel::setFrameMinimum);
    connect(ui_->ranger->slider(), &TimelineRangeSlider::viewMaximumChanged, ui_->view->model(), &TimelineModel::setFrameMaximum);
    connect(ui_->ranger, &TimelineRanger::fpsChanged, ui_->view->model(), &TimelineModel::setFps);
}

void TimelineEdit::setAxisPlayheadHeight(int height)
{
    ui_->view->setAxisPlayheadHeight(height);
}

void TimelineEdit::setRowCount(int row_count)
{
    ui_->view->model()->setRowCount(row_count);
}

} // namespace tl