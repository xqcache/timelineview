#include "timelineitem.h"
#include "timelinemodel.h"
#include <QCoreApplication>
#include <format>

namespace tl {

TimelineItem::TimelineItem(ItemID item_id, TimelineModel* model)
    : model_(model)
    , item_id_(item_id)
{
}

void TimelineItem::setStartTime(qint64 ms)
{
    if (ms == start_time_) {
        return;
    }
    start_time_ = ms;
    setDirty(true);
}

void TimelineItem::setDuration(qint64 ms)
{
    if (ms == duration_) {
        return;
    }
    duration_ = ms;
    setDirty(true);
}

int TimelineItem::type() const
{
    return Type;
}

QString TimelineItem::toolTip() const
{
    return QCoreApplication::translate("TimelineItem", "Start Time: %1\nDuration: %2").arg(start_time_).arg(duration_);
}

bool TimelineItem::setProperty(PropertyRole role, const QVariant& data)
{
    if (data.isNull()) {
        return false;
    }
    switch (role) {
    case StartTimeRole: {
        qint64 start_time = data.value<qint64>();
        if (model_->isTimeRangeOccupied(TimelineModel::itemRow(item_id_), start_time, start_time + duration_)) {
            TL_LOG_ERROR(std::format("This time range already occupied! start_time:{}, duration:{}", start_time, duration_));
            return false;
        }
        setStartTime(start_time);
    } break;
    case DurationRole: {
        qint64 duration = data.value<qint64>();
        if (model_->isTimeRangeOccupied(TimelineModel::itemRow(item_id_), start_time_, start_time_ + duration)) {
            TL_LOG_ERROR(std::format("This time range already occupied! start_time:{}, duration:{}", start_time_, duration));
            return false;
        }
        setDuration(duration);
    } break;
    default:
        break;
    }
    return true;
}

QVariant TimelineItem::property(PropertyRole role) const
{
    return {};
}

bool TimelineItem::load(const nlohmann::json& j)
{
    try {
        j.get_to(*this);
        return true;
    } catch (const nlohmann::json::exception& except) {
        TL_LOG_ERROR(std::format("Failed to load item. Exception: {}", except.what()));
    }
    return false;
}

nlohmann::json TimelineItem::save()
{
    return *this;
}

void from_json(const nlohmann::json& j, tl::TimelineItem& item)
{
    j["start_time"].get_to<qint64>(item.start_time_);
    j["duration"].get_to<qint64>(item.duration_);
}

void to_json(nlohmann::json& j, const TimelineItem& item)
{
    j["start_time"] = item.start_time_;
    j["duration"] = item.duration_;
}

} // namespace tl