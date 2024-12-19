#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");
    w.show();
    return a.exec();
}
