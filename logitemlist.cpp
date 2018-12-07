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

void LogItemList::appendItemOld(const LogItem &item)
{
    mItemsList.append(item);
}

void LogItemList::appendItemNew(const LogItem &item)
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

void LogItemList::outputdata()
{
    emit addAllItems();
}

void LogItemList::lockList()
{
    mNewItemMtx.lock();
}

void LogItemList::unlockList()
{
    mNewItemMtx.unlock();
}
