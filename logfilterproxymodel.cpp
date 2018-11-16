#include "logfilterproxymodel.h"

LogFilterProxyModel::LogFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

void LogFilterProxyModel::classBegin()
{
}

void LogFilterProxyModel::componentComplete()
{
}

bool LogFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QRegExp rx = filterRegExp();
    if (rx.isEmpty())
        return true;

    QHashIterator<int, QByteArray> it(sourceModel()->roleNames());

    while (it.hasNext()) {
        it.next();
        QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
        if (sourceModel()->data(sourceIndex, it.key()).toString().contains(rx))
            return true;
    }

    return false;
}
