#pragma once

#include "timelinedef.h"
#include "timelinelibexport.h"
#include <QObject>

namespace tl {
class TimelineItem;
class TimelineItemFactory;

struct TimelineModelPrivate;

class TIMELINE_LIB_EXPORT TimelineModel : public QObject {
    Q_OBJECT
public:
    explicit TimelineModel(QObject* parent = nullptr);
    ~TimelineModel() noexcept override;

    TimelineItem* item(ItemID item_id) const;
    inline constexpr static int itemRow(ItemID item_id);
    inline constexpr static int itemType(ItemID item_id);
    inline constexpr static ItemID makeItemID(int item_type, int item_row, ItemID id_index);

    bool isTimeRangeOccupied(int row, qint64 start_time, qint64 end_time) const;

    ItemID createItem(int item_type, int item_row, qint64 start_time, qint64 duration);

    TimelineItemFactory* itemFactory() const;

signals:
    void itemCreated(ItemID item_id);
    void itemRemoved(ItemID item_id);
    void itemChanged(ItemID item_id, int role);

private:
    ItemID nextItemID() const;

private:
    TimelineModelPrivate* d_ { nullptr };
};

inline constexpr int TimelineModel::itemRow(ItemID item_id)
{
    return (item_id >> 48) & 0xFF;
}

inline constexpr int TimelineModel::itemType(ItemID item_id)
{
    return (item_id >> 56) & 0x7F;
}

inline constexpr ItemID TimelineModel::makeItemID(int item_type, int item_row, ItemID id_index)
{
    return ((static_cast<ItemID>(item_type) & 0x7F) << 56) | ((static_cast<ItemID>(item_type) & 0xFF) << 48) | ((id_index << 16) >> 16);
}

} // namespace tl