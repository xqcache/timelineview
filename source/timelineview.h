#pragma once

#include "timelinelibexport.h"
#include <QGraphicsView>

namespace tl {
class TimelineScene;

struct TimelineViewPrivate;
class TIMELINE_LIB_EXPORT TimelineView : public QGraphicsView {
    Q_OBJECT
public:
    explicit TimelineView(QWidget* parent = nullptr);
    ~TimelineView() noexcept override;

    void setAxisPlayheadHeight(int height);
    void setScene(TimelineScene* scene);

    qreal mapToAxis(qint64 time) const;
    qreal mapToAxisX(qint64 time) const;
    qreal axisTickWidth() const;

    void movePlayhead(qint64 time);
    qint64 currentTime() const;

protected:
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    void initUi();
    void setupSignals();

private slots:
    void onAxisRulerScaled();
    void onAxisRulerScrolled(qint64 start_time);
    void onAxisRangeChanged(qint64 min, qint64 max);

private:
    TimelineViewPrivate* d_ { nullptr };
};
} // namespace tl