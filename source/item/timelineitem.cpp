#include "timelineitem.h"
#include "timelinemodel.h"
#include <QCoreApplication>
#include <format>

namespace tl {

TimelineItem::TimelineItem(ItemID item_id, TimelineModel* model)
    : model_(model)
    , item_id_(item_id)
{
    palette_.setBrush(QPalette::Base, QColor("#006064"));
    palette_.setBrush(QPalette::AlternateBase, QColor("#006064"));
    palette_.setColor(QPalette::Text, QColor("#006064"));
}

void TimelineItem::setNumber(int number)
{
    if (number == number_) {
        return;
    }
    number_ = number;
    setDirty(true);
    notifyPropertyChanged(NumberRole);
}

void TimelineItem::setStartTime(qint64 ms)
{
    if (ms == start_time_) {
        return;
    }
    start_time_ = ms;
    setDirty(true);
    notifyPropertyChanged(StartTimeRole);
}

void TimelineItem::setDuration(qint64 ms)
{
    if (ms == duration_) {
        return;
    }
    duration_ = ms;
    setDirty(true);
    notifyPropertyChanged(DurationRole);
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
            TL_LOG_ERROR("This time range already occupied! start_time:{}, duration:{}", start_time, duration_);
            return false;
        }
        setStartTime(start_time);
    } break;
    case DurationRole: {
        qint64 duration = data.value<qint64>();
        if (model_->isTimeRangeOccupied(TimelineModel::itemRow(item_id_), start_time_, start_time_ + duration)) {
            TL_LOG_ERROR("This time range already occupied! start_time:{}, duration:{}", start_time_, duration);
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
    switch (role) {
    case StartTimeRole:
        return start_time_;
    case DurationRole:
        return duration_;
    case NumberRole:
        return number_;
    default:
        break;
    }
    return {};
}

bool TimelineItem::operate(OperationRole op_role, const QVariant& param)
{
    switch (op_role) {
    case OperationRole::OpIncreaseNumberRole:
        setNumber(number_ + param.toInt());
        return true;
    case OperationRole::OpDecreaseNumberRole:
        setNumber(number_ - param.toInt());
        return true;
    case OperationRole::OpUpdateAsHead:
    case OperationRole::OpUpdateAsTail:
        model_->notifyItemOperateFinished(item_id_, op_role);
        return true;
    default:
        break;
    }
    return false;
}

const QPalette& TimelineItem::palette() const
{
    return palette_;
}

bool TimelineItem::load(const nlohmann::json& j)
{
    try {
        j.get_to(*this);
        return true;
    } catch (const nlohmann::json::exception& except) {
        TL_LOG_ERROR("Failed to load item. Exception: {}", except.what());
    }
    return false;
}

nlohmann::json TimelineItem::save() const
{
    nlohmann::json j;
    j["number"] = number_;
    j["start_time"] = start_time_;
    j["duration"] = duration_;
    return j;
}

void TimelineItem::notifyPropertyChanged(PropertyRole role)
{
    model_->notifyItemPropertyChanged(item_id_, role);
}

void from_json(const nlohmann::json& j, tl::TimelineItem& item)
{
    j["number"].get_to(item.number_);
    j["start_time"].get_to<qint64>(item.start_time_);
    j["duration"].get_to<qint64>(item.duration_);
}

} // namespace tl