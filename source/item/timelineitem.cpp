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
    palette_.setBrush(QPalette::Disabled, QPalette::Base, Qt::gray);
    palette_.setBrush(QPalette::AlternateBase, QColor("#006064"));
    palette_.setColor(QPalette::Text, Qt::white);
    palette_.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
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

void TimelineItem::setStart(qint64 frame_no)
{
    if (frame_no == start_) {
        return;
    }
    start_ = frame_no;
    setDirty(true);
    notifyPropertyChanged(StartRole | ToolTipRole);
}

void TimelineItem::setDuration(qint64 frame_count)
{
    if (frame_count == duration_) {
        return;
    }
    duration_ = frame_count;
    setDirty(true);
    notifyPropertyChanged(DurationRole | ToolTipRole);
}

void TimelineItem::setEnabled(bool enabled)
{
    if (enabled == enabled_) {
        return;
    }
    enabled_ = enabled;
    setDirty(true);
    notifyPropertyChanged(EnabledRole);
}
int TimelineItem::type() const
{
    return Type;
}

QString TimelineItem::toolTip() const
{
    return QCoreApplication::translate("TimelineItem", "Frame Start: %1\nFrame Delay: %2").arg(start_).arg(duration_);
}

bool TimelineItem::setProperty(int role, const QVariant& data)
{
    if (data.isNull()) {
        return false;
    }
    switch (role) {
    case StartRole: {
        qint64 start_time = data.value<qint64>();
        if (model_->isFrameRangeOccupied(TimelineModel::itemRow(item_id_), start_time, duration_, item_id_)) {
            TL_LOG_ERROR("This time range already occupied! start_time:{}, duration:{}", start_time, duration_);
            return false;
        }
        setStart(start_time);
    } break;
    case DurationRole: {
        qint64 duration = data.value<qint64>();
        if (model_->isFrameRangeOccupied(TimelineModel::itemRow(item_id_), start_, duration, item_id_)) {
            TL_LOG_ERROR("This time range already occupied! start_time:{}, duration:{}", start_, duration);
            return false;
        }
        setDuration(duration);
    } break;
    case EnabledRole: {
        setEnabled(data.toBool());
    } break;
    default:
        break;
    }
    return true;
}

std::optional<QVariant> TimelineItem::property(int role) const
{
    switch (role) {
    case StartRole:
        return start_;
    case DurationRole:
        return duration_;
    case NumberRole:
        return number_;
    case EnabledRole:
        return enabled_;
    default:
        break;
    }
    return std::nullopt;
}

bool TimelineItem::operate(int op_role, const QVariant& param)
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

QList<TimelineItem::PropertyElement> TimelineItem::editableProperties() const
{
    QList<TimelineItem::PropertyElement> elements;
    {
        TimelineItem::PropertyElement elmt;
        elmt.label = QCoreApplication::translate("TimelineItem", "Enabled:");
        elmt.readonly = false;
        elmt.role = EnabledRole;
        elmt.editor_type = "CheckBox";
        elements.append(elmt);
    }
    return elements;
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
    j["start"] = start_;
    j["duration"] = duration_;
    return j;
}

void TimelineItem::notifyPropertyChanged(int role)
{
    model_->notifyItemPropertyChanged(item_id_, role);
}

void from_json(const nlohmann::json& j, tl::TimelineItem& item)
{
    j["number"].get_to(item.number_);
    j["start"].get_to<qint64>(item.start_);
    j["duration"].get_to<qint64>(item.duration_);
}

} // namespace tl