#pragma once
#include <QIdentityProxyModel>

class FsProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    enum Column { ColIcon = 0, ColDate = 1, ColName = 2, ColSize = 3, ColCount = 4 };

    explicit FsProxyModel(QObject *parent = nullptr);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

private:
    static QString humanSize(qint64 bytes);
};
