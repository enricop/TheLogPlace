#ifndef LOGITEMLIST_H
#define LOGITEMLIST_H

#include <QObject>
#include <QMutex>

#include "logitem.h"

class LogItemList : public QObject
{
    Q_OBJECT

public:
    explicit LogItemList(QObject *parent = nullptr);

    int size() const;
    const LogItem& at(int i) const;

    void appendItemNew(const LogItem &item);
    void appendItemOld(const LogItem &item);

    void reset();
    void outputdata();

    void lockList();
    void unlockList();

signals:
    void preItemAppended();
    void postItemAppended();
    void addAllItems();

    void preClear();
    void postClear();

private:
    QList<LogItem> mItemsList;

    QMutex mNewItemMtx;
};

#endif // LOGITEMLIST_H
