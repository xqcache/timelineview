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
    TimelineScene* scene { nullptr };
    TimelineRanger* ranger { nullptr };
    QList<QMetaObject::Connection> model_connections;
};

TimelineView::TimelineView(QWidget* parent)
    : QGraphicsView(parent)
    , d_(new TimelineViewPrivate)
{

    d_->ranger = new TimelineRanger(this);
    initUi();
    setupSignals();
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
}

bool TimelineView::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::ToolTip:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::MouseButtonDblClick: {
        return viewportEvent(event);
    } break;
    case QEvent::ContextMenu: {
        QContextMenuEvent* old_event = static_cast<QContextMenuEvent*>(event);
        QContextMenuEvent new_event(old_event->reason(), viewport()->mapFromGlobal(old_event->globalPos()), old_event->globalPos(), old_event->modifiers());
        return viewportEvent(&new_event);
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

    if (d_->scene) {
        d_->scene->fitInAxis();
    }
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

    connect(d_->ranger->slider(), &TimelineRangeSlider::sliderReleased, scene, &TimelineScene::refreshCache);

    for (auto& connection : d_->model_connections) {
        disconnect(connection);
    }
    d_->model_connections.clear();

    auto* model = d_->scene->model();

    d_->model_connections.emplace_back(connect(model, &TimelineModel::viewFrameMaximumChanged, this, &TimelineView::onViewFrameMaximumChanged));
    d_->model_connections.emplace_back(connect(model, &TimelineModel::viewFrameMinimumChanged, this, &TimelineView::onViewFrameMinimumChanged));
    d_->model_connections.emplace_back(connect(model, &TimelineModel::frameMaximumChanged, this, &TimelineView::onFrameMaximumChanged));
    d_->model_connections.emplace_back(connect(model, &TimelineModel::frameMinimumChanged, this, &TimelineView::onFrameMinimumChanged));
    d_->model_connections.emplace_back(connect(model, &TimelineModel::fpsChanged, this, &TimelineView::onFpsChanged));

    d_->model_connections.emplace_back(
        connect(d_->ranger->slider(), &TimelineRangeSlider::frameRangeChanged, model, [this, model](qint64 minimum, qint64 maximum) {
            QSignalBlocker blocker(model);
            model->setFrameMaximum(maximum);
            model->setFrameMinimum(minimum);
        }));
    d_->model_connections.emplace_back(connect(d_->ranger->slider(), &TimelineRangeSlider::viewMinimumChanged, model, &TimelineModel::setViewFrameMinimum));
    d_->model_connections.emplace_back(connect(d_->ranger->slider(), &TimelineRangeSlider::viewMaximumChanged, model, &TimelineModel::setViewFrameMaximum));
    d_->model_connections.emplace_back(connect(d_->ranger, &TimelineRanger::fpsChanged, model, &TimelineModel::setFps));
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
    if (!d_->scene) {
        return nullptr;
    }
    return d_->scene->model();
}

void TimelineView::setFormat(FrameFormat fmt)
{
    d_->ranger->setFormat(fmt);
    d_->axis->setFormat(fmt);
}

FrameFormat TimelineView::format() const
{
    return d_->axis->format();
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
}

void TimelineView::onViewFrameMaximumChanged(qint64 value)
{
    if (!d_->scene) {
        return;
    }
    {
        QSignalBlocker blocker(d_->ranger->slider());
        d_->ranger->slider()->setViewFrameMaximum(value);
    }
    d_->axis->setMaximum(value);
    d_->scene->fitInAxis();
}

void TimelineView::onViewFrameMinimumChanged(qint64 value)
{
    if (!d_->scene) {
        return;
    }
    {
        QSignalBlocker blocker(d_->ranger->slider());
        d_->ranger->slider()->setViewFrameMinimum(value);
    }
    d_->axis->setMinimum(value);
    d_->scene->fitInAxis();
}

void TimelineView::onFrameMaximumChanged(qint64 value)
{
    QSignalBlocker blocker(d_->ranger->slider());
    d_->ranger->setFrameMaximum(value);
}

void TimelineView::onFrameMinimumChanged(qint64 value)
{
    QSignalBlocker blocker(d_->ranger->slider());
    d_->ranger->setFrameMinimum(value);
}

void TimelineView::onFpsChanged(double fps)
{
    d_->axis->setFps(fps);
    d_->ranger->setFps(fps);
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

} // namespace tl