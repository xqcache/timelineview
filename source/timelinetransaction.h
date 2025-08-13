#pragma once

#include "nlohmann/json.hpp"
#include "timelinedef.h"
#include <QUndoCommand>

namespace tl {

class TimelineModel;

class TimelineItemCreateCommand : public QUndoCommand {
public:
    explicit TimelineItemCreateCommand(TimelineModel* model, ItemID item_id, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    TimelineModel* model_ { nullptr };
    ItemID item_id_ { kInvalidItemID };
    nlohmann::json item_data_;
};

class TimelineItemDeleteCommand : public QUndoCommand {
public:
    explicit TimelineItemDeleteCommand(TimelineModel* model, ItemID item_id, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    TimelineModel* model_ { nullptr };
    ItemID item_id_ { kInvalidItemID };
    nlohmann::json item_data_;
};

class TimelineItemMoveCommand : public QUndoCommand {
public:
    explicit TimelineItemMoveCommand(TimelineModel* model, ItemID item_id, qint64 old_start, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    TimelineModel* model_ { nullptr };
    ItemID item_id_ { kInvalidItemID };
    qint64 old_start_ { -1 };
    qint64 new_start_ { -1 };
};

} // namespace tl