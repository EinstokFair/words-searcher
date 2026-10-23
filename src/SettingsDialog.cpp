#include "SettingsDialog.h"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
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

    m_editorPathEdit = new QLineEdit(this);
    auto *browseEditor = new QPushButton(tr("Обзор…"), this);
    connect(browseEditor, &QPushButton::clicked, this, [this]() {
        const QString p = QFileDialog::getOpenFileName(this, tr("Выбор редактора"), m_editorPathEdit->text());
        if (!p.isEmpty()) m_editorPathEdit->setText(p);
    });
    auto *editorRow = new QHBoxLayout;
    editorRow->addWidget(m_editorPathEdit, 1);
    editorRow->addWidget(browseEditor);

    m_hidden = new QCheckBox(tr("Показывать скрытые"), this);

    auto *form = new QFormLayout;
    form->addRow(tr("Корневая директория:"), pathRow);
    form->addRow(tr("Редактор:"), editorRow);
    form->addRow(QString(), m_hidden);

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

void SettingsDialog::onBrowse()
{
    const QString p = QFileDialog::getExistingDirectory(
        this, tr("Выбор корневой директории"), m_pathEdit->text());
    if (!p.isEmpty())
        m_pathEdit->setText(p);
}

void SettingsDialog::setEditorPath(const QString &p) { m_editorPathEdit->setText(p); }
QString SettingsDialog::editorPath() const { return m_editorPathEdit->text().trimmed(); }

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
    s.setValue(QStringLiteral("editorPath"), editorPath());

    emit settingsApplied(p, showHidden());
    return true;
}
