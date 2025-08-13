#pragma once

#include "itemview/timelineitemview.h"

namespace tl {

class TimelineAudioItemView : public TimelineItemView {
public:
    using TimelineItemView::TimelineItemView;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    bool onItemChanged(int role) override;

    void refreshCache() override;
    void rebuildCache() override;

private:
    QRectF calcBoundingRect() const override;

    void updateWaveformImage();
    void updateWaveformData();

private:
    QList<int16_t> pcm_data_;
    QImage waveform_image_;
};

} // namespace tl