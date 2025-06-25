#pragma once

#include "timelinedef.h"
#include "timelinelibexport.h"
#include <QGraphicsScene>

namespace tl {
class TimelineModel;
struct TimelineScenePrivate;
class TIMELINE_LIB_EXPORT TimelineScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit TimelineScene(QObject* parent = nullptr);
    ~TimelineScene() noexcept override;

    void setModel(TimelineModel* model);
    TimelineModel* model() const;

private:
    void onItemCreated(ItemID item_id);

private:
    TimelineScenePrivate* d_ { nullptr };
};
} // namespace tl