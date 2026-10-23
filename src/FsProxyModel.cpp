#include "FsProxyModel.h"
#include <QDateTime>
#include <QFileInfo>
#include <QFileSystemModel>

FsProxyModel::FsProxyModel(QObject *parent) : QIdentityProxyModel(parent) {}

int FsProxyModel::columnCount(const QModelIndex &) const { return ColCount; }

QVariant FsProxyModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();
    switch (section) {
    case ColIcon: return QString();
    case ColDate: return QStringLiteral("Дата");
    case ColName: return QStringLiteral("Имя");
    case ColSize: return QStringLiteral("Размер");
    }
    return QVariant();
}

QString FsProxyModel::humanSize(qint64 bytes)
{
    if (bytes < 1024)
        return QStringLiteral("%1 B").arg(bytes);
    static const char *units[] = {"KiB", "MiB", "GiB", "TiB"};
    double v = bytes / 1024.0;
    int i = 0;
    while (v >= 1024.0 && i < 3) { v /= 1024.0; ++i; }
    return QStringLiteral("%1 %2").arg(v, 0, 'f', 1).arg(units[i]);
}

QVariant FsProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    auto *fs = qobject_cast<QFileSystemModel *>(sourceModel());
    if (!fs)
        return QIdentityProxyModel::data(index, role);

    const QModelIndex src = mapToSource(index);
    const QFileInfo fi = fs->fileInfo(src);

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case ColIcon: return QVariant();
        case ColDate: return fi.lastModified().toString(QStringLiteral("yyyy-MM-dd"));
        case ColName: return fi.fileName().isEmpty() ? fi.absoluteFilePath() : fi.fileName();
        case ColSize: return fi.isDir() ? QVariant() : humanSize(fi.size());
        }
    }
    if (role == Qt::DecorationRole && index.column() == ColIcon)
        return fs->data(src, Qt::DecorationRole);
    if (role == Qt::TextAlignmentRole) {
        if (index.column() == ColSize)
            return int(Qt::AlignRight | Qt::AlignVCenter);
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    return QVariant();
}
