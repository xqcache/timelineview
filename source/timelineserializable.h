#pragma once

#include "nlohmann/json.hpp"

namespace tl {
class TimelineSerializable {
public:
    virtual ~TimelineSerializable() noexcept = default;

    virtual bool load(const nlohmann::json& j) = 0;
    virtual nlohmann::json save() const = 0;
};
} // namespace tl