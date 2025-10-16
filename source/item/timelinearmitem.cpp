#include "timelinearmitem.h"
#include "timelinemodel.h"
#include <QCoreApplication>

namespace tl {

void TimelineArmItem::setAngles(const QList<double>& angles)
{
    setAngles(std::vector<double>(angles.begin(), angles.end()));
}

void TimelineArmItem::setAngles(const std::vector<double>& angles)
{
    if (angles == angles_) {
        return;
    }

    angles_ = angles;
    setDirty(true);
    notifyPropertyChanged(static_cast<int>(JointAnglesRole) | static_cast<int>(ToolTipRole));
}

void TimelineArmItem::setTrackingAim(bool tracking, int flag)
{
    if (tracking == tracking_) {
        return;
    }
    tracking_ = tracking;
    setDirty(true);
    notifyPropertyChanged(static_cast<int>(TrackingAimRole) | static_cast<int>(ToolTipRole), flag);
}

void from_json(const nlohmann::json& j, tl::TimelineArmItem& item)
{
    j.get_to<TimelineItem>(static_cast<TimelineItem&>(item));
    j["angles"].get_to(item.angles_);
    j["tracking"].get_to(item.tracking_);
    j["enabled"].get_to(item.enabled_);
}

int TimelineArmItem::type() const
{
    return TimelineArmItem::Type;
}

bool TimelineArmItem::load(const nlohmann::json& j)
{
    try {
        j.get_to(*this);
        return true;
    } catch (const nlohmann::json::exception& except) {
        TL_LOG_ERROR("Failed to load {} item. Exception: {}", typeName(), except.what());
    }
    return false;
}

nlohmann::json TimelineArmItem::save() const
{
    nlohmann::json j = TimelineItem::save();
    j["angles"] = angles_;
    j["tracking"] = tracking_;
    j["enabled"] = enabled_;
    return j;
}

const char* TimelineArmItem::typeName() const
{
    return "Arm";
}

QString TimelineArmItem::toolTip() const
{
    QString content = TimelineItem::toolTip();
    QStringList joint_angles_lst;
    std::transform(
        angles_.begin(), angles_.end(), std::back_inserter(joint_angles_lst), [](double v) { return QString::number(qRadiansToDegrees(v), 'f', 3); });
    content += QCoreApplication::translate("TimelineArmItem", "\nFrame Delay: %1").arg(duration_);
    content += QCoreApplication::translate("TimelineArmItem", "\nAngles: %1").arg(joint_angles_lst.join(", "));
    content += QCoreApplication::translate("TimelineArmItem", "\nTracking Target: %1")
                   .arg(tracking_ ? QCoreApplication::translate("TimelineArmItem", "Yes") : QCoreApplication::translate("TimelineArmItem", "No"));
    return content;
}

QList<TimelineItem::PropertyElement> TimelineArmItem::editableProperties() const
{
    QList<TimelineItem::PropertyElement> elements = TimelineItem::editableProperties();

    {
        TimelineItem::PropertyElement elmt;
        elmt.label = QCoreApplication::translate("TimelineArmItem", "Tracking Target:");
        elmt.readonly = false;
        elmt.role = TrackingAimRole;
        elmt.editor_type = "CheckBox";
        elements.emplace_back(elmt);
    }
    {
        TimelineItem::PropertyElement elmt;
        elmt.label = QCoreApplication::translate("TimelineItem", "Delay[%1-%2]:").arg(model()->frameMinimum()).arg(model()->frameMaximum() - start_);
        elmt.readonly = false;
        elmt.role = DurationRole;
        elmt.editor_type = "SpinBox";
        elmt.editor_properties["minimum"] = QVariant::fromValue<int>(0);
        elmt.editor_properties["maximum"] = QVariant::fromValue<int>(model()->frameMaximum() - start_);
        elements.emplace_back(elmt);
    }

    return elements;
}

bool TimelineArmItem::setProperty(int role, const QVariant& data)
{
    if (data.isNull()) {
        return false;
    }

    switch (role) {
    case TrackingAimRole:
        setTrackingAim(data.toBool());
        return true;
    case JointAnglesRole:
        setAngles(data.value<std::vector<double>>());
        return true;
    default:
        break;
    }
    return TimelineItem::setProperty(role, data);
}

std::optional<QVariant> TimelineArmItem::property(int role) const
{
    switch (role) {
    case TrackingAimRole:
        return tracking_;
    case JointAnglesRole:
        return QVariant::fromValue(angles_);
    }
    return TimelineItem::property(role);
}

} // namespace tl