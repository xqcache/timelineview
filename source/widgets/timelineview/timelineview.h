#pragma once

#include <QGraphicsView>

namespace tl {
class TimelineAxis;
class TimelineScene;
class TimelineModel;
struct TimelineViewPrivate;
class TimelineView : public QGraphicsView {
    Q_OBJECT

public:
    TimelineView(QWidget* parent = nullptr);
    ~TimelineView() noexcept override;

    void setAxisPlayheadHeight(int height);
    void setScene(TimelineScene* scene);
    void setSceneSize(qreal width, qreal height);
    void setSceneWidth(qreal width);

    TimelineAxis* axis() const;
    TimelineModel* model() const;

    void setFrameMode(bool on);
    qreal mapFromSceneX(qreal x) const;
    qreal mapToSceneX(qreal x) const;

    bool isInView(qreal x, qreal width) const;

protected:
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    void initUi();
    void setupSignals();

    void onFrameMaximumChanged(qint64 value);
    void onFrameMinimumChanged(qint64 value);

private:
    TimelineViewPrivate* d_ { nullptr };
};

} // namespace tl