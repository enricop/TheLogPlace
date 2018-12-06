#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "logitemlist.h"
#include "loglistmodel.h"
#include "logfilterproxymodel.h"
#include "backend.h"

#include "syslogchannel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<LogListModel>("TheLogPlace", 1, 0, "LogListModel");
    qmlRegisterType<LogFilterProxyModel>("TheLogPlace", 1, 0, "LogFilterProxyModel");
    qmlRegisterUncreatableType<LogItemList>("TheLogPlace", 1, 0, "LogItemList",
        QStringLiteral("LogItemList should not be created in QML"));

    LogItemList oldlogs;
    LogItemList newlogs;

    Backend oldlogsbackend(&oldlogs, false);
    Backend newlogsbackend(&newlogs, true);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("oldlogs"), &oldlogs);
    engine.rootContext()->setContextProperty(QStringLiteral("newlogs"), &newlogs);

    engine.rootContext()->setContextProperty(QStringLiteral("oldlogsbackend"), &oldlogsbackend);
    engine.rootContext()->setContextProperty(QStringLiteral("newlogsbackend"), &newlogsbackend);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    /*
    Poco::Message msg1("source1", "message1", Poco::Message::PRIO_CRITICAL);
    cl->log(msg1);
    Poco::Message msg2("source2", "message2", Poco::Message::PRIO_CRITICAL);
    cl->log(msg2);

    std::string tmp("<15>15 2018-11-29T16:29:48.557292+01:00 AG-ea9ad8642c26b1d4 CloudManager[629] : : client (0x74e00908) received data:"
                    " \"{\"error\":0,\"function\":\"keepalive\",\"msgid\":\"55271ef6-4e23-4c12-87f4-8308c85091f5\","
                    "result\":[],\"source\":\"ea9ad8642c26b1d4\",\"target\":\"CloudClient\",\"type\":\"response\"}#012\".");
    listener->processMessage(tmp);
    */

    return app.exec();
}
