#include "timelinemodel.h"
#include "item/timelineitem.h"
#include "timelineitemfactory.h"

namespace tl {

struct TimelineModelPrivate {
    ItemID id_index { 1 };
    std::map<int, std::map<qint64, ItemID>> item_table;
    std::map<ItemID, std::unique_ptr<TimelineItem>> items;
    std::unique_ptr<TimelineItemFactory> item_factory;
};

TimelineModel::TimelineModel(QObject* parent)
    : QObject(parent)
    , d_(new TimelineModelPrivate)
{
    d_->item_factory = std::make_unique<TimelineItemFactory>();
}

TimelineModel::~TimelineModel() noexcept
{
    delete d_;
}

bool TimelineModel::isTimeRangeOccupied(int row, qint64 start_time, qint64 end_time) const
{
    // auto row_it = d_->item_table.find(row);
    // if (row_it == d_->item_table.end()) {
    //     return false;
    // }

    // auto time_it = row_it->second.lower_bound(start_time);
    // if (time_it == row_it->second.end()) {
    //     return false;
    // }

    // auto next_it = std::next(time_it);

    // if (time_it->first == start_time) { }

    return false;
}

ItemID TimelineModel::createItem(int item_type, int item_row, qint64 start_time, qint64 duration)
{
    ItemID item_id = makeItemID(item_type, item_row, d_->id_index);
    auto item = d_->item_factory->createItem(item_id, this);
    if (!item) {
        return kInvalidItemID;
    }
    d_->id_index++;
    d_->items[item_id] = std::move(item);
    d_->item_table[item_row][start_time] = item_id;

    return item_id;
}

TimelineItemFactory* TimelineModel::itemFactory() const
{
    return d_->item_factory.get();
}
} // namespace tl