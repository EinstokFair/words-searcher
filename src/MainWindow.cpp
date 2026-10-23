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
#include <QShortcut>
#include <QKeySequence>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("words-searcher"));
    {
        QFont f0 = QApplication::font();
        m_baseFontPointSize = f0.pointSize() > 0 ? f0.pointSize() : 10;
    }
    resize(900, 600);

    m_tree = new QTreeView(this);
    m_tree->setUniformRowHeights(true);
    m_tree->setAlternatingRowColors(true);
    setCentralWidget(m_tree);

    QMenu *searchMenu = menuBar()->addMenu(tr("Поиск"));
    QAction *actQuit = searchMenu->addAction(tr("Выход"));
    connect(actQuit, &QAction::triggered, this, &QWidget::close);

    QMenu *viewMenu = menuBar()->addMenu(tr("Вид"));
    QAction *actZoomIn = viewMenu->addAction(tr("Увеличить шрифт"));
    connect(actZoomIn, &QAction::triggered, this, &MainWindow::onZoomIn);
    QAction *actZoomOut = viewMenu->addAction(tr("Уменьшить шрифт"));
    connect(actZoomOut, &QAction::triggered, this, &MainWindow::onZoomOut);
    viewMenu->addSeparator();
    QAction *actRefresh = viewMenu->addAction(tr("Обновить"));
    actRefresh->setShortcut(QKeySequence(Qt::Key_F5));
    connect(actRefresh, &QAction::triggered, this, &MainWindow::onRefresh);
    QAction *actHidden = viewMenu->addAction(tr("Показать скрытые"));
    actHidden->setCheckable(true);
    connect(actHidden, &QAction::toggled, this, &MainWindow::onToggleHidden);

    QMenu *helpMenu = menuBar()->addMenu(tr("Справка"));
    QAction *actSettings = helpMenu->addAction(tr("Настройки…"));
    connect(actSettings, &QAction::triggered, this, &MainWindow::onSettings);
    helpMenu->addSeparator();
    QAction *actLight = helpMenu->addAction(tr("Светлая тема"));
    connect(actLight, &QAction::triggered, this, &MainWindow::onThemeLight);
    QAction *actDark = helpMenu->addAction(tr("Тёмная тема"));
    connect(actDark, &QAction::triggered, this, &MainWindow::onThemeDark);
    helpMenu->addSeparator();
    QAction *actAbout = helpMenu->addAction(tr("О программе"));
    connect(actAbout, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, tr("О программе"),
                           tr("words-searcher 0.0.1"));
    });

    setupModel();

    QSettings s;
    const QString root = s.value(QStringLiteral("rootPath"),
                                 QDir::homePath()).toString();
    const bool showHidden = s.value(QStringLiteral("showHidden"), false).toBool();
    actHidden->setChecked(showHidden);
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
