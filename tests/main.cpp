#include "timelineedit.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    tl::TimelineEdit edit;

    edit.resize(1000, 400);
    edit.show();
    return app.exec();
}