#include "SocketServer.hpp"
#include <iostream>
#include <cstring>
#include <vector>

#ifdef _WIN32
    static bool g_winsock_initialized = false;
#endif

SocketServer::SocketServer(int port) 
    : m_port(port), m_server_fd(INVALID_SOCKET), m_is_running(false) {}

SocketServer::~SocketServer() {
    if (m_server_fd != INVALID_SOCKET) {
        CLOSE_SOCKET(m_server_fd);
    }
#ifdef _WIN32
    if (g_winsock_initialized) {
        WSACleanup();
    }
#endif
}

bool SocketServer::start() {
#ifdef _WIN32
    if (!g_winsock_initialized) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "[ERROR] Winsock初期化失敗。" << std::endl;
            return false;
        }
        g_winsock_initialized = true;
    }
#endif

    m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_fd == INVALID_SOCKET) {
        std::cerr << "[ERROR] ソケット作成失敗。" << std::endl;
        return false;
    }

    int opt = 1;
#ifdef _WIN32
    setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(m_port);

#ifdef _WIN32
    if (bind(m_server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
#else
    if (bind(m_server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
#endif
        std::cerr << "[ERROR] ポート " << m_port << " のバインドに失敗しました。" << std::endl;
        return false;
    }

    if (listen(m_server_fd, 10) < 0) {
        std::cerr << "[ERROR] リッスン開始失敗。" << std::endl;
        return false;
    }

    std::cout << "[SUCCESS] サーバーがポート " << m_port << " で正常に起動しました。" << std::endl;
    m_is_running = true;
    return true;
}

void SocketServer::setNonBlocking(socket_t sock) {
#ifdef _WIN32
    unsigned long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);
#else
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
#endif
}

void SocketServer::listenForLogs() {
    std::cout << "[INFO] 🛡️ Slowloris防衛壁アクティブ。3秒のパケットタイムアウトを適用します。" << std::endl;
    std::cout << "[INFO] ログの受信待ち受けを開始します（Ctrl+C で停止）..." << std::endl;

    while (m_is_running) {
        sockaddr_in client_addr{};
        socklen_t addr_len = sizeof(client_addr);
        
        socket_t client_fd = accept(m_server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd == INVALID_SOCKET) {
            continue;
        }

#ifdef _WIN32
        DWORD timeout = 3000;
        setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
        struct timeval timeout{};
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif

        std::vector<char> buffer(1024, 0);
        
#ifdef _WIN32
        int bytes_received = recv(client_fd, buffer.data(), static_cast<int>(buffer.size() - 1), 0);
#else
        ssize_t bytes_received = recv(client_fd, buffer.data(), buffer.size() - 1, 0);
#endif

        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::string log_data(buffer.data());

            std::string_view view(log_data);
            std::cout << "📥 [RAW PACKET ARRIVED] -> " << view << std::endl;
        } else {
            std::cout << "⚠️ [DEFENSE ACTIVED] 不審な接続放置、または切断を検知。セッションを強制破棄しました。" << std::endl;
        }

        CLOSE_SOCKET(client_fd);
    }
}
