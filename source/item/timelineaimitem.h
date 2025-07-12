#pragma once

#include "timelineitem.h"
#include "timelinelibexport.h"
#include <QVector3D>

namespace tl {
class TIMELINE_LIB_EXPORT TimelineAimItem : public TimelineItem {
public:
    enum PropertyRole {
        PositionRole = userRole(0),
        XRole = userRole(1),
        YRole = userRole(2),
        ZRole = userRole(3),
        DistanceRole = userRole(4),
    };

    enum Type {
        Type = UserType + 3
    };

    TimelineAimItem(ItemID item_id, TimelineModel* model);

    void setPosition(const QVector3D& pos);
    void setPosition(float x, float y, float z);
    void setX(float x);
    void setY(float y);
    void setZ(float z);
    void setDistance(double distance);

    inline double distance() const;
    inline QVector3D position() const;
    inline double x() const;
    inline double y() const;
    inline double z() const;

public:
    int type() const override;
    const char* typeName() const override;
    QString toolTip() const override;
    bool load(const nlohmann::json& j) override;
    nlohmann::json save() const override;
    QList<TimelineItem::PropertyElement> editableProperties() const override;
    bool setProperty(int role, const QVariant& value) override;
    std::optional<QVariant> property(int role) const override;

private:
    friend void from_json(const nlohmann::json& j, TimelineAimItem& item);

    double distance_ { 0 };
    QVector3D position_ { 0, 0, 0 };
};

inline double TimelineAimItem::distance() const
{
    return distance_;
}

inline QVector3D TimelineAimItem::position() const
{
    return position_;
}

inline double TimelineAimItem::x() const
{
    return position_.x();
}

inline double TimelineAimItem::y() const
{
    return position_.y();
}

inline double TimelineAimItem::z() const
{
    return position_.z();
}

} // namespace tl