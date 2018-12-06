#include "backend.h"

#include "logitemlist.h"

#include <libssh2.h>
#include <QNetworkInterface>

#ifdef WIN32
    #include <winsock2.h>

    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <sys/select.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

static const unsigned int UDPport = 2000;

Backend::Backend(LogItemList *list, bool mode, QObject *parent) :
    QObject(parent),
    m_mode(mode),
    m_connectionInfo("Disconnesso - Inserire i dati e clikkare per proseguire"),
    m_downloadProgress(0),
    m_downloadSize(0),
    m_sockfd(-1),
    m_sshsession(nullptr),
    m_sshchannel(nullptr),
    m_logs(list)
{
    m_listener = new Poco::Net::RemoteSyslogListener(UDPport);

    if (mode)
        m_channel.reset(new SyslogChannelNew(list));
    else
        m_channel.reset(new SyslogChannelOld(list));

    m_listener->addChannel(m_channel.get());

    connect(&m_logsdownloadWatcher, &QFutureWatcher<void>::started, [=]() {
        emit dlCompletedChanged();
    });
    connect(&m_logsdownloadWatcher, &QFutureWatcher<void>::canceled, [=]() {
        emit dlCompletedChanged();
    });
    connect(&m_logsdownloadWatcher, &QFutureWatcher<void>::finished, [=]() {
        emit dlCompletedChanged();
    });
}

Backend::~Backend()
{
    m_logsdownloadWatcher.cancel();
    m_listener->close();
}

void Backend::openListener()
{
    try {
        m_listener->open();
    }
    catch (Poco::Exception& exc)
    {
        setConInfo(tr("Failed opening syslog UDP listener on port %1: %2").arg(UDPport).arg(QString::fromStdString(exc.displayText())));
    }
}

QString Backend::getConInfo() const {
    return m_connectionInfo;
}
void Backend::setConInfo(QString info) {
    if (info == m_connectionInfo) //avoid update loops
        return;
    m_connectionInfo = info;
    emit coninfoChanged();
}

QString Backend::getBannerInfo() const {
    return m_connectionInfo;
}
void Backend::setBannerInfo(QString info) {
    if (info == m_bannerInfo) //avoid update loops
        return;
    m_bannerInfo = info;
    emit bannerinfoChanged();
}

qint64 Backend::getDlProg() const {
    return m_downloadProgress;
}
void Backend::setDlProg(qint64 prog) {
    if (prog == m_downloadProgress) //avoid update loops
        return;
    m_downloadProgress = prog;
    emit dlProgChanged();
}

qint64 Backend::getDlSize() const {
    return m_downloadSize;
}
void Backend::setDlSize(qint64 size) {
    if (size == m_downloadSize) //avoid update loops
        return;
    m_downloadSize = size;
    emit dlSizeChanged();
}

bool Backend::getDlCompleted()
{
    return !m_logsdownloadWatcher.isRunning();
}

void Backend::sshConnector(const QString ipaddress,
                           const QString username,
                           const QString password)
{
    struct sockaddr_in sin;
    struct in_addr host_addr;
    int rc;

#ifdef WIN32
    WSADATA wsadata;

    rc = WSAStartup(MAKEWORD(2,2), &wsadata);
    if (rc != 0) {
        setConInfo(tr("WSAStartup failed with error: %1").arg(WSAGetLastError()));
        return;
    }
#endif

    if (ipaddress.isEmpty()) {
        setConInfo(tr("Empty IP Address"));
        return;
    }
    rc = inet_pton(AF_INET, ipaddress.toStdString().c_str(), &host_addr);
    if (rc != 1) {
        setConInfo(tr("Invalid IP Address"));
        return;
    }

    if (username.isEmpty()) {
        setConInfo(tr("Empty username"));
        return;
    }
    if (password.isEmpty())
        setConInfo(tr("Using empty password"));

    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = host_addr.S_un.S_addr;

    rc = ::connect(m_sockfd, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in));
    if (rc != 0) {
        setConInfo(tr("Failed enstablishing connection to %1: error %2")
                   .arg(ipaddress)
                   .arg(std::strerror(errno)));
        m_sockfd = -1;
        return;
    }

    rc = libssh2_init(0);
    if (rc != 0) {
        setConInfo(tr("libssh2 initialization failed (%1)").arg(rc));
        sshShutdown();
        return;
    }

    m_sshsession = libssh2_session_init();
    if (!m_sshsession) {
        setConInfo(tr("libssh2 session initialization failed"));
        sshShutdown();
        return;
    }

    /* Since we have set non-blocking, tell libssh2 we are non-blocking */
    libssh2_session_set_blocking(m_sshsession, 0);

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    while ((rc = libssh2_session_handshake(m_sshsession, m_sockfd)) == LIBSSH2_ERROR_EAGAIN);
    if (rc) {
        setConInfo(tr("Failure establishing SSH session: %1").arg(rc));
        sshShutdown();
        return;
    }

    /* We could authenticate via password */
    while ((rc = libssh2_userauth_password(m_sshsession,
                                           username.toStdString().c_str(),
                                           password.toStdString().c_str())) == LIBSSH2_ERROR_EAGAIN);
    if (rc) {
        setConInfo(tr("Authentication with username \"%1\" and provided password failed: %2").arg(username).arg(rc));
        sshShutdown();
        return;
    }

    setConInfo(tr("Authenticated with username \"%1\" and provided password!").arg(username));

    startJob();

    return;
}

void Backend::startJob() {
    m_logs->reset();

    if (m_mode) {
        sshExecutor("sed -i~ '46i *.* @" + getIPAddress() + ":" + QString::number(UDPport) + "' /etc/rsyslog.conf");
        sshExecutor("/etc/init.d/syslog restart");
        sshShutdown();
        openListener();
    } else {
        QFuture<void> future = QtConcurrent::run(this, &Backend::sshDownloader, QString("/var/log/messages"));
        m_logsdownloadWatcher.setFuture(future);
    }
}

void Backend::sshShutdown() {
    libssh2_channel_free(m_sshchannel);
    m_sshchannel = nullptr;

    libssh2_session_disconnect(m_sshsession, "Normal Shutdown, Thank you for playing");
    libssh2_session_free(m_sshsession);
    m_sshsession = nullptr;

#ifdef WIN32
    closesocket(m_sockfd);
    WSACleanup();
#else
    close(m_sockfd);
#endif
    m_sockfd = -1;

    libssh2_exit();
}

static int waitsocket(int socket_fd, LIBSSH2_SESSION *session)
{
    struct timeval timeout;
    fd_set fd;
    fd_set *writefd = nullptr;
    fd_set *readfd = nullptr;
    int dir;

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    FD_ZERO(&fd);

    FD_SET(socket_fd, &fd);

    /* now make sure we wait in the correct direction */
    dir = libssh2_session_block_directions(session);

    if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
        readfd = &fd;

    if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
        writefd = &fd;

    return select(socket_fd + 1, readfd, writefd, nullptr, &timeout);;
}

void Backend::sshDownloader(const QString filename) {
    setConInfo(tr("Downloading file %1!").arg(filename));

    /* Request a file via SCP */
    libssh2_struct_stat fileinfo;
    do {
        m_sshchannel = libssh2_scp_recv2(m_sshsession,
                                        filename.toStdString().c_str(),
                                        &fileinfo);

        if (!m_sshchannel) {
            if(libssh2_session_last_errno(m_sshsession) != LIBSSH2_ERROR_EAGAIN) {
                char *err_msg;
                libssh2_session_last_error(m_sshsession, &err_msg, nullptr, 0);
                setConInfo(tr("Error requesting SCP of file %1: %2").arg(filename).arg(err_msg));
                sshShutdown();
                return;
            }
            else {
                setConInfo(tr("Requesting SCP of file %1: spin").arg(filename));
                waitsocket(m_sockfd, m_sshsession);
            }
        }
    } while (!m_sshchannel);

    setConInfo(tr("Requested a copy of file %1, now receive data!").arg(filename));

    std::string oldlogbuffer;

    setDlSize(fileinfo.st_size);
    libssh2_struct_stat_size got = 0;
    setDlProg(got);

    while(got < fileinfo.st_size) {
        char mem[1024*128 + 2];
        int rc = 0;

        do {
            size_t amount = sizeof(mem) - 2;

            if ((fileinfo.st_size - got) < amount)
                amount = fileinfo.st_size - got;

            /* loop until we block */
            rc = libssh2_channel_read(m_sshchannel, mem, amount);
            if (rc > 0) {
                got += rc;
                setDlProg(got);
                processOldMessages(mem, rc, oldlogbuffer);
            }
        } while (rc >= 0);

        if (rc != LIBSSH2_ERROR_EAGAIN) {
            break;
        }

        /* this is due to blocking that would occur otherwise
        so we loop on this condition */
        waitsocket(m_sockfd, m_sshsession); /* now we wait */
    }

    m_logs->outputdata();

    if (got < fileinfo.st_size) {
        setConInfo(tr("Error during downloading of file %1").arg(filename));
    } else {
        setConInfo(tr("Processing of file %1 completed!").arg(filename));
    }

    sshShutdown();
}

void Backend::sshExecutor(const QString command) {
    /* Exec non-blocking on the remote host */
    while( (m_sshchannel = libssh2_channel_open_session(m_sshsession)) == nullptr &&
           libssh2_session_last_error(m_sshsession, nullptr, nullptr, 0) == LIBSSH2_ERROR_EAGAIN )
    {
        waitsocket(m_sockfd, m_sshsession);
    }
    if( m_sshchannel == nullptr )
    {
        setConInfo(tr("libssh2_channel_open_session error"));
        sshShutdown();
    }
    int rc;
    while( (rc = libssh2_channel_exec(m_sshchannel, command.toStdString().c_str())) == LIBSSH2_ERROR_EAGAIN )
    {
        waitsocket(m_sockfd, m_sshsession);
    }
    if( rc != 0 )
        setConInfo(tr("libssh2_channel_exec error: %1").arg(rc));

    int exitcode = 127;
    while( (rc = libssh2_channel_close(m_sshchannel)) == LIBSSH2_ERROR_EAGAIN )
        waitsocket(m_sockfd, m_sshsession);

    if( rc == 0 )
        exitcode = libssh2_channel_get_exit_status(m_sshchannel);

    if (exitcode == 0)
        setConInfo(tr("Command %1 executed successfully").arg(command));
    else
        setConInfo(tr("Error executing command %1: %2").arg(command).arg(rc));
}

void Backend::processOldMessages(char *buffer, const int len, std::string& oldlogbuffer) {
    if (buffer == nullptr || len <= 0)
        return;

    buffer[len] = '\n';
    buffer[len+1] = '\0';

    const char *begin = buffer;
    char *end = buffer;

    while ((end = std::strchr(end, '\n')) != nullptr) {
        *end = '\0';
        oldlogbuffer.append(begin);

        if (buffer + len != end) { //We have an endline - a complete log!
            validateAndHandleOldMessage(oldlogbuffer);
        }

        begin = ++end;
    }

    return;
}

void Backend::validateAndHandleOldMessage(std::string& oldlogbuffer) {
    std::string::size_type i = 0;

    int j;
    for (j = 1; j < 4 && i != std::string::npos; j++) //Find third space - beginning of text
        i = oldlogbuffer.find(' ', i+1);

    if (j == 4 &&
        i != 0 &&
        i != std::string::npos &&
        oldlogbuffer.at(i-1) == ':')  //Preceeding char should be ':'
    {
        try {
            if (oldlogbuffer.at(i+1) == '[') // Disable parsing of 'structured data'
                oldlogbuffer.insert(i+1, "-");
        } catch (...) {
            std::cout << "Log with empty message text: " << oldlogbuffer << std::endl;
        }

        oldlogbuffer.insert(i-1, " X "); // PROCID SP MSGID
        oldlogbuffer.insert(0, "<15>15 "); // <int>: priority, severity, facility
        try {
            m_listener->processMessage(oldlogbuffer);
        } catch (...) {
            std::cout << "Failed parsing: " << oldlogbuffer << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid or truncated log: " << oldlogbuffer << std::endl;
    }

    oldlogbuffer.clear();
}

QString Backend::getIPAddress() {
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            return address.toString();
    }
    return QString();
}
