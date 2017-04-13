#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QCoreApplication::setApplicationName("MDI Example");
//    QCoreApplication::setOrganizationName("QtProject");
//    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    MainWindow w;
    w.show();

    return a.exec();
}
