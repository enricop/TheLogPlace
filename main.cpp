#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <Poco/Net/RemoteSyslogListener.h>
#include <Poco/AutoPtr.h>
#include <Poco/Message.h>

#include <Poco/Exception.h>
#include <iostream>

#include "logitemlist.h"
#include "loglistmodel.h"
#include "logfilterproxymodel.h"

#include "syslogchannel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<LogListModel>("TheLogPlace", 1, 0, "LogListModel");
    qmlRegisterType<LogFilterProxyModel>("TheLogPlace", 1, 0, "LogFilterProxyModel");
    qmlRegisterUncreatableType<LogItemList>("TheLogPlace", 1, 0, "LogItemList",
        QStringLiteral("LogItemList should not be created in QML"));

    LogItemList logs;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("logItemList"), &logs);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    Poco::AutoPtr<Poco::Net::RemoteSyslogListener> listener = new Poco::Net::RemoteSyslogListener(2000);
    try {
        listener->open();
    }
    catch (Poco::Exception& exc)
    {
        std::cerr << exc.displayText() << std::endl;
    }

    SyslogChannel *cl = new SyslogChannel(&logs);
    listener->addChannel(cl);

    Poco::Message msg1("source1", "message1", Poco::Message::PRIO_CRITICAL);
    cl->log(msg1);
    Poco::Message msg2("source2", "message2", Poco::Message::PRIO_CRITICAL);
    cl->log(msg2);
    int ret = app.exec();

    listener->close();
    delete cl;

    return ret;
}
