#include <Poco/Message.h>
#include <Poco/DateTimeFormatter.h>

#include "syslogchannel.h"
#include "logitemlist.h"

void SyslogChannelNew::log(const Poco::Message& msg)
{

    std::string msgtext = msg.getText();
    const std::string::size_type i = msgtext.find(": ");
    std::string msgproc = "NoProc";
    if (i != std::string::npos)
        msgproc = msgtext.substr(0, i);
        msgtext = msgtext.substr(i + 2);

    const LogItem item {
        QString::fromStdString(Poco::DateTimeFormatter::format(msg.getTime(), "%m/%d %H:%M:%S")),
        QString::fromStdString(msgproc),
        QString::fromStdString(msgtext)
    };

    m_loglist->lockList();

    m_loglist->appendItemNew(item);
}

void SyslogChannelOld::log(const Poco::Message& msg)
{
    const LogItem item {
        QString::fromStdString(Poco::DateTimeFormatter::format(msg.getTime(), "%m/%d %H:%M:%S")),
        QString::fromStdString(msg.get("app", "NoProc")),
        QString::fromStdString(msg.getText())
    };

    m_loglist->appendItemOld(item);
}
