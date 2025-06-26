#include "item/timelinearmitem.h"
#include "timelinemodel.h"
#include "timelinescene.h"
#include "timelineview.h"
#include <QAction>
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    tl::TimelineView view;
    view.resize(1024, 400);

    auto* model = new tl::TimelineModel(&view);
    auto* scene = new tl::TimelineScene(model, &view);

    model->setRowCount(5);

    view.setScene(scene);
    view.setAxisPlayheadHeight(40);
    view.show();

    view.addAction("Add", QString("Ctrl+N"), [&model, &view] { model->createItem(tl::TimelineArmItem::Type, 0, view.currentTime(), 0, true); });
    view.addAction("Save", QString("Ctrl+S"), [&model, &view] { qDebug() << model->save().dump(4).c_str(); });

    return app.exec();
}