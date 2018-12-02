#include "backend.h"

#include "logitemlist.h"

#include <libssh2.h>

#ifdef WIN32
    #include <winsock2.h>

    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <sys/select.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

Backend::Backend(LogItemList *list, QObject *parent) :
    QObject(parent),
    m_connectionInfo("Disconnesso - Inserire un indirizzo IP e clikkare su OK"),
    m_downloadProgress(0),
    m_downloadSize(0),
    m_sockfd(-1),
    m_sshsession(nullptr),
    m_sshchannel(nullptr),
    m_logbuffer()
{
    m_listener = new Poco::Net::RemoteSyslogListener(2000);

    m_channel.reset(new SyslogChannel(list));

    m_listener->addChannel(m_channel.get());
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

    if (::connect(m_sockfd, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) {
        setConInfo(tr("Failed enstablishing connection to %1").arg(ipaddress));
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

    sshDownloader();
    sshShutdown();
    return;
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
}

static int waitsocket(int socket_fd, LIBSSH2_SESSION *session)
{
    struct timeval timeout;
    int rc;
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

    rc = select(socket_fd + 1, readfd, writefd, nullptr, &timeout);

    return rc;
}

void Backend::sshDownloader() {
    libssh2_struct_stat fileinfo;

    QString filename("messages");

    setConInfo(tr("Downloading file %1!").arg(filename));

    /* Request a file via SCP */
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

    setConInfo(tr("Requesting SCP of file %1 is done, now receive data!").arg(filename));

    setDlSize(fileinfo.st_size);

    libssh2_struct_stat_size got = 0;
    setDlProg(got);

    while(got < fileinfo.st_size) {
        char mem[1024*128 + 2];
        int rc = 0;

        do {
            size_t amount = sizeof(mem)-1;

            if ((fileinfo.st_size - got) < amount)
                amount = fileinfo.st_size - got;

            /* loop until we block */
            rc = libssh2_channel_read(m_sshchannel, mem, amount);
            if (rc > 0) {
                got += rc;
                setDlProg(got);
                processMessages(mem, rc);
            }
        } while (rc >= 0);

        if (rc != LIBSSH2_ERROR_EAGAIN) {
            break;
        }

        /* this is due to blocking that would occur otherwise
        so we loop on this condition */
        waitsocket(m_sockfd, m_sshsession); /* now we wait */
    }

    if (got < fileinfo.st_size) {
        setConInfo(tr("Error downloading whole file %1").arg(filename));
    } else {
        setConInfo(tr("Downloaded file %1!").arg(filename));
    }
}

void Backend::processMessages(char *buffer, const int len) {
    if (buffer == nullptr || len <= 0)
        return;

    buffer[len] = '\n';
    buffer[len+1] = '\0';

    const char *begin = buffer;
    char *end = buffer;

    while ((end = std::strchr(end, '\n')) != nullptr) {
        *end = '\0';
        m_logbuffer.append(begin);

        if (buffer + len != end) { //We have an endline - a complete log!
            validateAndHandleInputMessage();
        }

        begin = ++end;
    }

    return;
}

void Backend::validateAndHandleInputMessage() {
    std::string::size_type i = 0;

    for (int j = 1; j < 4 && i != std::string::npos; j++)
        i = m_logbuffer.find(' ', i+1);

    if (i == 0 || i == std::string::npos || m_logbuffer.at(i-1) != ':') {
        std::cout << "Invalid or truncated log: " << m_logbuffer << std::endl;
    } else {
        m_logbuffer.insert(i-1, " X ");
        m_logbuffer.insert(0, "<15>15 ");
        m_listener->processMessage(m_logbuffer);
    }

    m_logbuffer.clear();
}
