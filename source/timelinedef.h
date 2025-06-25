#pragma once

#include <limits>
#include <qtypes.h>

namespace tl {
using ItemID = quint64;
constexpr ItemID kInvalidItemID = std::numeric_limits<ItemID>::max();
} // namespace tl

#ifndef TL_LOG_ERROR
#include <QDebug>
#define TL_LOG_ERROR(msg) qDebug() << (msg)
#endif

#ifndef TL_LOG_INFO
#include <QDebug>
#define TL_LOG_INFO(msg) qDebug() << (msg)
#endif