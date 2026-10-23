#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("words-searcher");
    QCoreApplication::setApplicationName("words-searcher");
    // Загружаем перевод при старте в MainWindow, чтобы был один указатель.

    QSettings settings;
    QString lang = settings.value(QStringLiteral("language"), QStringLiteral("ru")).toString();
    if (lang == QStringLiteral("ru")) {
        QTranslator qtTranslator;
        if (qtTranslator.load(QStringLiteral("qtbase_ru"), QStringLiteral("/usr/share/qt6/translations"))) {
            app.installTranslator(&qtTranslator);
        }
    }
    MainWindow w;
    w.show();
    return app.exec();
}
