#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP

#ifdef _WIN32
    #define _WIN32_WINNT 0x0601
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef int socklen_t;
    typedef SOCKET socket_t;
    #define CLOSE_SOCKET(s) closesocket(s)
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <fcntl.h>
    typedef int socket_t;
    #define INVALID_SOCKET (-1)
    #define CLOSE_SOCKET(s) close(s)
#endif

class SocketServer {
public:
    SocketServer(int port);
    ~SocketServer();

    bool start();
    void listenForLogs();

private:
    int m_port;
    socket_t m_server_fd;
    bool m_is_running;

    void setNonBlocking(socket_t sock);
};

#endif // SOCKET_SERVER_HPP
