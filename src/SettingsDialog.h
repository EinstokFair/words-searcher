#pragma once
#include <QDialog>

class QLineEdit;
class QTreeWidget;
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

    void setEditorList(const QStringList &list);
    QStringList editorList() const;

signals:
    void settingsApplied(const QString &rootPath, bool showHidden);

private slots:
    void onBrowse();
    void onAddEditor();
    void onRemoveEditor();
    void onEditArgs();

private:
    bool validateAndApply();

    QLineEdit *m_pathEdit = nullptr;
    QTreeWidget *m_editorList = nullptr;
    QCheckBox *m_hidden = nullptr;
};
