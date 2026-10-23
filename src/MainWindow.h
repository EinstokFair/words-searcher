#pragma once
#include <QMainWindow>
class QCloseEvent;
class QKeyEvent;
class QEvent;
class QShortcut;
class QTranslator;
class FsProxyModel;

class QTreeView;
class QFileSystemModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onSettings();
    void onRefresh();
    void onToggleHidden(bool on);
    void onZoomIn();
    void onZoomOut();
    void onThemeLight();
    void onThemeDark();
    void onLangRu();
    void onLangEn();
    void switchLanguage(const QString &code);
    void retranslateUi();
    void applyTheme(bool dark);
    void applyFontDelta(int delta);

protected:
    void closeEvent(QCloseEvent *e) override;
    void changeEvent(QEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    void setupModel();
    void applyRoot(const QString &path);

    QTreeView *m_tree = nullptr;
    QFileSystemModel *m_fsModel = nullptr;
    FsProxyModel *m_proxy = nullptr;
    QTranslator *m_translator = nullptr;
    int m_fontDelta = 0;
    int m_baseFontPointSize = 0;
};
