#include <iostream>
#include "SocketServer.hpp"
#include "LogGenerator.hpp"

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "🛡️ Vestige-Filter: SIEM エージェント起動コアシステム" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "[INFO] 自律変動防衛アーキテクチャ（C++17）が有効化されました。" << std::endl;

    // セキュリティ分析の標準ポート 514 番（Syslog）で生ソケットサーバーをインスタンス化
    // ※一般ユーザー権限で検証しやすいよう、テスト用の高位ポート（例: 8080）も選べます。
    // 今回は低レイヤー検証の標準として 8080 ポートで起動します。
    int target_port = 8080;
    SocketServer server(target_port);

    // サーバーのネットワーク回路を開く
    if (!server.start()) {
        std::cerr << "[FATAL] サーバーの初期化インフラに致命的なエラーが発生しました。" << std::endl;
        return 1;
    }

    // 3秒タイムアウトを搭載した無限受信ループを起動（制御を委譲）
    server.listenForLogs();

    return 0;
}
