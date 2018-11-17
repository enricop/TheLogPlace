#include "logitemlist.h"

LogItemList::LogItemList(QObject *parent) : QObject(parent)
{
}

int LogItemList::size() const {
    return mItemsList.size();
}

const LogItem& LogItemList::at(int i) const {
    return mItemsList.at(i);
}

void LogItemList::appendItem(const LogItem &item)
{
    emit preItemAppended();

    mItemsList.append(item);

    emit postItemAppended();
}
