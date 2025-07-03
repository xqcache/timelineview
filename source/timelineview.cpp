#include "timelineview.h"
#include "item/timelinearmitem.h"
#include "timelineaxis.h"
#include "timelinemodel.h"
#include "timelineranger.h"
#include "timelinerangeslider.h"
#include "timelinescene.h"
#include <QMouseEvent>

namespace tl {

struct TimelineViewPrivate {
    QWidget* vbar_filler { nullptr };
    TimelineAxis* axis { nullptr };
    TimelineModel* model { nullptr };
    TimelineScene* scene { nullptr };
    TimelineRanger* ranger { nullptr };
};

TimelineView::TimelineView(QWidget* parent)
    : QGraphicsView(parent)
    , d_(new TimelineViewPrivate)
{
    d_->model = new TimelineModel(this);
    d_->scene = new TimelineScene(d_->model, this);
    d_->ranger = new TimelineRanger(this);
    setScene(d_->scene);

    initUi();
    setupSignals();
    initData();
}

TimelineView::~TimelineView() noexcept
{
    delete d_;
}

void TimelineView::initUi()
{
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setTransformationAnchor(NoAnchor);
    setResizeAnchor(NoAnchor);
    setFrameShape(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_->axis = new TimelineAxis(this);
    d_->vbar_filler = new QWidget(this);
    d_->vbar_filler->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    addScrollBarWidget(d_->vbar_filler, Qt::AlignTop);

    setSceneSize(1000000, 200);
    setAxisPlayheadHeight(40);

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
        qint64 start = d_->axis->frame();
        d_->model->createItem(TimelineArmItem::Type, 0, start, 0, true);
    });
    addAction("Save", QString("Ctrl+S"), this, [this] { qDebug() << d_->model->save().dump(4).c_str(); });
}

bool TimelineView::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick: {
        return viewportEvent(event);
    } break;
    case QEvent::ToolTip: {
        return viewportEvent(event);
    } break;
    default:
        break;
    }
    return QGraphicsView::event(event);
}

void TimelineView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);

    int ranger_height = d_->ranger->sizeHint().height();
    d_->axis->setGeometry(0, 0, viewport()->width(), height() - ranger_height);

    auto viewport_margins = viewportMargins();
    viewport_margins.setBottom(ranger_height);
    setViewportMargins(viewport_margins);
    d_->ranger->setGeometry(0, height() - ranger_height, width(), ranger_height);
}

void TimelineView::setScene(TimelineScene* scene)
{
    if (!scene) {
        return;
    }
    if (scene == this->scene()) {
        return;
    }
    d_->scene = scene;
    scene->setView(this);
    QGraphicsView::setScene(scene);
}

void TimelineView::setAxisPlayheadHeight(int height)
{
    d_->vbar_filler->setFixedHeight(height);
    d_->axis->setPlayheadHeight(height);
    setViewportMargins(0, height, 0, 0);
}

TimelineAxis* TimelineView::axis() const
{
    return d_->axis;
}

TimelineModel* TimelineView::model() const
{
    return d_->model;
}

void TimelineView::setFrameMode(bool on)
{
}

void TimelineView::setSceneSize(qreal width, qreal height)
{
    setSceneRect(0, 0, width, height);
}

void TimelineView::setSceneWidth(qreal width)
{
    setSceneRect(0, 0, width, sceneRect().height());
}

void TimelineView::setupSignals()
{
    connect(d_->model, &TimelineModel::frameMaximumChanged, this, &TimelineView::onFrameMaximumChanged);
    connect(d_->model, &TimelineModel::frameMinimumChanged, this, &TimelineView::onFrameMinimumChanged);
    connect(d_->model, &TimelineModel::fpsChanged, d_->axis, &TimelineAxis::setFps);

    connect(d_->ranger->slider(), &TimelineRangeSlider::viewMinimumChanged, d_->model, &TimelineModel::setFrameMinimum);
    connect(d_->ranger->slider(), &TimelineRangeSlider::viewMaximumChanged, d_->model, &TimelineModel::setFrameMaximum);
    connect(d_->ranger, &TimelineRanger::fpsChanged, d_->model, &TimelineModel::setFps);
}

void TimelineView::onFrameMaximumChanged(qint64 value)
{
    if (!d_->scene) {
        return;
    }
    d_->axis->setMaximum(value);
    d_->scene->fitInAxis();
}

void TimelineView::onFrameMinimumChanged(qint64 value)
{
    if (!d_->scene) {
        return;
    }
    d_->axis->setMinimum(value);
    d_->scene->fitInAxis();
}

void TimelineView::drawBackground(QPainter* painter, const QRectF& rect)
{
    QGraphicsView::drawBackground(painter, rect);
    painter->fillRect(rect, backgroundBrush());

    painter->setBrush(Qt::red);
    painter->drawEllipse(0, 0, 10, 10);
}

qreal TimelineView::mapFromSceneX(qreal x) const
{
    return mapFromScene(QPointF(x, 0)).x();
}

qreal TimelineView::mapToSceneX(qreal x) const
{
    return mapToScene(QPointF(x, 0).toPoint()).x();
}

bool TimelineView::isInView(qreal x, qreal width) const
{
    qreal view_x = mapFromSceneX(x);
    return view_x >= 0 && view_x + width <= this->width();
}

void TimelineView::initData()
{
    d_->ranger->setFrameRange(0, 14400);
    d_->ranger->setFrameMode(false);
    d_->ranger->slider()->setViewFrameMaximum(14400);

    d_->model->setFrameMinimum(d_->ranger->slider()->viewFrameMinimum());
    d_->model->setFrameMaximum(d_->ranger->slider()->viewFrameMaximum());
    d_->model->setFps(d_->ranger->fps());
}

} // namespace tl