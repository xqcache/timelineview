#pragma once

#include "timelinedef.h"
#include "timelinelibexport.h"
#include "timelineserializable.h"
#include <QObject>
#include <QVariant>

namespace tl {

class TimelineItem;
class TimelineItemFactory;

struct TimelineModelPrivate;

class TIMELINE_LIB_EXPORT TimelineModel : public QObject, public TimelineSerializable {
    Q_OBJECT
public:
    explicit TimelineModel(QObject* parent = nullptr);
    ~TimelineModel() noexcept override;

    template <typename T>
        requires std::is_base_of_v<TimelineItem, T>
    T* item(ItemID item_id) const
    {
        return static_cast<T*>(item(item_id));
    }

    TimelineItem* item(ItemID item_id) const;
    TimelineItem* itemByStart(int row, qint64 start) const;
    bool exists(ItemID item_id) const;
    inline constexpr static int itemRow(ItemID item_id);
    inline constexpr static int itemType(ItemID item_id);
    inline constexpr static ItemID makeItemID(int item_type, int item_row, ItemID id_index);

    bool isFrameRangeOccupied(int row, qint64 start, qint64 duration, ItemID except_item = kInvalidItemID) const;

    void removeItem(ItemID item_it);
    ItemID createItem(int item_type, int item_row, qint64 start, qint64 duration = 0, bool with_connection = false);
    ItemConnID createFrameConnection(ItemID from, ItemID to);
    ItemConnID previousConnection(ItemID item_id) const;
    ItemConnID nextConnection(ItemID item_id) const;
    void removeFrameNextConn(ItemID item_id);
    void removeFramePrevConn(ItemID item_id);
    void removeFrameConn(ItemID item_id);

    bool setItemProperty(ItemID item_id, int role, const QVariant& data);
    std::optional<QVariant> itemProperty(ItemID item_id, int role) const;
    bool requestItemOperate(ItemID item_id, int op_role, const QVariant& param = QVariant());

    TimelineItemFactory* itemFactory() const;

    void setRowHidden(int row, bool hidden);
    bool isRowHidden(int row) const;
    bool isItemHidden(ItemID item_id) const;

    void setRowCount(int row_count);
    int rowCount() const;

    void setFrameMaximum(qint64 maximum);
    void setFrameMinimum(qint64 minimum);
    qint64 frameMinimum() const;
    qint64 frameMaximum() const;
    void setViewFrameMaximum(qint64 maximum);
    void setViewFrameMinimum(qint64 minimum);
    qint64 viewFrameMinimum() const;
    qint64 viewFrameMaximum() const;

    void setFps(double fps);
    double fps() const;

    virtual void clear();
    bool isDirty() const;
    void setDirty(bool dirty = true);
    void resetDirty();

    void setItemHeight(qreal height);
    qreal itemHeight() const;
    qreal itemY(ItemID item_id) const;

    bool isFrameInRange(qint64 start, qint64 duration = 0) const;
    bool isItemInViewRange(ItemID item_id) const;

    ItemID headItem(int row) const;
    ItemID tailItem(int row) const;
    ItemID previousItem(ItemID item_id) const;
    ItemID nextItem(ItemID item_id) const;

    void notifyItemPropertyChanged(ItemID item_id, int role);
    void notifyItemOperateFinished(ItemID item_id, int op_role, const QVariant& param = QVariant());

    bool load(const nlohmann::json& j) override;
    nlohmann::json save() const override;

    qint64 frameToTime(qint64 frame_no) const;

signals:
    void itemCreated(ItemID item_id);
    void itemAboutToBeRemoved(ItemID item_id);
    void itemRemoved(ItemID item_id);
    void itemChanged(ItemID item_id, int role);
    void itemOperateFinished(ItemID item_id, int op_role, const QVariant& param = QVariant());

    void itemConnCreated(const ItemConnID& conn_id);
    void itemConnRemoved(const ItemConnID& conn_id);

    void rowCountChanged(int row_count);
    void requestUpdateItemY(ItemID item_id);

    void frameMaximumChanged(qint64 maximum);
    void frameMinimumChanged(qint64 minimum);
    void viewFrameMaximumChanged(qint64 maximum);
    void viewFrameMinimumChanged(qint64 minimum);
    void fpsChanged(double fps);

protected:
    friend void from_json(const nlohmann::json& j, TimelineModel& item);

private:
    ItemID nextItemID() const;

private:
    TimelineModelPrivate* d_ { nullptr };
};

inline constexpr int TimelineModel::itemRow(ItemID item_id)
{
    return (item_id >> 48) & 0xFF;
}

inline constexpr int TimelineModel::itemType(ItemID item_id)
{
    return (item_id >> 56) & 0x7F;
}

inline constexpr ItemID TimelineModel::makeItemID(int item_type, int item_row, ItemID id_index)
{
    return ((static_cast<ItemID>(item_type) & 0x7F) << 56) | ((static_cast<ItemID>(item_row) & 0xFF) << 48) | ((id_index << 16) >> 16);
}

} // namespace tl