#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("words-searcher");
    QCoreApplication::setApplicationName("words-searcher");
    MainWindow w;
    w.show();
    return app.exec();
}
