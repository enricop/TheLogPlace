#include "loglistmodel.h"

#include "logitemlist.h"

#include <iostream>

LogListModel::LogListModel(QObject *parent)
    : QAbstractListModel(parent)
    , mlogs(nullptr)
{
}

int LogListModel::rowCount(const QModelIndex & parent) const {
    if (parent.isValid() || !mlogs)
        return 0;

    return mlogs->size();
}

QVariant LogListModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid() || index.row() >= mlogs->size())
        return QVariant();

    const LogItem log = mlogs->at(index.row());

    switch (role) {
    case TimestampRole:
        return log.m_timestamp;
    case ProcessNameRole:
        return log.m_processname;
    case MessageRole:
        return log.m_message;
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags LogListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsSelectable;
}

QHash<int, QByteArray> LogListModel::roleNames() const {
    static const QHash<int, QByteArray> roles {
        {ProcessNameRole, "processname"},
        {MessageRole, "message"},
        {TimestampRole, "timestamp"}
    };
    return roles;
}

LogItemList *LogListModel::list() const
{
    return mlogs;
}

void LogListModel::setList(LogItemList *list)
{
    beginResetModel();

    if (mlogs)
        mlogs->disconnect(this);

    mlogs = list;

    if (mlogs) {
        connect(mlogs, &LogItemList::preItemAppended, this, [=]() {
            const int newindex = mlogs->size() - 1;
            std::cout << newindex << std::endl;
            beginInsertRows(QModelIndex(), newindex, newindex);
        });
        connect(mlogs, &LogItemList::postItemAppended, this, [=]() {
            endInsertRows();
            mlogs->unlockList();
        });
        connect(mlogs, &LogItemList::preClear, this, [=]() {
            beginResetModel();
        });
        connect(mlogs, &LogItemList::postClear, this, [=]() {
            endResetModel();
        });
        connect(mlogs, &LogItemList::addAllItems, this, [=]() {
            beginInsertRows(QModelIndex(), 0, mlogs->size() - 1);
            endInsertRows();
        });
    }

    endResetModel();
}
