#include "timelinemodel.h"
#include "item/timelineitem.h"
#include "timelineitemfactory.h"
#include <set>

namespace nlohmann {
void from_json(const nlohmann::json& j, tl::ItemConnID& conn_id)
{
    conn_id.from = j[0];
    conn_id.to = j[1];
}

void to_json(nlohmann::json& j, const tl::ItemConnID& conn_id)
{
    j.emplace_back(conn_id.from);
    j.emplace_back(conn_id.to);
}
} // namespace nlohmann

namespace tl {

struct TimelineModelPrivate {
    std::map<ItemID, std::unique_ptr<TimelineItem>> items;
    // {row: {start: item_id}}
    std::map<int, std::map<qint64, ItemID>> item_table;
    // {row: {item_id: start}}
    std::map<int, std::unordered_map<ItemID, qint64>> item_table_helper;
    std::set<int> hidden_rows;
    std::set<int> locked_rows;
    std::map<ItemID, ItemConnID> next_conns;
    std::map<ItemID, ItemConnID> prev_conns;
    int row_count { 1 };
    ItemID id_index { 1 };
    std::array<qint64, 2> frame_range { 0, 100 };
    std::array<qint64, 2> view_frame_range { 0, 100 };
    double fps { 24.0 };

    std::unique_ptr<TimelineItemFactory> item_factory;
    bool dirty { false };
    qreal item_height { 40 };
};

TimelineModel::TimelineModel(QObject* parent)
    : QObject(parent)
    , d_(new TimelineModelPrivate)
{
    d_->item_factory = std::make_unique<TimelineItemFactory>();
}

TimelineModel::~TimelineModel() noexcept
{
    delete d_;
}

TimelineItem* TimelineModel::item(ItemID item_id) const
{
    auto it = d_->items.find(item_id);
    if (it == d_->items.end()) {
        return nullptr;
    }
    return it->second.get();
}

TimelineItem* TimelineModel::itemByStart(int row, qint64 start) const
{
    if (row < 0 || row >= d_->row_count) {
        return nullptr;
    }
    auto it = d_->item_table[row].lower_bound(start);
    if (it == d_->item_table[row].end()) {
        return nullptr;
    }
    return item(it->second);
}

bool TimelineModel::exists(ItemID item_id) const
{
    return d_->items.contains(item_id);
}

bool TimelineModel::isTimeRangeOccupied(int row, qint64 start, qint64 end) const
{
    // auto row_it = d_->item_table.find(row);
    // if (row_it == d_->item_table.end()) {
    //     return false;
    // }

    // auto time_it = row_it->second.lower_bound(start);
    // if (time_it == row_it->second.end()) {
    //     return false;
    // }

    // auto next_it = std::next(time_it);

    // if (time_it->first == start) { }

    return false;
}

ItemID TimelineModel::createItem(int item_type, int row, qint64 start, qint64 duration, bool with_connection)
{
    if (row < 0 || row >= d_->row_count) {
        TL_LOG_ERROR("Failed to create frame item. Invalid row[{}], it must between 0 and {}", row, d_->row_count);
        return kInvalidItemID;
    }

    if (isTimeRangeOccupied(row, start, duration)) {
        TL_LOG_ERROR("The time period has been occupied.");
        return kInvalidItemID;
    }

    // 构造item_id
    ItemID item_id = makeItemID(item_type, row, d_->id_index);
    auto item = d_->item_factory->createItem(item_id, this);
    if (!item) {
        return kInvalidItemID;
    }

    // 获取插入位置的item序号，同时修改插入位置之后的item序号
    std::optional<int> number_opt;
    if (d_->item_table.contains(row)) {
        // 插入位置之后的item对应编号加一
        for (auto it = d_->item_table[row].upper_bound(start); it != d_->item_table[row].end(); ++it) {
            if (!number_opt) {
                auto number_opt = itemProperty(it->second, TimelineItem::NumberRole);
                if (number_opt.has_value()) {
                    number_opt = number_opt->value<int>();
                }
            }
            requestItemOperate(it->second, TimelineItem::OperationRole::OpIncreaseNumberRole, 1);
        }
    }

    ItemID old_head = kInvalidItemID;
    ItemID old_tail = kInvalidItemID;
    if (number_opt.has_value()) {
        if (*number_opt == 1) {
            old_head = headItem(row);
        }
    } else {
        old_tail = tailItem(row);
    }

    // 设置新item属性
    item->setNumber(number_opt.value_or(d_->item_table[row].size() + 1));
    item->setStart(start);
    item->setDuration(duration);

    // 登记item
    d_->id_index++;
    d_->items[item_id] = std::move(item);
    d_->dirty = true;
    d_->item_table[row][start] = item_id;
    d_->item_table_helper[row][item_id] = start;
    emit itemCreated(item_id);

    if (headItem(row) == item_id) {
        requestItemOperate(item_id, TimelineItem::OperationRole::OpUpdateAsHead);
    } else if (tailItem(row) == item_id) {
        requestItemOperate(item_id, TimelineItem::OperationRole::OpUpdateAsTail);
    }

    if (old_head != kInvalidItemID) {
        requestItemOperate(old_head, TimelineItem::OperationRole::OpUpdateAsHead);
    } else if (old_tail != kInvalidItemID) {
        requestItemOperate(old_tail, TimelineItem::OperationRole::OpUpdateAsTail);
    }

    if (with_connection) {
        // 增加Connection
        auto prev_item_id = previousItem(item_id);
        if (prev_item_id != kInvalidItemID) {
            removeFrameNextConn(prev_item_id);
            createFrameConnection(prev_item_id, item_id);
        }
        auto next_item_id = nextItem(item_id);
        if (next_item_id != kInvalidItemID) {
            removeFramePrevConn(next_item_id);
            createFrameConnection(item_id, next_item_id);
        }
    }

    return item_id;
}

void TimelineModel::removeItem(ItemID item_id)
{
    auto item_it = d_->items.find(item_id);
    if (item_it == d_->items.end()) {
        return;
    }
    // 从item_sorts中删除item_id
    int row = itemRow(item_id);
    if (row < 0) {
        return;
    }

    emit itemAboutToBeRemoved(item_id);

    ItemID new_head = kInvalidItemID;
    ItemID new_tail = kInvalidItemID;

    ItemID prev_item = previousItem(item_id);
    ItemID next_item = nextItem(item_id);

    if (item_id == tailItem(row)) {
        new_tail = prev_item;
    } else if (item_id == headItem(row)) {
        new_head = next_item;
    }

    // 删除旧连接
    removeFrameConn(item_id);
    // 绑定新连接
    if (prev_item != kInvalidItemID && next_item != kInvalidItemID) {
        createFrameConnection(prev_item, next_item);
    }

    if (auto helper_row_it = d_->item_table_helper.find(row); helper_row_it != d_->item_table_helper.end()) {
        if (auto item_origin_it = helper_row_it->second.find(item_id); item_origin_it != helper_row_it->second.end()) {
            // 后续受影响的item序号需要重新设置
            for (auto it = d_->item_table[row].upper_bound(item_origin_it->second); it != d_->item_table[row].end(); ++it) {
                requestItemOperate(it->second, TimelineItem::OperationRole::OpDecreaseNumberRole, 1);
            }
            if (auto row_it = d_->item_table.find(row); row_it != d_->item_table.end()) {
                if (auto origin_item_it = row_it->second.find(item_origin_it->second); origin_item_it != row_it->second.end()) {
                    row_it->second.erase(origin_item_it);
                }
            }
            helper_row_it->second.erase(item_origin_it);
        }
    }
    d_->items.erase(item_it);
    if (new_head != kInvalidItemID) {
        requestItemOperate(new_head, TimelineItem::OperationRole::OpUpdateAsHead);
    } else if (new_tail != kInvalidItemID) {
        requestItemOperate(new_head, TimelineItem::OperationRole::OpUpdateAsTail);
    }
    emit itemRemoved(item_id);
}

ItemConnID TimelineModel::createFrameConnection(ItemID from, ItemID to)
{
    if (!exists(from) || !exists(to)) {
        return {};
    }

    ItemConnID conn_id { .from = from, .to = to };
    d_->next_conns[from] = conn_id;
    d_->prev_conns[to] = conn_id;
    emit itemConnCreated(conn_id);
    return conn_id;
}

void TimelineModel::removeFrameConn(ItemID item_id)
{
    removeFramePrevConn(item_id);
    removeFrameNextConn(item_id);
}

void TimelineModel::removeFrameNextConn(ItemID item_id)
{
    auto it = d_->next_conns.find(item_id);
    if (it == d_->next_conns.end()) {
        return;
    }
    auto conn_id = it->second;
    d_->next_conns.erase(it);
    auto prev_it = d_->prev_conns.find(conn_id.to);
    if (prev_it != d_->prev_conns.end()) {
        d_->prev_conns.erase(prev_it);
    }
    emit itemConnRemoved(conn_id);
}

void TimelineModel::removeFramePrevConn(ItemID item_id)
{
    auto it = d_->prev_conns.find(item_id);
    if (it == d_->prev_conns.end()) {
        return;
    }
    auto conn_id = it->second;
    d_->prev_conns.erase(it);
    auto prev_it = d_->next_conns.find(conn_id.from);
    if (prev_it != d_->next_conns.end()) {
        d_->next_conns.erase(prev_it);
    }
    emit itemConnRemoved(conn_id);
}

bool TimelineModel::setItemProperty(ItemID item_id, int role, const QVariant& data)
{
    auto* item = this->item(item_id);
    if (!item) {
        return false;
    }
    return item->setProperty(role, data);
}

std::optional<QVariant> TimelineModel::itemProperty(ItemID item_id, int role) const
{
    auto* item = this->item(item_id);
    if (!item) {
        return std::nullopt;
    }
    return item->property(role);
}

bool TimelineModel::requestItemOperate(ItemID item_id, int op_role, const QVariant& param)
{
    auto* item = this->item(item_id);
    if (!item) {
        return {};
    }
    return item->operate(op_role, param);
}

TimelineItemFactory* TimelineModel::itemFactory() const
{
    return d_->item_factory.get();
}

void TimelineModel::setRowHidden(int row, bool hidden)
{
    if (hidden) {
        if (d_->hidden_rows.contains(row)) {
            return;
        }
        d_->hidden_rows.emplace(row);
    } else {
        if (!d_->hidden_rows.contains(row)) {
            return;
        }
        d_->hidden_rows.erase(row);
    }

    auto it = d_->item_table.find(row);
    if (it == d_->item_table.end()) {
        it = d_->item_table.upper_bound(row);
    }
    for (; it != d_->item_table.end(); ++it) {
        for (const auto& [_, item_id] : it->second) {
            emit requestUpdateItemY(item_id);
        }
    }
}

bool TimelineModel::isDirty() const
{
    return d_->dirty || std::any_of(d_->items.cbegin(), d_->items.cend(), [](const auto& pair) { return pair.second->isDirty(); });
}

void TimelineModel::setDirty(bool dirty)
{
    d_->dirty = dirty;
}

void TimelineModel::resetDirty()
{
    d_->dirty = true;
    std::for_each(d_->items.begin(), d_->items.end(), [](const auto& pair) { pair.second->resetDirty(); });
}

bool TimelineModel::isRowHidden(int row) const
{
    return d_->hidden_rows.contains(row);
}

bool TimelineModel::isItemHidden(ItemID item_id) const
{
    if (item_id == kInvalidItemID) {
        return true;
    }
    int row = itemRow(item_id);
    if (row < 0 || row >= d_->row_count) {
        return true;
    }
    return d_->hidden_rows.contains(row);
}

void TimelineModel::setRowCount(int row_count)
{
    if (row_count == d_->row_count) {
        return;
    }
    d_->row_count = row_count;
    emit rowCountChanged(row_count);
}

int TimelineModel::rowCount() const
{
    return d_->row_count;
}

qreal TimelineModel::itemHeight() const
{
    return d_->item_height;
}

qreal TimelineModel::itemY(ItemID item_id) const
{
    if (item_id == kInvalidItemID) {
        return -2 * d_->item_height;
    }
    int item_row = itemRow(item_id);
    if (item_row < 0 || item_row >= d_->row_count) {
        return -2 * d_->item_height;
    }
    if (isRowHidden(item_row)) {
        return -2 * d_->item_height;
    }

    int hidden_count = std::distance(d_->hidden_rows.begin(), d_->hidden_rows.lower_bound(item_row));
    return (item_row - hidden_count) * d_->item_height;
}

ItemID TimelineModel::headItem(int row) const
{
    auto it = d_->item_table.find(row);
    if (it == d_->item_table.end() || it->second.empty()) {
        return kInvalidItemID;
    }
    return it->second.begin()->second;
}

ItemID TimelineModel::tailItem(int row) const
{
    auto it = d_->item_table.find(row);
    if (it == d_->item_table.end() || it->second.empty()) {
        return kInvalidItemID;
    }
    return std::prev(it->second.end())->second;
}

ItemID TimelineModel::previousItem(ItemID item_id) const
{
    if (item_id == kInvalidItemID) {
        return kInvalidItemID;
    }
    int row = itemRow(item_id);
    if (row < 0) {
        return kInvalidItemID;
    }

    auto helper_row_it = d_->item_table_helper.find(row);
    if (helper_row_it == d_->item_table_helper.end()) {
        return kInvalidItemID;
    }
    auto start_it = helper_row_it->second.find(item_id);
    if (start_it == helper_row_it->second.end()) {
        return kInvalidItemID;
    }
    auto row_it = d_->item_table.find(row);
    if (row_it == d_->item_table.end()) {
        return kInvalidItemID;
    }
    auto item_it = row_it->second.find(start_it->second);
    if (item_it == row_it->second.end() || item_it == row_it->second.begin()) {
        return kInvalidItemID;
    }
    auto prev_it = std::prev(item_it);
    return prev_it->second;
}

ItemID TimelineModel::nextItem(ItemID item_id) const
{
    if (item_id == kInvalidItemID) {
        return kInvalidItemID;
    }
    int row = itemRow(item_id);
    if (row < 0) {
        return kInvalidItemID;
    }

    auto helper_row_it = d_->item_table_helper.find(row);
    if (helper_row_it == d_->item_table_helper.end()) {
        return kInvalidItemID;
    }
    auto origin_it = helper_row_it->second.find(item_id);
    if (origin_it == helper_row_it->second.end()) {
        return kInvalidItemID;
    }
    auto row_it = d_->item_table.find(row);
    if (row_it == d_->item_table.end()) {
        return kInvalidItemID;
    }
    auto item_it = row_it->second.find(origin_it->second);
    if (item_it == row_it->second.end()) {
        return kInvalidItemID;
    }

    auto next_it = std::next(item_it);
    if (next_it == row_it->second.end()) {
        return kInvalidItemID;
    }
    return next_it->second;
}

void TimelineModel::notifyItemPropertyChanged(ItemID item_id, int role)
{
    if (!d_->items.contains(item_id)) {
        return;
    }
    emit itemChanged(item_id, role);
}

void TimelineModel::notifyItemOperateFinished(ItemID item_id, int op_role, const QVariant& param)
{
    if (!d_->items.contains(item_id)) {
        return;
    }
    emit itemOperateFinished(item_id, op_role, param);
}

void TimelineModel::setFrameMaximum(qint64 maximum)
{
    if (maximum == d_->frame_range[1] || maximum < d_->frame_range[0] + 1) {
        return;
    }
    d_->frame_range[1] = maximum;
    emit frameMaximumChanged(maximum);
}

void TimelineModel::setFrameMinimum(qint64 minimum)
{
    if (minimum == d_->frame_range[0] || minimum > d_->frame_range[1] - 1) {
        return;
    }
    d_->frame_range[0] = minimum;
    emit frameMinimumChanged(minimum);
}

qint64 TimelineModel::frameMinimum() const
{
    return d_->frame_range[0];
}

qint64 TimelineModel::frameMaximum() const
{
    return d_->frame_range[1];
}

bool TimelineModel::isFrameInRange(qint64 start, qint64 duration) const
{
    return start >= d_->view_frame_range[0] && start + duration <= d_->view_frame_range[1];
}

bool TimelineModel::isItemValid(ItemID item_id) const
{
    auto* item = this->item(item_id);
    if (!item) {
        return false;
    }
    return isFrameInRange(item->start(), item->duration());
}

void TimelineModel::setFps(double fps)
{
    d_->fps = fps;
    emit fpsChanged(fps);
}

double TimelineModel::fps() const
{
    return d_->fps;
}

void TimelineModel::setViewFrameMaximum(qint64 maximum)
{
    if (maximum == d_->view_frame_range[1] || maximum < d_->view_frame_range[0] + 1) {
        return;
    }
    d_->view_frame_range[1] = maximum;
    emit viewFrameMaximumChanged(maximum);
}

void TimelineModel::setViewFrameMinimum(qint64 minimum)
{
    if (minimum == d_->view_frame_range[0] || minimum > d_->view_frame_range[1] - 1) {
        return;
    }
    d_->view_frame_range[0] = minimum;
    emit viewFrameMinimumChanged(minimum);
}

qint64 TimelineModel::viewFrameMinimum() const
{
    return d_->view_frame_range[0];
}

qint64 TimelineModel::viewFrameMaximum() const
{
    return d_->view_frame_range[1];
}

void TimelineModel::clear()
{
    d_->id_index = 0;
    d_->dirty = false;
    d_->hidden_rows.clear();
    d_->locked_rows.clear();

    std::set<ItemID> item_ids;
    std::transform(d_->items.cbegin(), d_->items.cend(), std::inserter(item_ids, item_ids.begin()), [](const auto& pair) { return pair.first; });
    for (const auto& item_id : item_ids) {
        removeItem(item_id);
    }
}

qint64 TimelineModel::frameToTime(qint64 frame_no) const
{
    return qRound64(frame_no * 1000.0 / d_->fps);
}

bool TimelineModel::load(const nlohmann::json& j)
{
    try {
        clear();
        j.get_to(*this);
        return true;
    } catch (const std::exception& excep) {
        TL_LOG_ERROR("Failed to load item. Exception: {}", excep.what());
    }
    return false;
}

nlohmann::json TimelineModel::save() const
{
    nlohmann::json j;

    j["id_index"] = d_->id_index;
    j["row_count"] = d_->row_count;
    j["item_table"] = d_->item_table;
    j["item_table_helper"] = d_->item_table_helper;
    j["hidden_rows"] = d_->hidden_rows;
    j["locked_rows"] = d_->locked_rows;
    j["frame_range"] = d_->frame_range;
    j["view_frame_range"] = d_->view_frame_range;

    nlohmann::json items_j;
    for (const auto& [item_id, item_ptr] : d_->items) {
        nlohmann::json item_j;
        item_j["id"] = item_id;
        item_j["data"] = item_ptr->save();
        items_j.emplace_back(item_j);
    }
    j["items"] = items_j;

    nlohmann::json prev_conns_j;
    for (const auto& [item_id, conn_id] : d_->prev_conns) {
        nlohmann::json conn_item_j;
        conn_item_j["item_id"] = item_id;
        conn_item_j["connection"] = conn_id;
        prev_conns_j.emplace_back(conn_item_j);
    }
    j["prev_conns"] = prev_conns_j;

    nlohmann::json next_conns_j;
    for (const auto& [item_id, conn_id] : d_->next_conns) {
        nlohmann::json conn_item_j;
        conn_item_j["item_id"] = item_id;
        conn_item_j["connection"] = conn_id;
        next_conns_j.emplace_back(conn_item_j);
    }
    j["next_conns"] = next_conns_j;
    return j;
}

void from_json(const nlohmann::json& j, TimelineModel& model)
{
    j["id_index"].get_to(model.d_->id_index);
    j["row_count"].get_to(model.d_->row_count);
    j["item_table"].get_to(model.d_->item_table);
    j["item_table_helper"].get_to(model.d_->item_table_helper);
    j["hidden_rows"].get_to(model.d_->hidden_rows);
    j["locked_rows"].get_to(model.d_->locked_rows);
    j["frame_range"].get_to(model.d_->frame_range);
    j["view_frame_range"].get_to(model.d_->view_frame_range);

    nlohmann::json items_j = j["items"];
    for (const auto& item_j : items_j) {
        ItemID item_id = item_j["id"];
        auto item = model.itemFactory()->createItem(item_id, &model);
        if (!item) {
            throw std::exception(std::format("create item[{}] failed!", item_id).c_str());
        }
        if (!item->load(item_j["data"])) {
            throw std::exception(std::format("load item[{}] failed!", item_id).c_str());
        }
        model.d_->items[item_id] = std::move(item);
        emit model.itemCreated(item_id);
    }

    nlohmann::json prev_conns_j = j["prev_conns"];
    for (const auto& conn_item_j : prev_conns_j) {
        ItemID item_id = conn_item_j["item_id"];
        ItemConnID conn_id = conn_item_j["connection"];
        model.d_->prev_conns[item_id] = conn_id;
    }

    nlohmann::json next_conns_j = j["next_conns"];
    for (const auto& conn_item_j : next_conns_j) {
        ItemID item_id = conn_item_j["item_id"];
        ItemConnID conn_id = conn_item_j["connection"];
        model.d_->prev_conns[item_id] = conn_id;
        emit model.itemConnCreated(conn_id);
    }
}

} // namespace tl