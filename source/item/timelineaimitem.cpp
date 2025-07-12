#include "timelineaimitem.h"
#include "timelinemodel.h"
#include <QCoreApplication>

namespace nlohmann {
void from_json(const nlohmann::json& j, QVector3D& pos)
{
    pos.setX(j[0].get<double>());
    pos.setY(j[1].get<double>());
    pos.setZ(j[2].get<double>());
}

void to_json(nlohmann::json& j, const QVector3D& pos)
{
    j = { pos.x(), pos.y(), pos.z() };
}
} // namespace nlohmann

namespace tl {

TimelineAimItem::TimelineAimItem(ItemID item_id, TimelineModel* model)
    : TimelineItem(item_id, model)
{
    palette_.setBrush(QPalette::Normal, QPalette::Base, QColor("#6200ee"));
    palette_.setBrush(QPalette::AlternateBase, QColor("#006064"));
}

const char* TimelineAimItem::typeName() const
{
    return "Aim";
}

void TimelineAimItem::setDistance(double distance)
{
    if (qFuzzyCompare(distance_, distance)) {
        return;
    }
    distance_ = distance;
    setDirty(true);
    notifyPropertyChanged(DistanceRole);
    updateBuddyProperty(DistanceRole, distance_);
}

void TimelineAimItem::setPosition(const QVector3D& pos)
{
    if (qFuzzyCompare(position_.x(), pos.x()) && qFuzzyCompare(position_.y(), pos.y()) && qFuzzyCompare(position_.z(), pos.z())) {
        return;
    }
    position_ = pos;
    setDirty(true);
    notifyPropertyChanged(PositionRole);
    updateBuddyProperty(PositionRole, position_);
}

void TimelineAimItem::setPosition(float x, float y, float z)
{
    setPosition(QVector3D(x, y, z));
}

void TimelineAimItem::setX(float x)
{
    if (qFuzzyCompare(position_.x(), x)) {
        return;
    }
    position_.setX(x);
    setDirty(true);
    notifyPropertyChanged(XRole | PositionRole);
    updateBuddyProperty(XRole, position_);
}

void TimelineAimItem::setY(float y)
{
    if (qFuzzyCompare(position_.y(), y)) {
        return;
    }
    position_.setY(y);
    setDirty(true);
    notifyPropertyChanged(YRole | PositionRole);
    updateBuddyProperty(YRole, position_);
}

void TimelineAimItem::setZ(float z)
{
    if (qFuzzyCompare(position_.z(), z)) {
        return;
    }
    position_.setZ(z);
    setDirty(true);
    notifyPropertyChanged(ZRole | PositionRole);
    updateBuddyProperty(ZRole, position_);
}

bool TimelineAimItem::setProperty(int role, const QVariant& value)
{
    if (value.isNull()) {
        return false;
    }

    switch (role) {
    case PositionRole:
        setPosition(value.value<QVector3D>());
        return true;
    case XRole:
        setX(value.toDouble());
        return true;
    case YRole:
        setY(value.toDouble());
        return true;
    case ZRole:
        setZ(value.toDouble());
        return true;
    case DistanceRole:
        setDistance(value.toDouble());
        return true;
    }
    return TimelineItem::setProperty(role, value);
}

std::optional<QVariant> TimelineAimItem::property(int role) const
{
    switch (role) {
    case PositionRole:
        return position_;
    case XRole:
        return static_cast<double>(position_.x());
    case YRole:
        return static_cast<double>(position_.y());
    case ZRole:
        return static_cast<double>(position_.z());
    case DistanceRole:
        return distance_;
    }
    return TimelineItem::property(role);
}

QString TimelineAimItem::toolTip() const
{
    QString content = TimelineItem::toolTip();
    content += QCoreApplication::translate("TimelineAimItem", "\nPosition: %1 %2 %3")
                   .arg(position_.x(), 0, 'f', 3)
                   .arg(position_.y(), 0, 'f', 3)
                   .arg(position_.z(), 0, 'f', 3);
    return content;
}

QList<TimelineItem::PropertyElement> TimelineAimItem::editableProperties() const
{
    QList<TimelineItem::PropertyElement> elements = TimelineItem::editableProperties();

    {
        TimelineItem::PropertyElement elmt;
        elmt.label = QCoreApplication::translate("TimelineItem", "Distance:");
        elmt.readonly = false;
        elmt.role = DistanceRole;
        elmt.editor_type = "DoubleSpinBox";
        elmt.editor_properties["decimals"] = 3;
        elmt.editor_properties["minimum"] = -1000000000.0;
        elmt.editor_properties["maximum"] = 1000000000.0;
        elmt.editor_properties["suffix"] = " mm";
        elmt.buddy_value_qproperty_names[XRole] = "value";
        elmt.buddy_value_qproperty_names[YRole] = "value";
        elmt.buddy_value_qproperty_names[ZRole] = "value";
        elements.emplace_back(elmt);
    }

    {
        TimelineItem::PropertyElement elmt;
        elmt.label = QCoreApplication::translate("TimelineItem", "X:");
        elmt.readonly = false;
        elmt.role = XRole;
        elmt.editor_type = "DoubleSpinBox";
        elmt.editor_properties["decimals"] = 3;
        elmt.editor_properties["minimum"] = -1000000000.0;
        elmt.editor_properties["maximum"] = 1000000000.0;
        elmt.editor_properties["suffix"] = " mm";
        elements.emplace_back(elmt);
    }

    {
        TimelineItem::PropertyElement elmt;
        elmt.label = QCoreApplication::translate("TimelineItem", "Y:");
        elmt.readonly = false;
        elmt.role = YRole;
        elmt.editor_type = "DoubleSpinBox";
        elmt.editor_properties["decimals"] = 3;
        elmt.editor_properties["minimum"] = -1000000000.0;
        elmt.editor_properties["maximum"] = 1000000000.0;
        elmt.editor_properties["suffix"] = " mm";
        elements.emplace_back(elmt);
    }

    {
        TimelineItem::PropertyElement elmt;
        elmt.label = QCoreApplication::translate("TimelineItem", "Z:");
        elmt.readonly = false;
        elmt.role = ZRole;
        elmt.editor_type = "DoubleSpinBox";
        elmt.editor_properties["decimals"] = 3;
        elmt.editor_properties["minimum"] = -1000000000.0;
        elmt.editor_properties["maximum"] = 1000000000.0;
        elmt.editor_properties["suffix"] = " mm";
        elements.emplace_back(elmt);
    }

    return elements;
}

void from_json(const nlohmann::json& j, tl::TimelineAimItem& item)
{
    j.get_to<TimelineItem>(static_cast<TimelineItem&>(item));
    j["position"].get_to(item.position_);
    j["distance"].get_to(item.distance_);
}
int TimelineAimItem::type() const
{
    return TimelineAimItem::Type;
}

bool TimelineAimItem::load(const nlohmann::json& j)
{
    try {
        j.get_to(*this);
        return true;
    } catch (const nlohmann::json::exception& except) {
        TL_LOG_ERROR("Failed to load {} item. Exception: {}", typeName(), except.what());
    }
    return false;
}

nlohmann::json TimelineAimItem::save() const
{
    nlohmann::json j = TimelineItem::save();
    j["position"] = position_;
    j["distance"] = distance_;
    return j;
}

} // namespace tl