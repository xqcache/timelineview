#include "item/timelinearmitem.h"
#include "item/timelineaudioitem.h"
#include "item/timelinevideoitem.h"
#include "timelineaxis.h"
#include "timelinemediautil.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include "timelineview.h"
#include <QApplication>
#include <QFileDialog>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    tl::TimelineView view;
    tl::TimelineModel* model = new tl::TimelineModel(&view);
    tl::TimelineScene* scene = new tl::TimelineScene(model, &view);

    view.setScene(scene);
    view.setFrameMode(false);

    model->setFrameMaximum(2000);
    model->setFrameMinimum(0);
    model->setViewFrameMaximum(2000);
    model->setViewFrameMinimum(0);
    model->setRowCount(3);
    model->setFps(25.0);

    // TODO: Only for test
    view.addAction("Add Audio", QString("Ctrl+A"), &view, [model, &view] {
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
    });

    view.addAction("Add Video", QString("Ctrl+V"), &view, [model, &view] {
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

    view.resize(1000, 400);
    view.show();
    return app.exec();
}