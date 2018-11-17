#ifndef SYSLOGLISTENER_H
#define SYSLOGLISTENER_H

#include <Poco/Channel.h>

class LogItemList;

class SyslogChannel: public Poco::Channel
{
public:
    SyslogChannel(LogItemList *loglist) : m_loglist(loglist) {}
    ~SyslogChannel() {}

    void log(const Poco::Message& msg);

private:
    LogItemList * m_loglist;
};
#endif // SYSLOGLISTENER_H
