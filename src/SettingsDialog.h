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

    void setEditorPath(const QString &p);
    QString editorPath() const;

signals:
    void settingsApplied(const QString &rootPath, bool showHidden);

private slots:
    void onBrowse();

private:
    bool validateAndApply();

    QLineEdit *m_pathEdit = nullptr;
    QLineEdit *m_editorPathEdit = nullptr;
    QCheckBox *m_hidden = nullptr;
};
