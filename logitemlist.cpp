#include "logitemlist.h"

LogItemList::LogItemList(QObject *parent) : QObject(parent), mItemsList()
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

void LogItemList::reset()
{
    emit preClear();

    mItemsList.clear();

    emit postClear();
}
