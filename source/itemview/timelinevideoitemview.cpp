#include "timelinevideoitemview.h"
#include "item/timelinevideoitem.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include "timelineutil.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace tl {

void TimelineVideoItemView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const auto& bounding_rect = boundingRect();
    if (bounding_rect.isEmpty()) {
        return;
    }

    // 计算可见区域，只绘制在视口内的部分
    QRectF visible_rect = bounding_rect;
    if (option && option->exposedRect.isValid()) {
        visible_rect = bounding_rect.intersected(option->exposedRect);
    }

    if (visible_rect.isEmpty()) {
        return;
    }

    // 绘制波形图 - 只绘制可见部分
    if (!painter_thumbnail_.isNull()) {
        // 计算源图像中对应的区域
        QRectF source_rect((visible_rect.x() / bounding_rect.width()) * painter_thumbnail_.width(),
            (visible_rect.y() / bounding_rect.height()) * painter_thumbnail_.height(),
            (visible_rect.width() / bounding_rect.width()) * painter_thumbnail_.width(),
            (visible_rect.height() / bounding_rect.height()) * painter_thumbnail_.height());

        painter->drawImage(visible_rect, painter_thumbnail_, source_rect);
    }

    painter->setPen(QPen(Qt::red, 4));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(bounding_rect);
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
    int count = qMax(1.0, bounding_rect_.width() / scaled_width);
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
    int count = TimelineUtil::getMaxScreenWidth() / scaled_width;
    int step = qMax(media_info.frame_count / count, 1);
    thumbnails_ = TimelineMediaUtil::loadVideoThumbnails(path, model()->itemHeight(), step);
}

bool TimelineVideoItemView::onItemChanged(int role)
{
    bool processed = TimelineItemView::onItemChanged(role);
    if (role & TimelineVideoItem::VideoInfoRole) {
        bounding_rect_ = calcBoundingRect();
        updateThumbnails();
        updatePainterThumbnail();
        processed = true;
    }

    return processed;
}

void TimelineVideoItemView::refreshCache()
{
    updatePainterThumbnail();
    update();
}

} // namespace tl