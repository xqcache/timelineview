#include "waitingmaskview.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>
#include <QSpacerItem>
#include <QVBoxLayout>

struct WaitingMaskViewPrivate {
    QLabel* lbl_text { nullptr };
    QLabel* lbl_movie { nullptr };
    QMovie* movie { nullptr };
};

WaitingMaskView::WaitingMaskView(QWidget* parent)
    : QWidget(parent)
    , d_(new WaitingMaskViewPrivate)
{
    initUi();
}

WaitingMaskView::~WaitingMaskView() noexcept
{
    delete d_;
}

void WaitingMaskView::initUi()
{
    d_->movie = new QMovie(":/timeline/wait.gif", "GIF", this);

    d_->lbl_text = new QLabel(this);
    d_->lbl_movie = new QLabel(this);
    d_->lbl_text->setText("Please wait...");

    d_->lbl_movie->setMinimumSize(100, 100);

    auto* lyt_horizontal = new QHBoxLayout();
    lyt_horizontal->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred));
    lyt_horizontal->addWidget(d_->lbl_text);
    lyt_horizontal->addWidget(d_->lbl_movie);
    lyt_horizontal->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred));

    auto* lyt_main = new QVBoxLayout(this);
    lyt_main->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));
    lyt_main->addLayout(lyt_horizontal);
    lyt_main->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));

    connect(d_->movie, &QMovie::frameChanged, d_->lbl_movie, [this](int frame_no) {
        auto pixmap = d_->movie->currentPixmap();
        d_->lbl_movie->setPixmap(pixmap.scaled(d_->lbl_movie->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    });
}

void WaitingMaskView::exec()
{
    d_->movie->start();
    show();
}

void WaitingMaskView::setText(const QString& text)
{
    d_->lbl_text->setText(text);
}