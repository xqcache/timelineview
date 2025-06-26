#include "timelineview.h"
#include "timelineaxis.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include <QMouseEvent>
#include <QScrollBar>

namespace tl {
struct TimelineViewPrivate {
    TimelineAxis* axis { nullptr };
    QWidget* vbar_filler { nullptr };
    bool skip_hbar_signal = false;
};

TimelineView::TimelineView(QWidget* parent)
    : QGraphicsView(parent)
    , d_(new TimelineViewPrivate)
{
    initUi();
    setupSignals();
}

TimelineView::~TimelineView() noexcept
{
    delete d_;
}

void TimelineView::setAxisPlayheadHeight(int height)
{
    d_->vbar_filler->setFixedHeight(height);
    d_->axis->setPlayheadHeight(height);
    setViewportMargins(0, height, 0, 0);
}

void TimelineView::setScene(TimelineScene* scene)
{
    if (!scene) {
        return;
    }
    if (scene == this->scene()) {
        return;
    }
    scene->setView(this);
    QGraphicsView::setScene(scene);
    setSceneRect(0, 0, qMax<qreal>(viewport()->width(), d_->axis->rulerLength()), scene->model()->rowCount() * scene->model()->itemHeight());
}

qreal TimelineView::mapToAxis(qint64 time) const
{
    return d_->axis->mapToAxis(time);
}

qreal TimelineView::mapToAxisX(qint64 time) const
{
    return d_->axis->mapToAxisX(time);
}

qreal TimelineView::axisTickWidth() const
{
    return d_->axis->tickWidth();
}

void TimelineView::movePlayhead(qint64 time)
{
    d_->axis->movePlayhead(time);
}

qint64 TimelineView::currentTime() const
{
    return d_->axis->currentTime();
}

bool TimelineView::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick: {
        QMouseEvent* old_evt = static_cast<QMouseEvent*>(event);
        QMouseEvent* new_evt = new QMouseEvent(old_evt->type(), viewport()->mapFromGlobal(old_evt->globalPosition()), old_evt->globalPosition(),
            old_evt->button(), old_evt->buttons(), old_evt->modifiers(), old_evt->pointingDevice());
        qApp->sendEvent(viewport(), new_evt);
        return true;
    } break;
    case QEvent::ContextMenu: {
        QContextMenuEvent* old_evt = static_cast<QContextMenuEvent*>(event);
        QContextMenuEvent* new_evt
            = new QContextMenuEvent(old_evt->reason(), viewport()->mapFromGlobal(old_evt->globalPos()), old_evt->globalPos(), old_evt->modifiers());
        qApp->sendEvent(viewport(), new_evt);
        return true;
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

    auto viewport_geo = viewport()->geometry();
    d_->axis->setGeometry(viewport_geo.left(), 0, viewport_geo.width(), viewport_geo.bottom());
}

void TimelineView::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillRect(rect, backgroundBrush());

    // // TODO: 绘制一个虚拟的原点，后期删除
    // painter->setPen(Qt::red);
    // painter->setBrush(Qt::red);
    // painter->drawEllipse(0, 0, 10, 10);

    // painter->setPen(Qt::red);
    // painter->setBrush(Qt::red);
    // painter->drawEllipse(sceneRect().right() - 10, 0, 10, 10);

    // painter->setBrush(Qt::NoBrush);
    // painter->setPen(Qt::green);
    // painter->drawRect(sceneRect());
}

void TimelineView::initUi()
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCacheMode(QGraphicsView::CacheBackground);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setFrameShape(QFrame::NoFrame);

    setBackgroundBrush(QColor("#BDBDBD"));
    d_->axis = new TimelineAxis(this);
    d_->vbar_filler = new QWidget(this);
    d_->vbar_filler->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    addScrollBarWidget(d_->vbar_filler, Qt::AlignTop);
}

void TimelineView::setupSignals()
{
    connect(d_->axis, &TimelineAxis::rulerScrolled, this, &TimelineView::onAxisRulerScrolled);
    connect(d_->axis, &TimelineAxis::rulerScaled, this, &TimelineView::onAxisRulerScaled);
    connect(d_->axis, &TimelineAxis::rangeChanged, this, &TimelineView::onAxisRangeChanged);
    connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, [this](int scene_x) {
        if (d_->skip_hbar_signal) {
            d_->skip_hbar_signal = false;
            return;
        }
        QSignalBlocker blocker(d_->axis);
        d_->axis->scrollPlayhead(scene_x);
    });
}

void TimelineView::onAxisRulerScaled()
{
    if (auto* scene = qobject_cast<TimelineScene*>(this->scene()); scene) {
        auto rect = sceneRect();
        rect.setWidth(qMax<qreal>(viewport()->width(), d_->axis->rulerLength()));
        setSceneRect(rect);
        scene->fitInAxis();
    }
}

void TimelineView::onAxisRulerScrolled(qint64 start_time)
{
    if (auto* tl_scene = qobject_cast<TimelineScene*>(scene()); tl_scene) {
        d_->skip_hbar_signal = true;
        horizontalScrollBar()->setValue(d_->axis->mapToAxis(start_time, 0));
    }
}

void TimelineView::onAxisRangeChanged(qint64 min, qint64 max)
{
    if (auto* tl_scene = qobject_cast<TimelineScene*>(scene()); tl_scene) {
        auto rect = sceneRect();
        rect.setWidth(qMax<qreal>(viewport()->width(), d_->axis->rulerLength()));
        setSceneRect(rect);
    }
}

} // namespace tl