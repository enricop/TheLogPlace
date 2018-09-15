#ifndef LOGITEMLIST_H
#define LOGITEMLIST_H

#include <QObject>

#include "logitem.h"

class LogItemList : public QObject
{
    Q_OBJECT

public:
    explicit LogItemList(QObject *parent = nullptr);

    int size() const;
    const LogItem& at(int i) const;

signals:
    void preItemAppended();
    void postItemAppended();

public slots:
    void appendItem(const LogItem item);

private:
    QList<LogItem> mItemsList;
};


#endif // LOGITEMLIST_H
