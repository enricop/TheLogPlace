#ifndef SYSLOGCHANNEL_H
#define SYSLOGCHANNEL_H

#include <Poco/Channel.h>

class LogItemList;

class SyslogChannel: public Poco::Channel
{
public:
    ~SyslogChannel() {}

    virtual void log(const Poco::Message& msg) = 0;

protected:
    LogItemList * m_loglist;
};

class SyslogChannelNew: public SyslogChannel
{
public:
    SyslogChannelNew(LogItemList *loglist) { m_loglist = loglist; }
    void log(const Poco::Message& msg);
};

class SyslogChannelOld: public SyslogChannel
{
public:
    SyslogChannelOld(LogItemList *loglist) { m_loglist = loglist; }
    void log(const Poco::Message& msg);
};

#endif // SYSLOGCHANNEL_H
