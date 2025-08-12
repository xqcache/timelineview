#include "timelineaudioitemview.h"
#include "item/timelineaudioitem.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include "timelineutil.h"
#include "timelineview.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace tl {

void TimelineAudioItemView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
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
    if (!waveform_image_.isNull()) {
        // 计算源图像中对应的区域
        QRectF source_rect((visible_rect.x() / bounding_rect.width()) * waveform_image_.width(),
            (visible_rect.y() / bounding_rect.height()) * waveform_image_.height(), (visible_rect.width() / bounding_rect.width()) * waveform_image_.width(),
            (visible_rect.height() / bounding_rect.height()) * waveform_image_.height());

        painter->drawImage(visible_rect, waveform_image_, source_rect);
    }

    // 绘制边框 - 只绘制可见部分的边框
    painter->setPen(QPen(isSelected() ? Qt::yellow : Qt::red, 2));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(visible_rect);
}

QRectF TimelineAudioItemView::calcBoundingRect() const
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

void TimelineAudioItemView::updateWaveformImage()
{
    auto* item = model()->item<TimelineAudioItem>(item_id_);
    if (!item) [[unlikely]] {
        return;
    }
    if (bounding_rect_.isNull()) {
        return;
    }

    int left = qMax<qreal>(0.0, 0.0 - x());
    int right = qMin<qreal>(bounding_rect_.width(), TimelineUtil::getMaxScreenWidth() - x());

    if (right <= left || left < 0) {
        return;
    }
    waveform_image_ = TimelineMediaUtil::drawWaveform(pcm_data_, left, right, bounding_rect_.width(), model()->itemHeight(), item->isEnabled());
}

void TimelineAudioItemView::updateWaveformData()
{
    auto* item = model()->item<TimelineAudioItem>(item_id_);
    if (!item) [[unlikely]] {
        return;
    }
    if (bounding_rect_.isNull()) {
        return;
    }
    QString path = item->path();
    pcm_data_ = TimelineMediaUtil::loadAudioWaveform(path);
}

bool TimelineAudioItemView::onItemChanged(int role)
{
    bool processed = TimelineItemView::onItemChanged(role);

    if (role & TimelineAudioItem::AudioInfoRole) {
        bounding_rect_ = calcBoundingRect();
        updateWaveformData();
        updateWaveformImage();
        processed = true;
    } else if (role & TimelineItem::EnabledRole) {
        updateWaveformImage();
        update();
        processed = true;
    }

    return processed;
}

void TimelineAudioItemView::refreshCache()
{
    updateWaveformImage();
    update();
}

} // namespace tl