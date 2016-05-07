#ifdef NOGUI
#include <QCoreApplication>
#else
#include <QApplication>
#endif
#include "mainwindow.h"

int main(int argc, char *argv[])
{
#ifdef NOGUI
    QCoreApplication a(argc, argv);
    int default_restart_seconds = 90;
    MainWindow w(true, default_restart_seconds);
#else
    QApplication a(argc, argv);
    MainWindow w(false);
    w.show();
#endif

    return a.exec();
}
