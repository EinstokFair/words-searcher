#include "SettingsDialog.h"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Настройки"));
    setModal(true);

    m_pathEdit = new QLineEdit(this);
    auto *browse = new QPushButton(QStringLiteral("Обзор…"), this);
    connect(browse, &QPushButton::clicked, this, &SettingsDialog::onBrowse);

    auto *pathRow = new QHBoxLayout;
    pathRow->addWidget(m_pathEdit, 1);
    pathRow->addWidget(browse);

    m_hidden = new QCheckBox(QStringLiteral("Показывать скрытые"), this);

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("Корневая директория:"), pathRow);
    form->addRow(QString(), m_hidden);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply,
        this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttons->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, [this]() { /* применяется через accept в MainWindow */ });

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
        this, QStringLiteral("Выбор корневой директории"), m_pathEdit->text());
    if (!p.isEmpty())
        m_pathEdit->setText(p);
}
