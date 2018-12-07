#ifndef LOGITEM_H
#define LOGITEM_H

#include <QString>

struct LogItem
{
    QString m_timestamp;
    QString m_processname;
    QString m_message;
};

#endif // LOGITEM_H
