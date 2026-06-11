#include "SocketServer.hpp"
#include <iostream>
#include <cstring>

#ifdef _WIN32
    // Windows用の初期化フラグ
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
    // Windows環境のみ必要なWSAStartup（ネットワーク初期化手続き）
    if (!g_winsock_initialized) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "[ERROR] Winsock初期化失敗。" << std::endl;
            return false;
        }
        g_winsock_initialized = true;
    }
#endif

    // 1. 生ソケットの作成 (IPv4 / TCP通信)
    m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_fd == INVALID_SOCKET) {
        std::cerr << "[ERROR] ソケット作成失敗。" << std::endl;
        return false;
    }

    // 2. ソケットの再利用設定 (サーバー再起動時のポート衝突・穴①防衛)
    int opt = 1;
#ifdef _WIN32
    setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    // 3. 通信ポートとアドレス（IP）のバインド（紐付け）
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 全てのネットワークカードから受信
    address.sin_port = htons(m_port);     // ホストバイト順からネットワークバイト順への変換

#ifdef _WIN32
    if (bind(m_server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
#else
    if (bind(m_server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
#endif
        std::cerr << "[ERROR] ポート " << m_port << " のバインドに失敗しました。" << std::endl;
        return false;
    }

    // 4. 接続待ち受け状態への移行 (最大同時キュー待ち数: 10)
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
    std::cout << "[INFO] ログの受信待ち受けを開始します..." << std::endl;
    
    // 次のステップで、ここに無限ループとタイムアウト（Slowloris防衛）の受信コアロジックをドッキングします。
}
