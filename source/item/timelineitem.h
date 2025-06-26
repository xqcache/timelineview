#pragma once

#include "timelinedef.h"
#include "timelineserializable.h"
#include <QObject>
#include <QPalette>
#include <QVariant>

namespace tl {
class TimelineModel;
class TimelineItem : public TimelineSerializable {
public:
    enum PropertyRole : int {
        NoneRole = 0,
        StartTimeRole = 0x01,
        DurationRole = 0x02,
        NumberRole = 0x04,
        ToolTipRole = 0x08,
        AllRole = std::numeric_limits<int>::max()
    };

    enum class OperationRole : int {
        OpIncreaseNumberRole = 0x01,
        OpDecreaseNumberRole = 0x02,
        OpUpdateAsHead = 0x04,
        OpUpdateAsTail = 0x08,
    };

    enum Type : int {
        Type = 1,
        UserType = 2,
    };

    explicit TimelineItem(ItemID item_id, TimelineModel* model);

    inline TimelineModel* model() const;

    inline qint64 startTime() const;
    inline qint64 endTime() const;
    inline qint64 duration() const;
    inline ItemID itemId() const;

    virtual void setNumber(int number);
    virtual void setStartTime(qint64 ms);
    virtual void setDuration(qint64 ms);

    inline bool isDirty() const;
    inline void setDirty(bool dirty);
    inline void resetDirty();
    inline int number() const;

    const QPalette& palette() const;

    virtual int type() const;
    virtual QString toolTip() const;

    virtual bool setProperty(PropertyRole role, const QVariant& data);
    virtual QVariant property(PropertyRole role) const;

    virtual bool operate(OperationRole op_role, const QVariant& param);

public:
    bool load(const nlohmann::json& j) override;
    nlohmann::json save() override;

protected:
    inline constexpr static PropertyRole userRole(qint64 index);

    void notifyPropertyChanged(PropertyRole role);

protected:
    friend void to_json(nlohmann::json& j, const TimelineItem& item);
    friend void from_json(const nlohmann::json& j, TimelineItem& item);
    QPalette palette_;
    // 数据部分
    int number_ { 0 };
    qint64 start_time_ { 0 };
    qint64 duration_ { 0 };

private:
    Q_DISABLE_COPY(TimelineItem)
    ItemID item_id_ { kInvalidItemID };
    TimelineModel* model_ { nullptr };
    bool dirty_ { false };
};

inline TimelineModel* TimelineItem::model() const
{
    return model_;
}

inline qint64 TimelineItem::startTime() const
{
    return start_time_;
}

inline qint64 TimelineItem::endTime() const
{
    return start_time_ + duration_;
}

inline qint64 TimelineItem::duration() const
{
    return duration_;
}

inline ItemID TimelineItem::itemId() const
{
    return item_id_;
}

inline bool TimelineItem::isDirty() const
{
    return dirty_;
}

inline void TimelineItem::setDirty(bool dirty)
{
    dirty_ = true;
}

inline void TimelineItem::resetDirty()
{
    dirty_ = false;
}

inline int TimelineItem::number() const
{
    return number_;
}

inline constexpr TimelineItem::PropertyRole TimelineItem::userRole(qint64 index)
{
    assert(index < 32 && "The role must be less than 32.");
    return static_cast<PropertyRole>(1 << (index + 5));
}

} // namespace tl
