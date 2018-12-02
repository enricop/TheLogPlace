QT += quick
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
    loglistmodel.cpp \
    logitemlist.cpp \
    logfilterproxymodel.cpp \
    syslogchannel.cpp \
    backend.cpp

# Poco
win32: LIBS += -L$$PWD/'../../../../Program Files/Poco/VS2017/1.9.0/lib/'
INCLUDEPATH += $$PWD/'../../../../Program Files/Poco/VS2017/1.9.0/inc'
DEPENDPATH += $$PWD/'../../../../Program Files/Poco/VS2017/1.9.0/inc'

#Libssh2
win32: LIBS += -L$$PWD/../libssh2/dll/lib/
INCLUDEPATH += $$PWD/../libssh2/dll/include
DEPENDPATH += $$PWD/../libssh2/dll/include

LIBS += -llibssh2

win32: LIBS += -lws2_32

RESOURCES += qml.qrc

HEADERS += \
    loglistmodel.h \
    logitem.h \
    logitemlist.h \
    logfilterproxymodel.h \
    syslogchannel.h \
    backend.h
