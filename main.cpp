#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "logitemlist.h"
#include "loglistmodel.h"

#include "sysloglistener.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<LogListModel>("TheLogPlace", 1, 0, "LogListModel");
    qmlRegisterUncreatableType<LogItemList>("TheLogPlace", 1, 0, "LogItemList",
        QStringLiteral("ToDoList should not be created in QML"));

    LogItemList logs;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("logItemList"), &logs);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    testStructuredData();

    return app.exec();
}
