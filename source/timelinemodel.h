#pragma once

#include "timelinedef.h"
#include "timelinelibexport.h"
#include "timelineserializable.h"
#include <QObject>
#include <QVariant>

namespace tl {

class TimelineItem;
class TimelineItemFactory;
class TimelineItemCreateCommand;
class TimelineItemDeleteCommand;
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
    tl::ItemID itemIdByStart(int row, qint64 start) const;
    bool exists(ItemID item_id) const;
    inline constexpr static int itemRow(ItemID item_id);
    inline constexpr static int itemType(ItemID item_id);
    inline constexpr static ItemID makeItemID(int item_type, int item_row, ItemID id_index);

    bool isFrameRangeOccupied(int row, qint64 start, qint64 duration, ItemID except_item = kInvalidItemID) const;

    void removeItem(ItemID item_id);
    ItemID createItem(int item_type, int item_row, qint64 start, qint64 duration = 0, bool with_connection = false);
    ItemConnID createFrameConnection(ItemID from, ItemID to);
    ItemConnID previousConnection(ItemID item_id) const;
    ItemConnID nextConnection(ItemID item_id) const;
    bool hasConnection(ItemID item_id) const;
    void removeFrameNextConn(ItemID item_id);
    void removeFramePrevConn(ItemID item_id);
    void removeFrameConn(ItemID item_id);

    bool setItemProperty(ItemID item_id, int role, const QVariant& data);
    std::optional<QVariant> itemProperty(ItemID item_id, int role) const;
    bool requestItemOperate(ItemID item_id, int op_role, const QVariant& param = QVariant());
    void notifyLanguageChanged();

    TimelineItemFactory* itemFactory() const;

    void setTypeHidden(int row, int type, bool hidden);
    bool isTypeHidden(int type) const;
    void setTypeLocked(int type, bool locked);
    bool isTypeLocked(int type) const;
    void setTypeDisabled(int type, bool disabled);
    bool isTypeDisabled(int type) const;

    bool isItemHidden(ItemID item_id) const;
    bool isItemLocked(ItemID item_id) const;
    bool isItemDisabled(ItemID item_id) const;

    void setRowCount(int row_count);
    int rowCount() const;
    int rowItemCount(int row) const;

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

    bool modifyItemStart(ItemID item_id, qint64 start, bool clamp_to_range = true);

    ItemID headItem(int row) const;
    ItemID tailItem(int row) const;
    ItemID previousItem(ItemID item_id) const;
    ItemID nextItem(ItemID item_id) const;
    std::map<qint64, ItemID> rowItems(int row) const;

    void notifyItemPropertyChanged(ItemID item_id, int role, const QVariant& old_val = QVariant());
    void notifyItemOperateFinished(ItemID item_id, int op_role, const QVariant& param = QVariant());

    bool load(const nlohmann::json& j) override;
    nlohmann::json save() const override;

    qint64 frameToTime(qint64 frame_no) const;

    QString copyItem(ItemID item_id) const;
    ItemID pasteItem(const QString& data, qint64 frame_no);

signals:
    void itemAboutToCreated(TimelineItem* item);
    void itemCreated(ItemID item_id);
    void itemAboutToBeRemoved(ItemID item_id);
    void itemRemoved(ItemID item_id);
    void itemChanged(ItemID item_id, int role, const QVariant& old_val = QVariant());
    void itemOperateFinished(ItemID item_id, int op_role, const QVariant& param = QVariant());

    void itemConnCreated(const ItemConnID& conn_id);
    void itemConnRemoved(const ItemConnID& conn_id);

    void requestRefreshItemViewCache(ItemID item_id);
    void requestRebuildItemCache(ItemID item_id);

    void rowCountChanged(int row_count);
    void requestUpdateItemY(ItemID item_id);

    void frameMaximumChanged(qint64 maximum);
    void frameMinimumChanged(qint64 minimum);
    void viewFrameMaximumChanged(qint64 maximum);
    void viewFrameMinimumChanged(qint64 minimum);
    void fpsChanged(double fps);

    void errorOccurred(const QString& error);

protected:
    friend void from_json(const nlohmann::json& j, TimelineModel& item);

private:
    ItemID nextItemID() const;

    friend class TimelineItemCreateCommand;
    friend class TimelineItemDeleteCommand;
    virtual void loadItem(const nlohmann::json& j, const std::optional<ItemID>& item_id_opt = std::nullopt, const std::optional<qint64>& start = std::nullopt);
    virtual nlohmann::json saveItem(ItemID item_id) const;

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