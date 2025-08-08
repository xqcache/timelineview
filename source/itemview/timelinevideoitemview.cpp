#include "timelinevideoitemview.h"
#include "item/timelinevideoitem.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include <QGuiApplication>
#include <QPainter>
#include <QScreen>

namespace tl {

void TimelineVideoItemView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const auto& bounding_rect = boundingRect();
    if (bounding_rect.isEmpty()) {
        return;
    }

    painter->setClipRect(bounding_rect);
    if (!painter_thumbnail_.isNull()) {
        painter->drawImage(0, 0, painter_thumbnail_);
    }
    painter->setPen(QPen(Qt::red, 4));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(bounding_rect);
    qDebug() << x() << bounding_rect;
}

void TimelineVideoItemView::fitInAxis()
{
    TimelineItemView::fitInAxis();
    updatePainterThumbnail();
}

QRectF TimelineVideoItemView::calcBoundingRect() const
{
    QRectF result;
    if (!model()) [[unlikely]] {
        return result;
    }
    auto* item = model()->item(item_id_);
    if (!item) [[unlikely]] {
        return result;
    }

    auto duration = item->duration();
    qreal tick_width = sceneRef().axisTickWidth();
    qreal width = sceneRef().mapFrameToAxis(duration);
    qreal height = model()->itemHeight();
    result = QRectF(0, 0, width, height);
    return result;
}

void TimelineVideoItemView::updatePainterThumbnail()
{
    auto* item = model()->item<TimelineVideoItem>(item_id_);
    if (!item) [[unlikely]] {
        return;
    }
    if (bounding_rect_.isNull()) {
        return;
    }
    QString path = item->path();

    const auto& media_info = item->mediaInfo();
    qreal scaled_height = model()->itemHeight();
    qreal scaled_width = scaled_height * media_info.size.width() / media_info.size.height();
    int count = bounding_rect_.width() / scaled_width;
    int step = qMax(thumbnails_.size() / count, 1);

    if (thumbnails_.isEmpty()) {
        return;
    }

    QImage thumbnail(bounding_rect_.width(), model()->itemHeight(), QImage::Format_ARGB32);
    QPainter painter(&thumbnail);

    int x = 0;
    for (int i = 0; i < thumbnails_.size(); i += step) {
        if (thumbnails_[i].isNull()) {
            continue;
        }
        painter.drawImage(x, 0, thumbnails_[i]);
        x += thumbnails_[i].width();
    }
    painter_thumbnail_ = thumbnail;
}

void TimelineVideoItemView::updateThumbnails()
{
    auto* item = model()->item<TimelineVideoItem>(item_id_);
    if (!item) [[unlikely]] {
        return;
    }
    if (bounding_rect_.isNull()) {
        return;
    }
    QString path = item->path();

    const auto& media_info = item->mediaInfo();
    qreal scaled_height = model()->itemHeight();
    qreal scaled_width = scaled_height * media_info.size.width() / media_info.size.height();
    int count = getMaxScreenWidth() / scaled_width;
    int step = qMax(media_info.frame_count / count, 1);
    thumbnails_ = TimelineMediaUtil::loadThumbnails(path, model()->itemHeight(), step);
}

bool TimelineVideoItemView::onItemChanged(int role)
{
    bool processed = TimelineItemView::onItemChanged(role);
    if (role & TimelineVideoItem::MediaInfoRole) {
        bounding_rect_ = calcBoundingRect();
        updateThumbnails();
        updatePainterThumbnail();
        processed = true;
    }

    return processed;
}

int TimelineVideoItemView::getMaxScreenWidth()
{
    int width = 1080;
    QList<QScreen*> screens = QGuiApplication::screens();
    for (const auto& screen : screens) {
        if (screen->geometry().width() > width) {
            width = qMax(width, screen->geometry().width());
        }
    }
    return width;
}

} // namespace tl