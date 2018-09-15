#include "logitemlist.h"

LogItemList::LogItemList(QObject *parent) : QObject(parent)
{
    mItemsList.append({ QStringLiteral("Wdsadas"), QStringLiteral("Wash the car") });
    mItemsList.append({ QStringLiteral("321dsadWdsadas"), QStringLiteral("Wash tfdafdshe car") });
}

int LogItemList::size() const {
    return mItemsList.size();
}

const LogItem& LogItemList::at(int i) const {
    return mItemsList.at(i);
}

void LogItemList::appendItem(const LogItem item)
{
    emit preItemAppended();

    mItemsList.append(item);

    emit postItemAppended();
}
