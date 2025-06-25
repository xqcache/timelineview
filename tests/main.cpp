#include "timelinemodel.h"
#include "timelinescene.h"
#include "timelineview.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    tl::TimelineView view;
    view.resize(1024, 400);

    auto* model = new tl::TimelineModel(&view);
    auto* scene = new tl::TimelineScene(&view);

    scene->setModel(model);
    view.setScene(scene);
    view.show();

    return app.exec();
}