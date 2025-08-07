#include "timelinevideoitemview.h"
#include "item/timelinevideoitem.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include <QPainter>

namespace tl {

void TimelineVideoItemView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const auto& bounding_rect = boundingRect();
    if (bounding_rect.isEmpty()) {
        return;
    }

    painter->setClipRect(bounding_rect);
    if (!thumbnail_.isNull()) {
        painter->drawImage(0, 0, thumbnail_);
    }
    painter->setPen(QPen(Qt::red, 4));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(bounding_rect);
    qDebug() << x() << bounding_rect;
}

void TimelineVideoItemView::fitInAxis()
{
    TimelineItemView::fitInAxis();
    updateThumbnail();
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

void TimelineVideoItemView::updateThumbnail()
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
    int step = qMax(media_info.frame_count / count, 1);
    const auto& thumbnails = TimelineMediaUtil::loadThumbnails(path, model()->itemHeight(), step);
    if (thumbnails.isEmpty()) {
        return;
    }

    QImage thumbnail(bounding_rect_.width(), model()->itemHeight(), QImage::Format_ARGB32);
    QPainter painter(&thumbnail);

    int x = 0;
    for (const auto& img : thumbnails) {
        if (img.isNull()) {
            continue;
        }
        painter.drawImage(x, 0, img);
        x += img.width();
    }
    if (!thumbnail.isNull()) {
        thumbnail_ = thumbnail;
    } else {
        TL_LOG_ERROR("Failed to create thumbnail for video item: {}", path.toStdString());
    }
}

bool TimelineVideoItemView::onItemChanged(int role)
{
    bool processed = TimelineItemView::onItemChanged(role);
    if (role & TimelineVideoItem::MediaInfoRole) {
        bounding_rect_ = calcBoundingRect();
        updateThumbnail();
        processed = true;
    }

    return processed;
}

} // namespace tl