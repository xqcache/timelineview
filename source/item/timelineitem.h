#pragma once

#include "timelinedef.h"
#include "timelinelibexport.h"
#include "timelineserializable.h"
#include <QObject>
#include <QPalette>
#include <QVariant>

namespace tl {
class TimelineModel;
class TIMELINE_LIB_EXPORT TimelineItem : public TimelineSerializable {
public:
    enum PropertyRole : int {
        NoneRole = 0,
        StartRole = 0x01,
        DurationRole = 0x02,
        NumberRole = 0x04,
        ToolTipRole = 0x08,
        EnabledRole = 0x10,
        AllRole = std::numeric_limits<int>::max()
    };

    enum OperationRole : int {
        OpIncreaseNumberRole = 0x01,
        OpDecreaseNumberRole = 0x02,
        OpUpdateAsHead = 0x04,
        OpUpdateAsTail = 0x08,
    };

    enum Type : int {
        None = 0,
        Type = 1,
        UserType = 2,
    };

    struct PropertyElement {
        QString label;
        int role;
        bool readonly;
        QString editor_type;
        std::map<QString, QVariant> editor_properties;
    };

    TimelineItem(ItemID item_id, TimelineModel* model);
    virtual ~TimelineItem() = default;

    inline TimelineModel* model() const;

    inline qint64 start() const;
    inline qint64 end() const;
    inline qint64 duration() const;
    inline qint64 destination() const;
    inline ItemID itemId() const;

    virtual void setNumber(int number);
    virtual void setStart(qint64 frame_no);
    virtual void setDuration(qint64 frame_count);

    inline bool isDirty() const;
    inline void setDirty(bool dirty);
    inline void resetDirty();
    inline int number() const;

    inline bool isEnabled() const;
    void setEnabled(bool enabled);

    const QPalette& palette() const;

    virtual int type() const;
    virtual const char* typeName() const = 0;
    virtual QString toolTip() const;

    virtual bool setProperty(int role, const QVariant& data);
    virtual std::optional<QVariant> property(int role) const;

    virtual bool operate(int op_role, const QVariant& param);

    virtual QList<PropertyElement> editableProperties() const;

public:
    bool load(const nlohmann::json& j) override;
    nlohmann::json save() const override;

protected:
    inline constexpr static PropertyRole userRole(qint64 index);

    void notifyPropertyChanged(int role);

protected:
    friend void from_json(const nlohmann::json& j, TimelineItem& item);
    QPalette palette_;
    // 数据部分
    int number_ { 0 };
    // 起始帧
    qint64 start_ { 0 };
    // 持续帧数
    qint64 duration_ { 0 };

    bool enabled_ { true };

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

inline qint64 TimelineItem::start() const
{
    return start_;
}

inline qint64 TimelineItem::end() const
{
    return start_ + duration_;
}

inline qint64 TimelineItem::duration() const
{
    return duration_;
}

inline qint64 TimelineItem::destination() const
{
    return start_ + duration_;
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

inline bool TimelineItem::isEnabled() const
{
    return enabled_;
}

inline int TimelineItem::number() const
{
    return number_;
}

inline constexpr TimelineItem::PropertyRole TimelineItem::userRole(qint64 index)
{
    assert(index < 32 && "The role must be less than 32.");
    return static_cast<PropertyRole>(1 << (index + 6));
}

} // namespace tl
