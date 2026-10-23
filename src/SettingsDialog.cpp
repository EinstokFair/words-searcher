#include "SettingsDialog.h"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Настройки"));
    setModal(true);

    m_pathEdit = new QLineEdit(this);
    auto *browse = new QPushButton(tr("Обзор…"), this);
    connect(browse, &QPushButton::clicked, this, &SettingsDialog::onBrowse);

    auto *pathRow = new QHBoxLayout;
    pathRow->addWidget(m_pathEdit, 1);
    pathRow->addWidget(browse);

    m_hidden = new QCheckBox(tr("Показывать скрытые"), this);

    m_editorList = new QTreeWidget(this);
    m_editorList->setColumnCount(2);
    m_editorList->setHeaderLabels({tr("Название"), tr("Путь")});
    m_editorList->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_editorList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_editorList->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    m_editorList->setRootIsDecorated(false);
    m_editorList->setDragDropMode(QAbstractItemView::InternalMove);
    m_editorList->setDefaultDropAction(Qt::MoveAction);
    m_editorList->setDragEnabled(true);
    m_editorList->setAcceptDrops(true);
    m_editorList->setDropIndicatorShown(true);

    auto *addBtn = new QPushButton(tr("Добавить"), this);
    auto *removeBtn = new QPushButton(tr("Удалить"), this);
    auto *argsBtn = new QPushButton(tr("Аргументы"), this);
    connect(addBtn, &QPushButton::clicked, this, &SettingsDialog::onAddEditor);
    connect(removeBtn, &QPushButton::clicked, this, &SettingsDialog::onRemoveEditor);
    connect(argsBtn, &QPushButton::clicked, this, &SettingsDialog::onEditArgs);

    auto *btnRow = new QHBoxLayout;
    btnRow->addWidget(addBtn);
    btnRow->addWidget(removeBtn);
    btnRow->addWidget(argsBtn);
    btnRow->addStretch();

    auto *form = new QFormLayout;
    form->addRow(tr("Корневая директория:"), pathRow);
    form->addRow(QString(), m_hidden);
    form->addRow(tr("Редакторы:"), m_editorList);
    form->addRow(QString(), btnRow);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply,
        this);

    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (validateAndApply()) accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttons->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, [this]() { validateAndApply(); });

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

void SettingsDialog::setRootPath(const QString &p) { m_pathEdit->setText(p); }
QString SettingsDialog::rootPath() const { return m_pathEdit->text().trimmed(); }
void SettingsDialog::setShowHidden(bool on) { m_hidden->setChecked(on); }
bool SettingsDialog::showHidden() const { return m_hidden->isChecked(); }

void SettingsDialog::setEditorList(const QStringList &list)
{
    m_editorList->clear();
    for (const QString &entry : list) {
        const QStringList parts = entry.split('|');
        QString name = parts.size() > 0 ? parts[0] : QFileInfo(entry).fileName();
        QString path = parts.size() > 1 ? parts[1] : entry;
        QString tmpl = parts.size() > 2 ? parts[2] : QStringLiteral("$FILE_PATH");
        auto *item = new QTreeWidgetItem(m_editorList);
        item->setText(0, name);
        item->setText(1, path);
        item->setData(1, Qt::UserRole + 1, tmpl);
    }
}

QStringList SettingsDialog::editorList() const
{
    QStringList list;
    for (int i = 0; i < m_editorList->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_editorList->topLevelItem(i);
        const QString name = item->text(0).trimmed();
        const QString path = item->text(1).trimmed();
        const QString tmpl = item->data(1, Qt::UserRole + 1).toString();
        if (!path.isEmpty())
            list << (name + "|" + path + "|" + tmpl);
    }
    return list;
}

void SettingsDialog::onAddEditor()
{
    const QString p = QFileDialog::getOpenFileName(this, tr("Выбор редактора"));
    if (p.isEmpty()) return;
    auto *item = new QTreeWidgetItem(m_editorList);
    item->setText(0, QFileInfo(p).fileName());
    item->setText(1, p);
    m_editorList->editItem(item, 0);
}

void SettingsDialog::onRemoveEditor()
{
    QTreeWidgetItem *item = m_editorList->currentItem();
    if (item) delete m_editorList->takeTopLevelItem(m_editorList->indexOfTopLevelItem(item));
}

void SettingsDialog::onBrowse()
{
    const QString p = QFileDialog::getExistingDirectory(
        this, tr("Выбор корневой директории"), m_pathEdit->text());
    if (!p.isEmpty())
        m_pathEdit->setText(p);
}

bool SettingsDialog::validateAndApply()
{
    const QString p = rootPath();
    QFileInfo fi(p);
    if (p.isEmpty() || !fi.exists() || !fi.isDir() || !fi.isReadable()) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Путь недоступен для чтения: %1").arg(p));
        return false;
    }

    QSettings s;
    s.setValue(QStringLiteral("rootPath"), p);
    s.setValue(QStringLiteral("showHidden"), showHidden());
    s.setValue(QStringLiteral("editorList"), editorList());

    emit settingsApplied(p, showHidden());
    return true;
}

void SettingsDialog::onEditArgs()
{
    QTreeWidgetItem *item = m_editorList->currentItem();
    if (!item) {
        QMessageBox::information(this, tr("Аргументы"), tr("Сначала выберите редактор в списке."));
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle(tr("Аргументы для %1").arg(item->text(0)));

    auto *help = new QLabel(tr("Доступные замены:\n"
                               "  $FILE_PATH — путь к открываемому файлу\n"
                               "  $LANGUAGE — текущий язык (ru/en)\n"
                               "  $FONT_SIZE — размер шрифта"), &dlg);
    help->setWordWrap(true);
    help->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

    auto *edit = new QLineEdit(item->data(1, Qt::UserRole + 1).toString(), &dlg);
    edit->setPlaceholderText(QStringLiteral("$FILE_PATH"));

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    auto *layout = new QVBoxLayout(&dlg);
    layout->addWidget(help);
    layout->addWidget(edit);
    layout->addWidget(buttons);

    if (dlg.exec() == QDialog::Accepted) {
        item->setData(1, Qt::UserRole + 1, edit->text().trimmed());
    }
}
