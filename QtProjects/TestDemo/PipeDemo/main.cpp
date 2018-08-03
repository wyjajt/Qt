#include "pipedemo.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    PipeDemo w;
    w.show();

    return a.exec();
}
