#pragma once

#include <limits>
#include <qtypes.h>

namespace tl {
using ItemID = quint64;
constexpr ItemID kInvalidItemID = std::numeric_limits<ItemID>::max();

struct ItemConnID {
    ItemID from = kInvalidItemID;
    ItemID to = kInvalidItemID;

    bool isValid() const
    {
        return from != kInvalidItemID && to != kInvalidItemID;
    }
};

struct ItemConnIDHash {
    std::size_t operator()(const ItemConnID& conn_id) const
    {
        return std::hash<ItemID>()(conn_id.from) ^ std::hash<ItemID>()(conn_id.to);
    };
};

struct ItemConnIDEqual {
    bool operator()(const ItemConnID& lhs, const ItemConnID& rhs) const
    {
        return lhs.from == rhs.from && lhs.to == rhs.to;
    }
};

} // namespace tl

#ifndef TL_LOG_ERROR
#include <QDebug>
#include <format>
#define TL_LOG_ERROR(msg, ...) qDebug() << std::format(msg, __VA_ARGS__)
#endif

#ifndef TL_LOG_INFO
#include <QDebug>
#include <format>
#define TL_LOG_INFO(msg, ...) qDebug() << std::format(msg, __VA_ARGS__)
#endif