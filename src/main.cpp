#include <QApplication>
#include <QSettings>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("words-searcher");
    QCoreApplication::setApplicationName("words-searcher");
    // Загружаем перевод при старте в MainWindow, чтобы был один указатель.

    MainWindow w;
    w.show();
    return app.exec();
}
