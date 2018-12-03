#ifndef LOGITEM_H
#define LOGITEM_H

#include <QString>

struct LogItem
{
    const QString m_timestamp;
    const QString m_processname;
    const QString m_message;
};

#endif // LOGITEM_H
