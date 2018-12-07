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

    return app.exec();
}
