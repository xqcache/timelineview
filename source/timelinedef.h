#pragma once

#include <functional>
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
#ifndef SPDLOG_H
#include <spdlog/spdlog.h>
#endif
#define TL_LOG_ERROR(...) SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__)
#endif

#ifndef TL_LOG_INFO
#ifndef SPDLOG_H
#include <spdlog/spdlog.h>
#endif
#define TL_LOG_WARN(...) SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__)
#endif

#ifndef TL_LOG_INFO
#ifndef SPDLOG_H
#include <spdlog/spdlog.h>
#endif
#define TL_LOG_INFO(...) SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__)
#endif

#ifndef TL_LOG_DEBUG
#ifndef SPDLOG_H
#include <spdlog/spdlog.h>
#endif
#define TL_LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(spdlog::default_logger_raw(), __VA_ARGS__)
#endif
