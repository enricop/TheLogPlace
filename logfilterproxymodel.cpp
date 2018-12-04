#include "logfilterproxymodel.h"

#include "loglistmodel.h"

LogFilterProxyModel::LogFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

QObject *LogFilterProxyModel::source() const
{
    return sourceModel();
}

void LogFilterProxyModel::setSource(QObject *source)
{
    setSourceModel(qobject_cast<QAbstractItemModel *>(source));
}


QString LogFilterProxyModel::filterString() const
{
    return m_logTextFilterString;
}

void LogFilterProxyModel::setFilterString(const QString &filter)
{
    m_logTextFilterString = filter;
    setFilterRegExp(QRegExp(filter, filterCaseSensitivity(), QRegExp::PatternSyntax::FixedString));
}

/* Invoked after class creation, but before any properties have been set. */
void LogFilterProxyModel::classBegin()
{
}

/* Invoked after the root component that caused this instantiation has completed construction.
 * At this point all static values and binding values have been assigned to the class. */
void LogFilterProxyModel::componentComplete()
{
}

bool LogFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (m_logTextFilterString.isEmpty())
        return true;

    //QHashIterator<int, QByteArray> it(sourceModel()->roleNames());

    //while (it.hasNext()) {
    //    it.next();
        const QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
        return sourceModel()->data(sourceIndex, LogListModel::MessageRole).toString().contains(m_logTextFilterString, Qt::CaseSensitivity::CaseInsensitive);
    //        return true;
    //}

    //return false;
}
