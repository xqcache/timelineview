#include "timelineview.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    tl::TimelineView view;

    view.resize(1000, 400);
    view.show();
    return app.exec();
}