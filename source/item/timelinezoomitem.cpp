#include "timelinezoomitem.h"
#include <QCoreApplication>

namespace tl {

TimelineZoomItem::TimelineZoomItem(ItemID item_id, TimelineModel* model)
    : TimelineItem(item_id, model)
{
    palette_.setBrush(QPalette::Normal, QPalette::Base, QColor("#AD1457"));
    palette_.setBrush(QPalette::AlternateBase, QColor("#AD1457"));
}

void TimelineZoomItem::setValue(double value)
{
    if (value_ == value) {
        return;
    }
    value_ = value;
    setDirty(true);
    notifyPropertyChanged(static_cast<int>(ValueRole) | static_cast<int>(ToolTipRole));
}

double TimelineZoomItem::value() const
{
    return value_;
}

bool TimelineZoomItem::setProperty(int role, const QVariant& value)
{
    if (role == ValueRole) {
        setValue(value.toDouble());
        return true;
    }
    return TimelineItem::setProperty(role, value);
}

std::optional<QVariant> TimelineZoomItem::property(int role) const
{
    if (role == ValueRole) {
        return QVariant::fromValue(value_);
    }
    return TimelineItem::property(role);
}

QString TimelineZoomItem::toolTip() const
{
    QString content = TimelineItem::toolTip();
    content += QCoreApplication::translate("TimelineFocusItem", "\nZoom: %1").arg(value_, 0, 'f', 3);
    return content;
}

QList<TimelineItem::PropertyElement> TimelineZoomItem::editableProperties() const
{
    QList<TimelineItem::PropertyElement> elements = TimelineItem::editableProperties();
    {
        TimelineItem::PropertyElement elmt;
        elmt.label = QCoreApplication::translate("TimelineItem", "Zoom:");
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

int TimelineZoomItem::type() const
{
    return TimelineZoomItem::Type;
}

bool TimelineZoomItem::load(const nlohmann::json& j)
{
    try {
        j.get_to(*this);
        return true;
    } catch (const nlohmann::json::exception& except) {
        TL_LOG_ERROR("Failed to load {} item. Exception: {}", typeName(), except.what());
    }
    return false;
}

nlohmann::json TimelineZoomItem::save() const
{
    nlohmann::json j = TimelineItem::save();
    j["value"] = value_;
    return j;
}

const char* TimelineZoomItem::typeName() const
{
    return "Zoom";
}

void from_json(const nlohmann::json& j, tl::TimelineZoomItem& item)
{
    j.get_to<TimelineItem>(static_cast<TimelineItem&>(item));
    j["value"].get_to(item.value_);
}
} // namespace tl