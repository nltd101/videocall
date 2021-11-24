#include "mainwindow.h"
#include <QApplication>
#define DEBUG true
#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
