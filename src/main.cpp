#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("words-searcher");
    QCoreApplication::setApplicationName("words-searcher");
    QTranslator translator;
    if (translator.load(QLocale(), "words-searcher", "_", ":/i18n")) {
        app.installTranslator(&translator);
    }
    MainWindow w;
    w.show();
    return app.exec();
}
