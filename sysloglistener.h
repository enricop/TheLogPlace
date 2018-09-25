#ifndef SYSLOGLISTENER_H
#define SYSLOGLISTENER_H

#include <Poco/Channel.h>

class LogItemList;

class InputChannel: public Poco::Channel
{
public:
    InputChannel(LogItemList *loglist) : m_loglist(loglist) {}
    ~InputChannel() {}

    void log(const Poco::Message& msg);

private:
    LogItemList * m_loglist;
};
#endif // SYSLOGLISTENER_H
