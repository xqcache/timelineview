#include "timelineioitem.h"
#include "timelinemodel.h"
#include "timelineutil.h"
#include <QCoreApplication>

namespace tl {
TimelineIOItem::TimelineIOItem(ItemID item_id, TimelineModel* model)
    : TimelineItem(item_id, model)
{
}

void TimelineIOItem::setData(int data)
{
    if (data_ == data) {
        return;
    }
    data_ = data;
    setDirty(true);
    notifyPropertyChanged(static_cast<int>(DataRole) | static_cast<int>(ToolTipRole));
}

int TimelineIOItem::data() const
{
    return data_;
}

bool TimelineIOItem::setProperty(int role, const QVariant& value)
{
    if (role == DataRole) {
        setData(value.toInt());
        return true;
    }
    return TimelineItem::setProperty(role, value);
}

std::optional<QVariant> TimelineIOItem::property(int role) const
{
    if (role == DataRole) {
        return data();
    }
    return TimelineItem::property(role);
}

QList<TimelineItem::PropertyElement> TimelineIOItem::editableProperties() const
{
    QList<TimelineItem::PropertyElement> elements = TimelineItem::editableProperties();
    {
        qint64 ts = std::llround(static_cast<qreal>(TimelineUtil::frameToTime(model()->frameMaximum() - start_, model()->fps())) / 100.0);
        QString secs_str = QString("%1.%2").arg(ts / 10).arg(ts % 10);
        TimelineItem::PropertyElement elmt;
        elmt.label = QCoreApplication::translate("TimelineIOItem", "Duration[%1s-%2s]:").arg(0).arg(secs_str);
        elmt.readonly = false;
        elmt.role = DurationRole;
        elmt.editor_type = "DoubleSpinBox";
        elmt.editor_properties["minimum"] = QVariant::fromValue<qreal>(0);
        elmt.editor_properties["maximum"] = QVariant::fromValue(secs_str);
        elmt.editor_properties["decimals"] = QVariant::fromValue(1);
        elmt.editor_properties["singleStep"] = QVariant::fromValue<qreal>(0.1);
        elements.emplace_back(elmt);
    }
    {
        TimelineItem::PropertyElement elmt;
        elmt.label = QCoreApplication::translate("TimelineIOItem", "Data:");
        elmt.readonly = false;
        elmt.role = DataRole;
        elmt.editor_type = "SpinBox";
        elmt.editor_properties["minimum"] = 0;
        elmt.editor_properties["maximum"] = 10000;
        elements.append(elmt);
    }
    return elements;
}

bool TimelineIOItem::load(const nlohmann::json& j)
{
    try {
        j.get_to(*this);
        return true;
    } catch (const nlohmann::json::exception& except) {
        TL_LOG_ERROR("Failed to load {} item. Exception: {}", typeName(), except.what());
    }
    return false;
}

int TimelineIOItem::type() const
{
    return Type;
}

nlohmann::json TimelineIOItem::save() const
{
    nlohmann::json j = TimelineItem::save();
    j["data"] = data_;
    return j;
}

void from_json(const nlohmann::json& j, tl::TimelineIOItem& item)
{
    j.get_to<TimelineItem>(static_cast<TimelineItem&>(item));
    j["data"].get_to(item.data_);
}

} // namespace tl
