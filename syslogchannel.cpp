#include <Poco/Message.h>
#include <Poco/DateTimeFormatter.h>

#include "syslogchannel.h"
#include "logitemlist.h"

void SyslogChannel::log(const Poco::Message& msg)
{
    const LogItem item {
        QString::fromStdString(Poco::DateTimeFormatter::format(msg.getTime(), "%m/%d %H:%M:%S")),
        QString::fromStdString(msg.get("app", "NoProc")),
        QString::fromStdString(msg.getText())
    };

    m_loglist->appendItem(item);
}
