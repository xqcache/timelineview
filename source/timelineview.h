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

protected:
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    TimelineViewPrivate* d_ { nullptr };
};
} // namespace tl