#include <Poco/Message.h>

#include "sysloglistener.h"
#include "logitemlist.h"

void InputChannel::log(const Poco::Message& msg)
{
    const LogItem item {
        QString::fromStdString(msg.get("app")),
        QString::fromStdString(msg.getText())
    };

    m_loglist->appendItem(item);
}
