#include "timelineranger.h"
#include "ui_timelineranger.h"
#include <QIntValidator>
#include <QRegularExpressionValidator>

namespace tl {

struct TimelineRangerPrivate {

    bool ignore_frame_range_signal = false;
};

TimelineRanger::TimelineRanger(QWidget* parent)
    : QWidget(parent)
    , ui_(new Ui::TimelineRanger)
    , d_(new TimelineRangerPrivate)
{
    initUi();
    setupSignals();
}

TimelineRanger::~TimelineRanger() noexcept
{
    delete ui_;
    delete d_;
}

void TimelineRanger::initUi()
{
    ui_->setupUi(this);

    ui_->combo_fps->addItem("24 fps", 24.0);
    ui_->combo_fps->addItem("30 fps", 30.0);
    ui_->combo_fps->addItem("50 fps", 50.0);
    ui_->combo_fps->addItem("60 fps", 60.0);

    setFps(ui_->combo_fps->itemData(ui_->combo_fps->currentIndex()).toDouble());
}

void TimelineRanger::setupSignals()
{
    connect(ui_->slider, &TimelineRangeSlider::frameRangeChanged, this, [this](qint64 minimum, qint64 maximum) {
        if (d_->ignore_frame_range_signal) {
            return;
        }

        {
            QSignalBlocker blocker(ui_->lnedt_minimum);
            ui_->lnedt_minimum->setText(ui_->slider->valueToText(minimum));
            ui_->lnedt_minimum->setProperty("minimum", minimum);
        }
        {
            QSignalBlocker blocker(ui_->lnedt_maximum);
            ui_->lnedt_maximum->setText(ui_->slider->valueToText(maximum));
            ui_->lnedt_maximum->setProperty("maximum", maximum);
        }
    });

    connect(ui_->lnedt_maximum, &QLineEdit::editingFinished, this, [this] {
        const auto text = ui_->lnedt_maximum->text();
        if (text == ui_->lnedt_maximum->property("maximum").toString()) {
            return;
        }
        if (ui_->slider->checkFrameMaximumValid(text)) {
            {
                d_->ignore_frame_range_signal = true;
                ui_->slider->setFrameMaximum(text);
            }
            {
                QSignalBlocker blocker(ui_->lnedt_maximum);
                ui_->lnedt_maximum->setText(ui_->slider->frameMaximumText());
                ui_->lnedt_maximum->setProperty("maximum", ui_->lnedt_maximum->text());
            }
        } else {
            QSignalBlocker blocker(ui_->lnedt_maximum);
            ui_->lnedt_maximum->setText(ui_->lnedt_maximum->property("maximum").toString());
        }
    });

    connect(ui_->lnedt_minimum, &QLineEdit::editingFinished, this, [this] {
        const auto text = ui_->lnedt_minimum->text();
        if (text == ui_->lnedt_minimum->property("minimum").toString()) {
            return;
        }
        if (ui_->slider->checkFrameMinimumValid(text)) {
            {
                d_->ignore_frame_range_signal = true;
                ui_->slider->setFrameMinimum(text);
            }
            {
                QSignalBlocker blocker(ui_->lnedt_minimum);
                ui_->lnedt_minimum->setText(ui_->slider->frameMinimumText());
                ui_->lnedt_minimum->setProperty("minimum", ui_->lnedt_minimum->text());
            }
        } else {
            QSignalBlocker blocker(ui_->lnedt_minimum);
            ui_->lnedt_minimum->setText(ui_->lnedt_minimum->property("minimum").toString());
        }
    });

    connect(ui_->combo_fps, &QComboBox::currentIndexChanged, this, [this](int index) {
        const auto fps = ui_->combo_fps->itemData(index).toDouble();
        setFps(fps);
        emit fpsChanged(fps);
    });
}

void TimelineRanger::setFrameMode(bool on)
{
    ui_->slider->setFrameMode(on);

    if (on) {
        ui_->lnedt_minimum->setInputMask("");
        ui_->lnedt_maximum->setInputMask("");
        auto* validator = new QIntValidator(ui_->slider->frameMinimum(), ui_->slider->frameMaximum(), this);
        validator->setRange(0, 999999999);
        ui_->lnedt_minimum->setValidator(validator);
        ui_->lnedt_maximum->setValidator(validator);
    } else {
        ui_->lnedt_minimum->setInputMask("00:00:00:000");
        ui_->lnedt_maximum->setInputMask("00:00:00:000");
        const auto* old_validator = ui_->lnedt_minimum->validator();
        ui_->lnedt_minimum->setValidator(nullptr);
        ui_->lnedt_maximum->setValidator(nullptr);
        if (old_validator) {
            delete old_validator;
        }
    }
    ui_->lnedt_minimum->setText(ui_->slider->frameMinimumText());
    ui_->lnedt_maximum->setText(ui_->slider->frameMaximumText());
    ui_->lnedt_minimum->setProperty("minimum", ui_->lnedt_minimum->text());
    ui_->lnedt_maximum->setProperty("maximum", ui_->lnedt_maximum->text());
}

void TimelineRanger::setFps(double fps)
{
    ui_->slider->setFps(fps);
    ui_->lnedt_minimum->setText(ui_->slider->frameMinimumText());
    ui_->lnedt_maximum->setText(ui_->slider->frameMaximumText());
    ui_->lnedt_minimum->setProperty("minimum", ui_->lnedt_minimum->text());
    ui_->lnedt_maximum->setProperty("maximum", ui_->lnedt_maximum->text());

    QSignalBlocker blocker(ui_->combo_fps);
    int index = ui_->combo_fps->findData(fps);
    if (index != -1) {
        ui_->combo_fps->setCurrentIndex(index);
    } else {
        ui_->combo_fps->addItem(QString::number(fps) + " fps", fps);
        ui_->combo_fps->setCurrentIndex(ui_->combo_fps->count() - 1);
    }
}

void TimelineRanger::setFrameMinimum(qint64 minimum)
{
    {
        QSignalBlocker blocker(ui_->slider);
        ui_->slider->setFrameMinimum(minimum);
    }
    {
        QSignalBlocker blocker(ui_->lnedt_minimum);
        ui_->lnedt_minimum->setText(ui_->slider->valueToText(minimum));
        ui_->lnedt_minimum->setProperty("minimum", minimum);
    }
}

void TimelineRanger::setFrameMaximum(qint64 maximum)
{
    {
        QSignalBlocker blocker(ui_->slider);
        ui_->slider->setFrameMaximum(maximum);
    }
    {
        QSignalBlocker blocker(ui_->lnedt_maximum);
        ui_->lnedt_maximum->setText(ui_->slider->valueToText(maximum));
        ui_->lnedt_maximum->setProperty("maximum", maximum);
    }
}

TimelineRangeSlider* TimelineRanger::slider() const
{
    return ui_->slider;
}

double TimelineRanger::fps() const
{
    return ui_->slider->fps();
}
} // namespace tl