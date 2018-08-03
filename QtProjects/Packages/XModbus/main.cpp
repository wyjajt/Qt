#include <QApplication>
#include <managertool.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ManagerTool  window;
    window.show();

    return a.exec();
}
