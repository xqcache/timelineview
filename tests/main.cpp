#include "item/timelinearmitem.h"
#include "item/timelineaudioitem.h"
#include "item/timelinevideoitem.h"
#include "timelineaxis.h"
#include "timelinemediautil.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include "timelinetransaction.h"
#include "timelineview.h"
#include <QApplication>
#include <QClipboard>
#include <QCursor>
#include <QFileDialog>
#include <QMenu>
#include <QMimeData>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    tl::TimelineView view;
    tl::TimelineModel* model = new tl::TimelineModel(&view);
    tl::TimelineScene* scene = new tl::TimelineScene(model, &view);

    view.setScene(scene);
    view.setFrameMode(false);

    model->setFrameMaximum(10000);
    model->setFrameMinimum(0);
    model->setViewFrameMaximum(10000);
    model->setViewFrameMinimum(0);
    model->setRowCount(3);
    model->setFps(25.0);

    // TODO: Only for test
    view.addAction("Add Audio", QString("Ctrl+A"), &view, [model, &view, scene] {
        qint64 start = view.axis()->frame();
        QString path = QFileDialog::getOpenFileName(nullptr, "Open Audio", "", "Audio Files (*.mp3 *.wav *.flac *.aac *.ogg *.m4a)");
        auto media_info = tl::TimelineMediaUtil::loadAudio(path, model->fps());
        if (!media_info) {
            return;
        }
        auto item_id = model->createItem(tl::TimelineAudioItem::Type, 2, start, media_info->frame_count, false);
        if (item_id == tl::kInvalidItemID) {
            return;
        }
        model->setItemProperty(item_id, tl::TimelineAudioItem::AudioInfoRole, QVariant::fromValue(media_info.value()));
        scene->recordUndo(new tl::TimelineItemCreateCommand(model, item_id));
    });

    view.addAction("Add Video", QString("Ctrl+M"), &view, [model, &view] {
        qint64 start = view.axis()->frame();
        QString path = QFileDialog::getOpenFileName(nullptr, "Open Video", "", "Video Files (*.mp4 *.avi *.mov *.mkv *.flv *.wmv *.webm)");
        auto media_info = tl::TimelineMediaUtil::loadVideo(path);
        if (!media_info) {
            return;
        }
        auto item_id = model->createItem(tl::TimelineVideoItem::Type, 0, start, media_info->frame_count, false);
        if (item_id == tl::kInvalidItemID) {
            return;
        }
        model->setItemProperty(item_id, tl::TimelineVideoItem::VideoInfoRole, QVariant::fromValue(media_info.value()));
    });

    view.addAction("Save", QString("Ctrl+S"), &view, [model] { qDebug() << model->save().dump(4).c_str(); });

    view.addAction("Open", QString("Ctrl+O"), &view, [model] {
        QString path = QFileDialog::getOpenFileName(nullptr, "Open Video", "", "Video Files (*.mp4 *.avi *.mov *.mkv *.flv *.wmv *.webm)");
        if (path.isEmpty()) {
            return;
        }
        auto media_info = tl::TimelineMediaUtil::loadVideo(path);
        if (!media_info) {
            return;
        }
        qDebug() << tl::TimelineMediaUtil::mediaInfoString(*media_info);
    });

    QObject::connect(scene, &tl::TimelineScene::requestMoveItem, &view,
        [scene](tl::ItemID item_id, qint64 frame_no) { scene->model()->modifyItemStart(item_id, frame_no); });

    QObject::connect(scene, &tl::TimelineScene::requestRecordMoveCommand, &view,
        [scene](tl::ItemID item_id, qint64 frame_no) { scene->recordUndo(new tl::TimelineItemMoveCommand(scene->model(), item_id, frame_no)); });

    QObject::connect(scene, &tl::TimelineScene::requestItemContextMenu, &view, [model, scene, &view](tl::ItemID item_id) {
        QMenu menu(&view);
        menu.addAction("Remove", QString("Del"), &view, [scene, model, item_id] { scene->recordUndo(new tl::TimelineItemDeleteCommand(model, item_id)); });
        menu.addAction("Copy", &view, [model, item_id] {
            auto data = model->copyItem(item_id);
            if (data.isEmpty()) {
                return;
            }
            QClipboard* clipboard = QApplication::clipboard();
            QMimeData* mime_data = new QMimeData();
            mime_data->setData("application/timeline-item", data.toUtf8());
            clipboard->setMimeData(mime_data);
        });
        menu.exec(QCursor::pos());
    });

    QObject::connect(scene, &tl::TimelineScene::requestSceneContextMenu, &view, [model, scene, &view] {
        QMenu menu(&view);
        const QMimeData* mime_data = QApplication::clipboard()->mimeData();
        bool can_paste = mime_data && mime_data->hasFormat("application/timeline-item");
        menu.addAction("Paste", &view,
                [model, mime_data, scene, &view] {
                    auto data = QString::fromUtf8(mime_data->data("application/timeline-item"));
                    auto item_id = model->pasteItem(data, view.axis()->frame());
                    if (item_id != tl::kInvalidItemID) {
                        scene->recordUndo(new tl::TimelineItemCreateCommand(model, item_id));
                    }
                })
            ->setEnabled(can_paste);
        menu.exec(QCursor::pos());
    });

    QObject::connect(scene->undoStack(), &QUndoStack::undoTextChanged, &view, [&view](const QString& undo_text) { qDebug() << "Undo: " << undo_text; });
    QObject::connect(scene->undoStack(), &QUndoStack::redoTextChanged, &view, [&view](const QString& redo_text) { qDebug() << "Redo: " << redo_text; });

    view.addAction("Undo", QString("Ctrl+Z"), &view, [scene] { scene->undo(); });
    view.addAction("Redo", QString("Ctrl+Y"), &view, [scene] { scene->redo(); });

    view.resize(1000, 400);
    view.show();
    return app.exec();
}