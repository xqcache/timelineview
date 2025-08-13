#include "timelinetransaction.h"
#include "item/timelineitem.h"
#include "timelinemodel.h"
#include <QCoreApplication>

namespace tl {
TimelineItemCreateCommand::TimelineItemCreateCommand(TimelineModel* model, ItemID item_id, QUndoCommand* parent)
    : QUndoCommand(parent)
    , model_(model)
    , item_id_(item_id)
    , item_data_(model->saveItem(item_id))
{
    setText(QCoreApplication::translate("TimelineItemCreateCommand", "Create Item"));
}

void TimelineItemCreateCommand::undo()
{
    model_->removeItem(item_id_);
}

void TimelineItemCreateCommand::redo()
{
    model_->loadItem(item_data_);
}

TimelineItemDeleteCommand::TimelineItemDeleteCommand(TimelineModel* model, ItemID item_id, QUndoCommand* parent)
    : QUndoCommand(parent)
    , model_(model)
    , item_id_(item_id)
    , item_data_(model_->saveItem(item_id_))
{
    setText(QCoreApplication::translate("TimelineItemDeleteCommand", "Delete Item"));
}

void TimelineItemDeleteCommand::undo()
{
    model_->loadItem(item_data_);
}

void TimelineItemDeleteCommand::redo()
{
    model_->removeItem(item_id_);
}

TimelineItemMoveCommand::TimelineItemMoveCommand(TimelineModel* model, ItemID item_id, qint64 old_start, QUndoCommand* parent)
    : QUndoCommand(parent)
    , model_(model)
    , item_id_(item_id)
    , old_start_(old_start)
{
    if (auto* item = model_->item(item_id_); item) {
        new_start_ = item->start();
    }

    setText(QCoreApplication::translate("TimelineItemMoveCommand", "Move Item"));
}

void TimelineItemMoveCommand::undo()
{
    auto* item = model_->item(item_id_);
    if (old_start_ >= 0 && item && old_start_ != item->start()) {
        model_->modifyItemStart(item_id_, old_start_);
    }
}

void TimelineItemMoveCommand::redo()
{
    auto* item = model_->item(item_id_);
    if (new_start_ >= 0 && item && new_start_ != item->start()) {
        model_->modifyItemStart(item_id_, new_start_);
    }
}

} // namespace tl