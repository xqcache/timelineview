#include "timelinerangeslider.h"
#include "timelineutil.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRegularExpression>

namespace tl {

struct TimelineRangeSliderPrivate {
    int handle_width { 10 };
    double fps { 24.0 };
    QMargins margins { 3, 3, 3, 3 };
    std::array<bool, 2> hovered { false, false };
    std::array<bool, 3> pressed { false, false, false };
    std::array<qint64, 2> view_range { 0, 1 };
    std::array<qint64, 2> frame_range { 0, 1 };
    FrameFormat fmt { FrameFormat::TimeCode };
    bool range_changed { false };
};

TimelineRangeSlider::TimelineRangeSlider(QWidget* parent)
    : QWidget(parent)
    , d_(new TimelineRangeSliderPrivate)
{
    initUi();
    setMouseTracking(true);
}

TimelineRangeSlider::~TimelineRangeSlider() noexcept
{
    delete d_;
}

QSize TimelineRangeSlider::minimumSizeHint() const
{
    return QSize(d_->fmt == FrameFormat::Frame ? 100 : 250, 20 + d_->margins.top() + d_->margins.bottom());
}

void TimelineRangeSlider::initUi()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

int TimelineRangeSlider::innerWidth() const
{
    return width() - d_->handle_width * 4 - d_->margins.left() - d_->margins.right();
}

int TimelineRangeSlider::innerHeight() const
{
    return height() - d_->margins.top() - d_->margins.bottom();
}

int TimelineRangeSlider::sliderWidth() const
{
    return qRound(deltaX() * (d_->view_range[1] - d_->view_range[0])) + d_->handle_width * 4;
}

void TimelineRangeSlider::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    painter.setPen(Qt::NoPen);
    painter.setBrush(palette().brush(backgroundRole()));
    painter.drawRoundedRect(event->rect(), 3, 3);

    painter.setBrush(d_->hovered[0] ? QColor("#dcdcdc") : QColor("#999999"));
    painter.drawPath(minHandleShape());

    painter.setBrush(d_->hovered[1] ? QColor("#dcdcdc") : QColor("#999999"));
    painter.drawPath(maxHandleShape());

    painter.setBrush(QColor("#444444"));
    painter.drawPath(midHandleShape());

    QString view_min_text = viewMinimumText();
    QString view_max_text = viewMaximumText();
    int view_min_text_w = viewRangeTextWidth(view_min_text);
    int view_max_text_w = viewRangeTextWidth(view_max_text);
    int view_interval_w = viewIntervalWidth();
    int view_min_blank_w = viewMinimumX() - d_->margins.left();
    int view_max_blank_w = width() - viewMaximumX() - d_->margins.right() - d_->handle_width;

    painter.setPen(QColor("#dcdcdc"));
    if (view_min_text_w + view_max_text_w + 4 + d_->handle_width * 2 < view_interval_w) {
        QRect min_text_rect = QRect(viewMinimumX() + d_->handle_width + 2, 0, view_min_text_w, height());
        painter.drawText(min_text_rect, Qt::AlignCenter, view_min_text);
        QRect max_text_rect = QRect(viewMaximumX() - view_max_text_w - 2, 0, view_max_text_w, height());
        painter.drawText(max_text_rect, Qt::AlignCenter, view_max_text);
    } else if (view_min_text_w <= view_min_blank_w && view_max_text_w <= view_max_blank_w) {
        QRect min_text_rect = QRect(viewMinimumX() - view_min_text_w - 2, 0, view_min_text_w, height());
        painter.drawText(min_text_rect, Qt::AlignCenter, view_min_text);
        QRect max_text_rect = QRect(viewMaximumX() + d_->handle_width + 2, 0, view_max_text_w, height());
        painter.drawText(max_text_rect, Qt::AlignCenter, view_max_text);
    } else if (view_min_blank_w > view_max_blank_w) {
        QString middle_text = " — ";
        int middle_text_w = viewRangeTextWidth(middle_text);
        QRect range_text_rect
            = QRect(viewMinimumX() - view_min_text_w - view_max_text_w - 2 - middle_text_w, 0, view_min_text_w + view_max_text_w + 2 + middle_text_w, height());
        painter.drawText(range_text_rect, Qt::AlignCenter, view_min_text + middle_text + view_max_text);
    } else if (view_min_blank_w < view_max_blank_w) {
        QString middle_text = " — ";
        int middle_text_w = viewRangeTextWidth(middle_text);
        QRect range_text_rect = QRect(viewMaximumX() + d_->handle_width + 2, 0, view_min_text_w + view_max_text_w + 2 + middle_text_w, height());
        painter.drawText(range_text_rect, Qt::AlignCenter, view_min_text + middle_text + view_max_text);
    }
}

QPainterPath TimelineRangeSlider::minHandleShape() const
{
    QPainterPath path;

    QRectF rect = QRectF(0, d_->margins.top(), d_->handle_width, innerHeight());
    rect.moveLeft(viewMinimumX());
    path.addRect(rect);

    return path;
}

QPainterPath TimelineRangeSlider::maxHandleShape() const
{
    QPainterPath path;

    QRectF rect = QRectF(0, d_->margins.top(), d_->handle_width, innerHeight());
    rect.moveLeft(viewMaximumX());
    path.addRect(rect);

    return path;
}

QPainterPath TimelineRangeSlider::midHandleShape() const
{
    QPainterPath path;

    QRectF boundary = QRectF(0, d_->margins.top(), d_->handle_width * 2, innerHeight());
    qreal dx = deltaX();
    int left = qRound64(dx * (d_->view_range[0] - d_->frame_range[0])) + d_->margins.left() + d_->handle_width;
    int right = qRound64(dx * (d_->view_range[1] - d_->frame_range[0])) + d_->margins.left() + d_->handle_width * 3;
    boundary.setLeft(left);
    boundary.setRight(right);
    path.addRect(boundary);

    return path;
}

void TimelineRangeSlider::mousePressEvent(QMouseEvent* event)
{
    if (event->modifiers() != Qt::NoModifier || event->buttons() != Qt::LeftButton) {
        return;
    }

    auto pos = event->position();
    if (minHandleShape().contains(pos)) {
        d_->pressed[0] = true;
    } else if (midHandleShape().contains(pos)) {
        d_->pressed[1] = true;
    } else if (maxHandleShape().contains(pos)) {
        d_->pressed[2] = true;
    }
}

void TimelineRangeSlider::mouseMoveEvent(QMouseEvent* event)
{
    auto pos = event->position();
    d_->hovered[0] = minHandleShape().contains(pos);
    d_->hovered[1] = maxHandleShape().contains(pos);

    if (d_->pressed[1]) {
        qreal x = pos.x() - d_->margins.left() - sliderWidth() / 2.0;
        qint64 min_v = qMax(d_->frame_range[0], qRound64(x / deltaX()) + d_->frame_range[0]);
        min_v = qMin(d_->frame_range[1] - (d_->view_range[1] - d_->view_range[0]), min_v);
        qint64 max_v = min_v + (d_->view_range[1] - d_->view_range[0]);
        if (min_v >= d_->frame_range[0] && max_v <= d_->frame_range[1]) {
            setViewMinimum(min_v);
            setViewMaximum(max_v);
        }
    } else if (d_->pressed[0]) {
        qreal x = pos.x() - d_->margins.left() - d_->handle_width / 2.0;
        qint64 min_v = qMax(d_->frame_range[0], qRound64(x / deltaX()) + d_->frame_range[0]);
        min_v = qMin(min_v, d_->view_range[1] - 1);
        setViewMinimum(min_v);
    } else if (d_->pressed[2]) {
        qreal x = pos.x() - d_->margins.left() - d_->handle_width * 3 - d_->handle_width / 2.0;
        qint64 max_v = qMin(d_->frame_range[1], qRound64(x / deltaX()) + d_->frame_range[0]);
        max_v = qMax(max_v, d_->view_range[0] + 1);
        setViewMaximum(max_v);
    }

    update();
}

void TimelineRangeSlider::mouseReleaseEvent(QMouseEvent* event)
{
    if (d_->pressed[0] || d_->pressed[1] || d_->pressed[2]) {
        emit sliderReleased();
    }

    d_->pressed[0] = false;
    d_->pressed[1] = false;
    d_->pressed[2] = false;
}

void TimelineRangeSlider::setViewMinimum(qint64 value)
{
    if (value == d_->view_range[0]) {
        return;
    }
    emit viewMinimumAboutToBeChanged(d_->view_range[0], value);
    d_->view_range[0] = value;
    update();
    emit viewMinimumChanged(d_->view_range[0]);
}

void TimelineRangeSlider::setViewMaximum(qint64 value)
{
    if (value == d_->view_range[1]) {
        return;
    }
    emit viewMaximumAboutToBeChanged(d_->view_range[1], value);
    d_->view_range[1] = value;
    update();
    emit viewMaximumChanged(d_->view_range[1]);
}

void TimelineRangeSlider::setFrameRange(qint64 minimum, qint64 maximum)
{
    if (minimum < 0 || maximum < 0) {
        return;
    }

    if (minimum >= maximum) {
        return;
    }

    bool need_update = false;
    if (minimum != d_->frame_range[0] && minimum < d_->frame_range[1]) {
        d_->frame_range[0] = minimum;
        if (d_->view_range[0] < minimum) {
            emit viewMinimumAboutToBeChanged(d_->view_range[0], minimum);
            d_->view_range[0] = minimum;
            emit viewMinimumChanged(d_->view_range[0]);
        }
        need_update = true;
    }
    if (maximum != d_->frame_range[1] && maximum > d_->frame_range[0]) {
        d_->frame_range[1] = maximum;
        if (d_->view_range[1] > maximum) {
            emit viewMaximumAboutToBeChanged(d_->view_range[1], maximum);
            d_->view_range[1] = maximum;
            emit viewMaximumChanged(d_->view_range[1]);
        }
        need_update = true;
    }
    if (need_update) {
        update();
        emit frameRangeChanged(d_->frame_range[0], d_->frame_range[1]);
    }
}

void TimelineRangeSlider::setFrameMaximum(qint64 maximum)
{
    if (maximum < 0) {
        return;
    }

    if (maximum <= d_->frame_range[0]) {
        return;
    }

    if (maximum == d_->frame_range[1]) {
        return;
    }

    qreal old_min_x = deltaX() * (d_->view_range[0] - d_->frame_range[0]);
    qreal old_max_x = deltaX() * (d_->view_range[1] - d_->frame_range[0]);

    d_->frame_range[1] = maximum;

    qint64 new_view_min = qRound64(old_min_x * detalV()) + d_->frame_range[0];
    qint64 new_view_max = qRound64(old_max_x * detalV()) + d_->frame_range[0];

    new_view_max = qMax(d_->frame_range[0] + 1, qMin(new_view_max, d_->frame_range[1]));
    new_view_min = qMin(new_view_min, new_view_max - 1);

    if (d_->view_range[1] != new_view_max) {
        emit viewMaximumAboutToBeChanged(d_->view_range[1], new_view_max);
        d_->view_range[1] = new_view_max;
        emit viewMaximumChanged(new_view_max);
    }
    if (d_->view_range[0] != new_view_min) {
        emit viewMinimumAboutToBeChanged(d_->view_range[0], new_view_min);
        d_->view_range[0] = new_view_min;
        emit viewMinimumChanged(new_view_min);
    }

    update();
    emit frameRangeChanged(d_->frame_range[0], d_->frame_range[1]);
}

void TimelineRangeSlider::setFrameMinimum(qint64 minimum)
{
    if (minimum < 0) {
        return;
    }
    if (minimum >= d_->frame_range[1]) {
        return;
    }

    if (minimum == d_->frame_range[0]) {
        return;
    }

    qreal old_min_x = deltaX() * (d_->view_range[0] - d_->frame_range[0]);
    qreal old_max_x = deltaX() * (d_->view_range[1] - d_->frame_range[0]);

    d_->frame_range[0] = minimum;

    qint64 new_view_min = qRound64(old_min_x * detalV()) + d_->frame_range[0];
    qint64 new_view_max = qRound64(old_max_x * detalV()) + d_->frame_range[0];

    new_view_min = qMin(d_->frame_range[1] - 1, qMax(d_->frame_range[0], new_view_min));
    new_view_max = qMax(new_view_max, new_view_min + 1);

    if (d_->view_range[1] != new_view_max) {
        emit viewMaximumAboutToBeChanged(d_->view_range[1], new_view_max);
        d_->view_range[1] = new_view_max;
        emit viewMaximumChanged(new_view_max);
    }
    if (d_->view_range[0] != new_view_min) {
        emit viewMinimumAboutToBeChanged(d_->view_range[0], new_view_min);
        d_->view_range[0] = new_view_min;
        emit viewMinimumChanged(new_view_min);
    }

    update();
    emit frameRangeChanged(d_->frame_range[0], d_->frame_range[1]);
}

void TimelineRangeSlider::setFrameMaximum(const QString& text)
{
    switch (d_->fmt) {
    case FrameFormat::Frame:
        setFrameMaximum(text.toLongLong());
    case FrameFormat::TimeCode:
        setFrameMaximum(TimelineUtil::parseTimeCode(text, d_->fps));
    case FrameFormat::TimeString:
        setFrameMaximum(TimelineUtil::parseTimeString(text, d_->fps, false));
    default:
        break;
    }
}

void TimelineRangeSlider::setFrameMinimum(const QString& text)
{
    switch (d_->fmt) {
    case FrameFormat::Frame:
        setFrameMinimum(text.toLongLong());
    case FrameFormat::TimeCode:
        setFrameMinimum(TimelineUtil::parseTimeCode(text, d_->fps));
    case FrameFormat::TimeString:
        setFrameMinimum(TimelineUtil::parseTimeString(text, d_->fps, false));
    default:
        break;
    }
}

void TimelineRangeSlider::setFps(double fps)
{
    if (qFuzzyCompare(d_->fps, fps)) {
        return;
    }
    d_->fps = fps;
    update();
}

double TimelineRangeSlider::fps() const
{
    return d_->fps;
}

QString TimelineRangeSlider::valueToText(qint64 value) const
{
    switch (d_->fmt) {
    case FrameFormat::Frame:
        return QString::number(value);
    case FrameFormat::TimeCode:
        return TimelineUtil::formatTimeCode(value, d_->fps);
    case FrameFormat::TimeString:
        return TimelineUtil::formatTimeString(value, d_->fps, false);
    default:
        break;
    }
    assert(0 && "Invalid format");
    return "";
}

qint64 TimelineRangeSlider::viewFrameMinimum() const
{
    return d_->view_range[0];
}

qint64 TimelineRangeSlider::viewFrameMaximum() const
{
    return d_->view_range[1];
}

void TimelineRangeSlider::setViewFrameMinimum(qint64 minimum)
{
    if (minimum >= d_->view_range[1] || minimum < d_->frame_range[0] || minimum >= d_->frame_range[1]) {
        return;
    }

    d_->view_range[0] = minimum;
    update();
}

void TimelineRangeSlider::setViewFrameMaximum(qint64 maximum)
{
    if (maximum <= d_->view_range[0] || maximum > d_->frame_range[1] || maximum <= d_->frame_range[0]) {
        return;
    }

    d_->view_range[1] = maximum;
    update();
}

QString TimelineRangeSlider::viewMinimumText() const
{
    return valueToText(d_->view_range[0]);
}

QString TimelineRangeSlider::viewMaximumText() const
{
    return valueToText(d_->view_range[1]);
}

int TimelineRangeSlider::viewMinimumX() const
{
    return qRound64(deltaX() * (d_->view_range[0] - d_->frame_range[0])) + d_->margins.left();
}

int TimelineRangeSlider::viewMaximumX() const
{
    return qRound64(deltaX() * (d_->view_range[1] - d_->frame_range[0])) + d_->margins.left() + d_->handle_width * 3;
}

int TimelineRangeSlider::viewIntervalWidth() const
{
    qreal dx = deltaX();
    int left = qRound64(dx * (d_->view_range[0] - d_->frame_range[0])) + d_->margins.left() + d_->handle_width;
    int right = qRound64(dx * (d_->view_range[1] - d_->frame_range[0])) + d_->margins.left() + d_->handle_width * 3;
    return right - left;
}

int TimelineRangeSlider::viewRangeTextWidth(const QString& text) const
{
    return fontMetrics().horizontalAdvance(text);
}

void TimelineRangeSlider::setFormat(FrameFormat fmt)
{
    if (d_->fmt == fmt) {
        return;
    }
    d_->fmt = fmt;
    update();
}

qint64 TimelineRangeSlider::frameMaximum() const
{
    return d_->frame_range[1];
}

qint64 TimelineRangeSlider::frameMinimum() const
{
    return d_->frame_range[0];
}

QString TimelineRangeSlider::frameMaximumText() const
{
    return valueToText(d_->frame_range[1]);
}

QString TimelineRangeSlider::frameMinimumText() const
{
    return valueToText(d_->frame_range[0]);
}

bool TimelineRangeSlider::checkFrameMinimumValid(const QString& min_text) const
{
    qint64 min_value = 0;

    switch (d_->fmt) {
    case FrameFormat::Frame: {
        bool ok = false;
        min_value = min_text.toLongLong(&ok);
        if (!ok || min_value < 0) {
            return false;
        }
        break;
    }
    case FrameFormat::TimeCode: {
        min_value = TimelineUtil::parseTimeCode(min_text, d_->fps);
        if (min_value < 0) {
            return false;
        }
    } break;
    case FrameFormat::TimeString: {
        min_value = TimelineUtil::parseTimeString(min_text, d_->fps, false);
        if (min_value < 0) {
            return false;
        }
    } break;
    default:
        assert(0 && "Invalid format");
        break;
    }

    return min_value < d_->frame_range[1];
}

bool TimelineRangeSlider::checkFrameMaximumValid(const QString& max_text) const
{
    qint64 max_value = 0;

    switch (d_->fmt) {
    case FrameFormat::Frame: {
        bool ok = false;
        max_value = max_text.toLongLong(&ok);
        if (!ok || max_value < 0) {
            return false;
        }
    } break;
    case FrameFormat::TimeCode: {
        max_value = TimelineUtil::parseTimeCode(max_text, d_->fps);
        if (max_value < 0) {
            return false;
        }
    } break;
    case FrameFormat::TimeString: {
        max_value = TimelineUtil::parseTimeString(max_text, d_->fps, false);
        if (max_value < 0) {
            return false;
        }
    } break;
    default:
        assert(0 && "Invalid format");
        break;
    }

    return max_value > d_->frame_range[0];
}

qreal TimelineRangeSlider::deltaX() const
{
    return 1.0 * innerWidth() / (d_->frame_range[1] - d_->frame_range[0]);
}

qreal TimelineRangeSlider::detalV() const
{
    return 1.0 * (d_->frame_range[1] - d_->frame_range[0]) / innerWidth();
}

} // namespace tl