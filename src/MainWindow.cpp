#include "MainWindow.h"
#include "SettingsDialog.h"
#include "FsProxyModel.h"
#include <QApplication>
#include <QDir>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QStatusBar>
#include <QTreeView>
#include <QCloseEvent>
#include <QDebug>
#include <QPalette>
#include <QStyle>
#include <QKeyEvent>
#include <QTranslator>
#include <QEvent>
#include <QWindow>
#include <QProcess>
#include <QRegularExpression>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QShortcut>
#include <QKeySequence>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << "[i18n] setWindowTitle to" << tr("Искатель слов");
    setWindowTitle(tr("Искатель слов"));
    if (windowHandle()) windowHandle()->setTitle(tr("Искатель слов"));
    {
        QFont f0 = QApplication::font();
        m_baseFontPointSize = f0.pointSize() > 0 ? f0.pointSize() : 10;
    }
    resize(900, 600);

    m_tree = new QTreeView(this);
    m_tree->setUniformRowHeights(true);
    m_tree->setAlternatingRowColors(true);
    setCentralWidget(m_tree);

    setupModel();
    retranslateUi();
    {
        QSettings langSettings;
        QString lang = langSettings.value(QStringLiteral("language"), QStringLiteral("ru")).toString();
        switchLanguage(lang);
    }

    QSettings s;
    const QString root = s.value(QStringLiteral("rootPath"),
                                 QDir::homePath()).toString();
    const bool showHidden = s.value(QStringLiteral("showHidden"), false).toBool();
    m_fsModel->setFilter(showHidden
        ? (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System)
        : (QDir::AllEntries | QDir::NoDotAndDotDot));
    applyRoot(root);

    restoreGeometry(s.value(QStringLiteral("geometry")).toByteArray());
    {
        const int savedDelta = s.value(QStringLiteral("fontDelta"), 0).toInt();
        if (savedDelta != 0) {
            m_fontDelta = 0;
            applyFontDelta(savedDelta);
        }
    }
    {
        const QString theme = s.value(QStringLiteral("theme")).toString();
        if (theme == QStringLiteral("light"))
            applyTheme(false);
        else if (theme == QStringLiteral("dark"))
            applyTheme(true);
    }
}

void MainWindow::setupModel()
{
    m_fsModel = new QFileSystemModel(this);
    m_fsModel->setReadOnly(true);
    m_fsModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    m_proxy = new FsProxyModel(this);
    m_proxy->setSourceModel(m_fsModel);
    m_tree->setModel(m_proxy);
    m_tree->setSortingEnabled(true);
    m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_tree, &QTreeView::customContextMenuRequested, this, &MainWindow::onContextMenu);
    m_tree->header()->setSortIndicatorShown(true);
    m_tree->setRootIndex(QModelIndex());
    m_tree->header()->setSectionResizeMode(FsProxyModel::ColIcon, QHeaderView::ResizeToContents);
    m_tree->header()->setSectionResizeMode(FsProxyModel::ColDate, QHeaderView::ResizeToContents);
    m_tree->header()->setSectionResizeMode(FsProxyModel::ColName, QHeaderView::Stretch);
    m_tree->header()->setSectionResizeMode(FsProxyModel::ColSize, QHeaderView::ResizeToContents);
    m_tree->setColumnHidden(FsProxyModel::ColIcon, false);
}

void MainWindow::applyRoot(const QString &path)
{
    QFileInfo fi(path);
    if (!fi.exists() || !fi.isDir() || !fi.isReadable()) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Путь недоступен для чтения: %1").arg(path));
        statusBar()->showMessage(tr("Ошибка: путь недоступен: %1").arg(path));
        return;
    }
    const QModelIndex idx = m_fsModel->setRootPath(path);
    m_tree->setRootIndex(m_proxy->mapFromSource(idx));
    statusBar()->showMessage(tr("Корень: %1").arg(path));
}

void MainWindow::onSettings()
{
    QSettings s;
    SettingsDialog dlg(this);
    dlg.setRootPath(s.value(QStringLiteral("rootPath"), QDir::homePath()).toString());
    dlg.setShowHidden(s.value(QStringLiteral("showHidden"), false).toBool());
    dlg.setEditorList(s.value(QStringLiteral("editorList"), QStringList()).toStringList());
    if (dlg.exec() == QDialog::Accepted) {
        const QString p = dlg.rootPath();
        QFileInfo fi(p);
        if (!fi.exists() || !fi.isDir() || !fi.isReadable()) {
            QMessageBox::warning(this, QStringLiteral("Ошибка"),
                                 QStringLiteral("Путь недоступен для чтения: %1").arg(p));
            return;
        }
        s.setValue(QStringLiteral("rootPath"), p);
        s.setValue(QStringLiteral("showHidden"), dlg.showHidden());
        m_fsModel->setFilter(dlg.showHidden()
            ? (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System)
            : (QDir::AllEntries | QDir::NoDotAndDotDot));
        applyRoot(p);
    }
}

void MainWindow::onRefresh()
{
    const QString root = m_fsModel->rootPath();
    applyRoot(root);
}

void MainWindow::onToggleHidden(bool on)
{
    QSettings s;
    s.setValue(QStringLiteral("showHidden"), on);
    m_fsModel->setFilter(on
        ? (QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System)
        : (QDir::AllEntries | QDir::NoDotAndDotDot));
    applyRoot(m_fsModel->rootPath());
}

void MainWindow::onZoomIn()  { applyFontDelta(+1); }
void MainWindow::onZoomOut() { applyFontDelta(-1); }

void MainWindow::applyFontDelta(int delta)
{
    if (m_baseFontPointSize <= 0) {
        QFont f0 = QApplication::font();
        m_baseFontPointSize = f0.pointSize() > 0 ? f0.pointSize() : 10;
    }
    m_fontDelta = qBound(-5, m_fontDelta + delta, 20);
    qDebug() << "[font] delta=" << delta << "m_fontDelta=" << m_fontDelta
             << "base=" << m_baseFontPointSize;
    QFont f = QApplication::font();
    f.setPointSize(qMax(6, m_baseFontPointSize + m_fontDelta));
    QApplication::setFont(f);
    if (m_tree) {
        m_tree->setFont(f);
        m_tree->doItemsLayout();
        m_tree->viewport()->update();
    }
    if (menuBar()) menuBar()->update();
    if (statusBar()) statusBar()->update();
    update();
    QSettings s;
    s.setValue(QStringLiteral("fontDelta"), m_fontDelta);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    QSettings s;
    s.setValue(QStringLiteral("geometry"), saveGeometry());
    QMainWindow::closeEvent(e);
}


void MainWindow::onThemeLight()
{
    applyTheme(false);
    QSettings s;
    s.setValue(QStringLiteral("theme"), QStringLiteral("light"));
}

void MainWindow::onThemeDark()
{
    applyTheme(true);
    QSettings s;
    s.setValue(QStringLiteral("theme"), QStringLiteral("dark"));
}

void MainWindow::applyTheme(bool dark)
{
    QPalette pal;
    if (dark) {
        pal.setColor(QPalette::Window, QColor(53, 53, 53));
        pal.setColor(QPalette::WindowText, Qt::white);
        pal.setColor(QPalette::Base, QColor(35, 35, 35));
        pal.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        pal.setColor(QPalette::ToolTipBase, QColor(53, 53, 53));
        pal.setColor(QPalette::ToolTipText, Qt::white);
        pal.setColor(QPalette::Text, Qt::white);
        pal.setColor(QPalette::Button, QColor(53, 53, 53));
        pal.setColor(QPalette::ButtonText, Qt::white);
        pal.setColor(QPalette::BrightText, Qt::red);
        pal.setColor(QPalette::Link, QColor(42, 130, 218));
        pal.setColor(QPalette::Highlight, QColor(42, 130, 218));
        pal.setColor(QPalette::HighlightedText, Qt::black);
    } else {
        pal = QApplication::style()->standardPalette();
    }
    QApplication::setPalette(pal);
    qDebug() << "[theme] dark=" << dark;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->key() == Qt::Key_Plus || e->key() == Qt::Key_Equal) {
            onZoomIn();
            e->accept();
            return;
        }
        if (e->key() == Qt::Key_Minus) {
            onZoomOut();
            e->accept();
            return;
        }
    }
    QMainWindow::keyPressEvent(e);
}

void MainWindow::onLangRu()
{
    switchLanguage(QStringLiteral("ru"));
}

void MainWindow::onLangEn()
{
    switchLanguage(QStringLiteral("en"));
}

void MainWindow::switchLanguage(const QString &code)
{
    qDebug() << "[i18n] switchLanguage to" << code;

    if (!m_translator) m_translator = new QTranslator(this);
    if (!m_qtTranslator) m_qtTranslator = new QTranslator(this);

    // Всегда убираем старые переводчики
    QCoreApplication::removeTranslator(m_translator);
    QCoreApplication::removeTranslator(m_qtTranslator);

    if (code == QStringLiteral("en")) {
        if (m_translator->load(QStringLiteral(":/i18n/words-searcher_en.qm"))) {
            QCoreApplication::installTranslator(m_translator);
        }
    } else if (code == QStringLiteral("ru")) {
        // Загружаем стандартный перевод Qt для кнопок
        if (m_qtTranslator->load(QStringLiteral("qtbase_ru"), QStringLiteral("/usr/share/qt6/translations"))) {
            QCoreApplication::installTranslator(m_qtTranslator);
        }
    }

    QSettings s;
    s.setValue(QStringLiteral("language"), code);
}


void MainWindow::retranslateUi()
{
    qDebug() << "[i18n] setWindowTitle to" << tr("Искатель слов");
    setWindowTitle(tr("Искатель слов"));
    if (windowHandle()) windowHandle()->setTitle(tr("Искатель слов"));

    menuBar()->clear();

    QMenu *searchMenu = menuBar()->addMenu(tr("Поиск"));
    QAction *actQuit = searchMenu->addAction(tr("Выход"));
    connect(actQuit, &QAction::triggered, this, &QWidget::close);

    QMenu *viewMenu = menuBar()->addMenu(tr("Вид"));
    QAction *actRefresh = viewMenu->addAction(tr("Обновить"));
    actRefresh->setShortcut(QKeySequence(Qt::Key_F5));
    connect(actRefresh, &QAction::triggered, this, &MainWindow::onRefresh);

    QAction *actHidden = viewMenu->addAction(tr("Показать скрытые"));
    actHidden->setCheckable(true);
    connect(actHidden, &QAction::toggled, this, &MainWindow::onToggleHidden);

    viewMenu->addSeparator();

    QAction *actZoomIn = viewMenu->addAction(tr("Увеличить шрифт"));
    connect(actZoomIn, &QAction::triggered, this, &MainWindow::onZoomIn);
    QAction *actZoomOut = viewMenu->addAction(tr("Уменьшить шрифт"));
    connect(actZoomOut, &QAction::triggered, this, &MainWindow::onZoomOut);

    QMenu *helpMenu = menuBar()->addMenu(tr("Справка"));
    QAction *actSettings = helpMenu->addAction(tr("Настройки…"));
    connect(actSettings, &QAction::triggered, this, &MainWindow::onSettings);

    helpMenu->addSeparator();

    QAction *actRu = helpMenu->addAction(tr("Русский"));
    connect(actRu, &QAction::triggered, this, &MainWindow::onLangRu);
    QAction *actEn = helpMenu->addAction(tr("Английский"));
    connect(actEn, &QAction::triggered, this, &MainWindow::onLangEn);

    helpMenu->addSeparator();

    QAction *actLight = helpMenu->addAction(tr("Светлая тема"));
    connect(actLight, &QAction::triggered, this, &MainWindow::onThemeLight);
    QAction *actDark = helpMenu->addAction(tr("Тёмная тема"));
    connect(actDark, &QAction::triggered, this, &MainWindow::onThemeDark);

    helpMenu->addSeparator();

    QAction *actAbout = helpMenu->addAction(tr("О программе"));
    connect(actAbout, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, tr("О программе"), tr("Искатель слов 0.0.1"));
    });

    // Восстанавливаем состояние чекбокса скрытых файлов
    QSettings s;
    bool showHidden = s.value(QStringLiteral("showHidden"), false).toBool();
}

void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QMainWindow::changeEvent(e);
}

void MainWindow::onContextMenu(const QPoint &pos)
{
    QModelIndex index = m_tree->indexAt(pos);
    if (!index.isValid()) return;

    QFileInfo fi = m_fsModel->fileInfo(m_proxy->mapToSource(index));
    bool isDir = fi.isDir();

    QMenu menu(this);

    if (isDir) {
        QAction *actDolphin = menu.addAction(tr("Открыть директорию в Dolphin"));
        connect(actDolphin, &QAction::triggered, this, [fi]() {
            QProcess::startDetached("dolphin", {fi.absoluteFilePath()});
        });
        QAction *actNautilus = menu.addAction(tr("Открыть директорию в Nautilus"));
        connect(actNautilus, &QAction::triggered, this, [fi]() {
            QProcess::startDetached("nautilus", {fi.absoluteFilePath()});
        });
    } else {
        QSettings s;
        QStringList editorList = s.value(QStringLiteral("editorList"), QStringList()).toStringList();

        for (const QString &entry : editorList) {
            const QStringList parts = entry.split('|');
            QString name = parts.size() > 0 ? parts[0] : QFileInfo(entry).fileName();
            QString path = parts.size() > 1 ? parts[1] : entry;
            QString tmpl = parts.size() > 2 ? parts[2] : QStringLiteral("$FILE_PATH");
            QAction *act = menu.addAction(tr("Открыть в %1").arg(name));
            connect(act, &QAction::triggered, this, [this, path, tmpl, fi]() {
                QSettings s;
                QString lang = s.value(QStringLiteral("language"), QStringLiteral("ru")).toString();
                int fontSize = s.value(QStringLiteral("fontDelta"), 0).toInt() + 10;
                QString arg = tmpl;
                arg.replace("$FILE_PATH", fi.absoluteFilePath());
                arg.replace("$LANGUAGE", lang);
                arg.replace("$FONT_SIZE", QString::number(fontSize));
                QProcess::startDetached(path, {arg});
            });
        }
        menu.addSeparator();
        QAction *actFirefox = menu.addAction(tr("Открыть файл в Firefox"));
        connect(actFirefox, &QAction::triggered, this, [fi]() {
            QProcess::startDetached("firefox", {fi.absoluteFilePath()});
        });
    }

    menu.exec(m_tree->viewport()->mapToGlobal(pos));
}

