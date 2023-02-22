#include "MainDialog.h"
#include <QApplication>
#include <Tab_NS_Monitor.h>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainDialog w;
    w.show();
    return a.exec();
}
