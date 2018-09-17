//
// SyslogTest.cpp
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/Net/RemoteSyslogChannel.h"
#include "Poco/Net/RemoteSyslogListener.h"
#include "Poco/Thread.h"
#include "Poco/Message.h"
#include "Poco/AutoPtr.h"
#include <list>

using namespace Poco::Net;


class InputChannel: public Poco::Channel
{
public:
    InputChannel() {}
    ~InputChannel() {}

    void log(const Poco::Message& msg);
};

void InputChannel::log(const Poco::Message& msg)
{

}

void testStructuredData()
{
    //Poco::AutoPtr<RemoteSyslogChannel> channel = new RemoteSyslogChannel();
    //channel->setProperty("loghost", "127.0.0.1:51400");
    //channel->open();
    Poco::AutoPtr<RemoteSyslogListener> listener = new RemoteSyslogListener();
    listener->open();
    InputChannel *cl = new InputChannel;
    listener->addChannel(cl);

    Poco::Message msg1("asource", "amessage", Poco::Message::PRIO_CRITICAL);
    //msg1.set("structured-data", "[exampleSDID@32473 iut=\"3\" eventSource=\"Application\" eventID=\"1011\"]");
    //channel->log(msg1);
    Poco::Message msg2("asource", "amessage", Poco::Message::PRIO_CRITICAL);
    //msg2.set("structured-data", "[exampleSDID@32473 iut=\"3\" eventSource=\"Application\" eventID=\"1011\"][examplePriority@32473 class=\"high\"]");
    //channel->log(msg2);
    //Poco::Thread::sleep(1000);
    listener->close();
    //channel->close();

    std::vector<Poco::Message> msgs;
    //cl->getMessages(msgs, 0, 10);

    /*
    assertTrue (msgs[0].getSource() == "asource");
    assertTrue (msgs[0].getText() == "amessage");
    assertTrue (msgs[0].getPriority() == Poco::Message::PRIO_CRITICAL);
    assertTrue (msgs[0].get("structured-data") == "[exampleSDID@32473 iut=\"3\" eventSource=\"Application\" eventID=\"1011\"][examplePriority@32473 class=\"high\"]");

    assertTrue (msgs[1].getSource() == "asource");
    assertTrue (msgs[1].getText() == "amessage");
    assertTrue (msgs[1].getPriority() == Poco::Message::PRIO_CRITICAL);
    assertTrue (msgs[1].get("structured-data") == "[exampleSDID@32473 iut=\"3\" eventSource=\"Application\" eventID=\"1011\"]");
    */
}
