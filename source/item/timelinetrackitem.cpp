#include "timelinetrackitem.h"
#include <QCoreApplication>

namespace tl {

TimelineTrackItem::TimelineTrackItem(ItemID item_id, TimelineModel* model)
    : TimelineItem(item_id, model)
{
    palette_.setBrush(QPalette::Base, QColor("#b00020"));
    palette_.setBrush(QPalette::AlternateBase, QColor("#b00020"));
    palette_.setColor(QPalette::Text, QColor("#b00020"));
}

void TimelineTrackItem::setPosition(double position)
{
    if (position_ == position) {
        return;
    }
    position_ = position;
    setDirty(true);
    notifyPropertyChanged(PositionRole);
}

double TimelineTrackItem::position() const
{
    return position_;
}

bool TimelineTrackItem::setProperty(int role, const QVariant& value)
{
    if (role == PositionRole) {
        setPosition(value.toDouble());
        return true;
    }
    return TimelineItem::setProperty(role, value);
}

std::optional<QVariant> TimelineTrackItem::property(int role) const
{
    if (role == PositionRole) {
        return QVariant::fromValue(position_);
    }
    return TimelineItem::property(role);
}

QString TimelineTrackItem::toolTip() const
{
    QString content = TimelineItem::toolTip();
    content += QCoreApplication::translate("TimelineTrackItem", "\nPosition: %1").arg(position_, 0, 'f', 3);
    return content;
}

QList<TimelineItem::PropertyElement> TimelineTrackItem::editableProperties() const
{
    QList<TimelineItem::PropertyElement> elements;
    {
        TimelineItem::PropertyElement elmt;
        elmt.label = QCoreApplication::translate("TimelineItem", "Position:");
        elmt.readonly = false;
        elmt.role = PositionRole;
        elmt.editor_type = "DoubleSpinBox";
        elmt.editor_properties["decimals"] = 3;
        elmt.editor_properties["minimum"] = 0.0;
        elmt.editor_properties["maximum"] = 100.0;
        elements.append(elmt);
    }
    return elements;
}

int TimelineTrackItem::type() const
{
    return TimelineTrackItem::Type;
}

bool TimelineTrackItem::load(const nlohmann::json& j)
{
    try {
        j.get_to(*this);
        return true;
    } catch (const nlohmann::json::exception& except) {
        TL_LOG_ERROR("Failed to load {} item. Exception: {}", typeName(), except.what());
    }
    return false;
}

nlohmann::json TimelineTrackItem::save() const
{
    nlohmann::json j = TimelineItem::save();
    j["position"] = position_;
    return j;
}

const char* TimelineTrackItem::typeName() const
{
    return "Track";
}

void from_json(const nlohmann::json& j, tl::TimelineTrackItem& item)
{
    j.get_to<TimelineItem>(static_cast<TimelineItem&>(item));
    j["position"].get_to(item.position_);
}

} // namespace tl