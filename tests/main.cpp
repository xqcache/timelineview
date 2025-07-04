#include "item/timelinearmitem.h"
#include "timelineaxis.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include "timelineview.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    tl::TimelineView view;
    tl::TimelineModel* model = new tl::TimelineModel(&view);
    tl::TimelineScene* scene = new tl::TimelineScene(model, &view);

    view.setScene(scene);
    view.setFrameMode(true);

    model->setFrameMaximum(10000);
    model->setFrameMinimum(0);
    model->setViewFrameMaximum(10000);
    model->setViewFrameMinimum(0);
    model->setFps(25.0);

    // TODO: Only for test
    view.addAction("Add", QString("Ctrl+N"), &view, [model, &view] {
        qint64 start = view.axis()->frame();
        model->createItem(tl::TimelineArmItem::Type, 0, start, 0, true);
    });
    view.addAction("Save", QString("Ctrl+S"), &view, [model] { qDebug() << model->save().dump(4).c_str(); });

    view.resize(1000, 400);
    view.show();
    return app.exec();
}