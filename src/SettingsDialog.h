#pragma once
#include <QDialog>

class QLineEdit;
class QCheckBox;

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    void setRootPath(const QString &p);
    QString rootPath() const;
    void setShowHidden(bool on);
    bool showHidden() const;

private slots:
    void onBrowse();

private:
    QLineEdit *m_pathEdit = nullptr;
    QCheckBox *m_hidden = nullptr;
};
