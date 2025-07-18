#include "timelinefocusitem.h"
#include <QCoreApplication>

namespace tl {

TimelineFocusItem::TimelineFocusItem(ItemID item_id, TimelineModel* model)
    : TimelineItem(item_id, model)
{
    palette_.setBrush(QPalette::Normal, QPalette::Base, QColor("#1565C0"));
    palette_.setBrush(QPalette::AlternateBase, QColor("#1565C0"));
}

void TimelineFocusItem::setValue(double value)
{
    if (value_ == value) {
        return;
    }
    value_ = value;
    setDirty(true);
    notifyPropertyChanged(ValueRole);
}

double TimelineFocusItem::value() const
{
    return value_;
}

bool TimelineFocusItem::setProperty(int role, const QVariant& value)
{
    if (role == ValueRole) {
        setValue(value.toDouble());
        return true;
    }
    return TimelineItem::setProperty(role, value);
}

std::optional<QVariant> TimelineFocusItem::property(int role) const
{
    if (role == ValueRole) {
        return QVariant::fromValue(value_);
    }
    return TimelineItem::property(role);
}

QString TimelineFocusItem::toolTip() const
{
    QString content = TimelineItem::toolTip();
    content += QCoreApplication::translate("TimelineFocusItem", "\nFocus: %1").arg(value_, 0, 'f', 3);
    return content;
}

QList<TimelineItem::PropertyElement> TimelineFocusItem::editableProperties() const
{
    QList<TimelineItem::PropertyElement> elements = TimelineItem::editableProperties();
    {
        TimelineItem::PropertyElement elmt;
        elmt.label = QCoreApplication::translate("TimelineItem", "Focus:");
        elmt.readonly = false;
        elmt.role = ValueRole;
        elmt.editor_type = "DoubleSpinBox";
        elmt.editor_properties["decimals"] = 3;
        elmt.editor_properties["minimum"] = 0.0;
        elmt.editor_properties["maximum"] = 1000000.0;
        elements.emplace_back(elmt);
    }
    return elements;
}

int TimelineFocusItem::type() const
{
    return TimelineFocusItem::Type;
}

bool TimelineFocusItem::load(const nlohmann::json& j)
{
    try {
        j.get_to(*this);
        return true;
    } catch (const nlohmann::json::exception& except) {
        TL_LOG_ERROR("Failed to load {} item. Exception: {}", typeName(), except.what());
    }
    return false;
}

nlohmann::json TimelineFocusItem::save() const
{
    nlohmann::json j = TimelineItem::save();
    j["value"] = value_;
    return j;
}

const char* TimelineFocusItem::typeName() const
{
    return "Focus";
}

void from_json(const nlohmann::json& j, tl::TimelineFocusItem& item)
{
    j.get_to<TimelineItem>(static_cast<TimelineItem&>(item));
    j["value"].get_to(item.value_);
}
} // namespace tl