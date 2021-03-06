#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QtConcurrent/QtConcurrent>

#include <Poco/Net/RemoteSyslogListener.h>
#include <Poco/AutoPtr.h>

#include "syslogchannel.h"

#include <memory>

class LogItemList;
typedef struct _LIBSSH2_SESSION LIBSSH2_SESSION;
typedef struct _LIBSSH2_CHANNEL LIBSSH2_CHANNEL;

class Backend : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString connectionInfo READ getConInfo WRITE setConInfo NOTIFY coninfoChanged)
    Q_PROPERTY(QString bannerInfo READ getBannerInfo WRITE setBannerInfo NOTIFY bannerinfoChanged)
    Q_PROPERTY(qint64 downloadProgress READ getDlProg WRITE setDlProg NOTIFY dlProgChanged)
    Q_PROPERTY(qint64 downloadSize READ getDlSize WRITE setDlSize NOTIFY dlSizeChanged)
    Q_PROPERTY(bool downloadCompleted READ getDlCompleted NOTIFY dlCompletedChanged)

public:
    explicit Backend(LogItemList *list, bool mode, QObject *parent = nullptr);
    ~Backend();

    QString getConInfo() const;
    void setConInfo(QString info);
    QString getBannerInfo() const;
    void setBannerInfo(QString info);
    qint64 getDlProg() const;
    void setDlProg(qint64 info);
    qint64 getDlSize() const;
    void setDlSize(qint64 info);

    bool getDlCompleted();

    Q_INVOKABLE void sshConnector(const QString ipaddress, const QString username, const QString password);

signals:
     void coninfoChanged();
     void bannerinfoChanged();
     void dlProgChanged();
     void dlSizeChanged();
     void dlCompletedChanged();

public slots:

private:
    void openListener();

    void sshDownloader(const QString filename);
    void sshExecutor(const QString command);
    void sshShutdown();

    void processOldMessages(char *buffer, const int len, std::string &oldlogbuffer);
    void validateAndHandleOldMessage(std::string& oldlogbuffer);

    QString getIPAddress();

    void startJob();

    bool m_mode; //Download old messages (false) OR Receive new messages (true)

    QString m_connectionInfo;
    QString m_bannerInfo;
    qint64 m_downloadProgress;
    qint64 m_downloadSize;

    QFutureWatcher<void> m_logsdownloadWatcher;

    int m_sockfd;
    LIBSSH2_SESSION *m_sshsession;
    LIBSSH2_CHANNEL *m_sshchannel;

    Poco::AutoPtr<Poco::Net::RemoteSyslogListener> m_listener;
    std::unique_ptr<SyslogChannel> m_channel;
    LogItemList * m_logs;
};

#endif // BACKEND_H
