#include <iostream>
#include <thread>
#include <chrono>
#include "SocketServer.hpp"
#include "LogGenerator.hpp"

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "🛡️ Vestige-Filter: SIEM エージェント起動コアシステム" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "[INFO] 自律変動防衛＆メルセンヌ・ツイスタ乱数コアが有効化されました。" << std::endl;

    // 1. ログジェネレーター（頭脳）と生ソケットサーバー（筋肉）をインスタンス化
    LogGenerator generator;
    int target_port = 8080;
    SocketServer server(target_port);

    // 2. サーバーのネットワーク回路を開く
    if (!server.start()) {
        std::cerr << "[FATAL] サーバーの初期化インフラに致命的なエラーが発生しました。" << std::endl;
        return 1;
    }

    // 3. 別スレッドで生ソケットの待ち受け（3秒タイムアウト無限ループ）をバックグラウンド起動
    std::thread server_thread([&server]() {
        server.listenForLogs();
    });
    server_thread.detach(); // スレッドを切り離して並行Ops駆動

    std::cout << "[SUCCESS] バックグラウンドで防衛壁がアクティブになりました。" << std::endl;
    std::cout << "[INFO] 実験用ログの高速自動生成を開始します（Ctrl+C で停止）...\n" << std::endl;

    // 4. メインループ：メルセンヌ・ツイスタによるログ超高速生成と確率ツマミのシミュレーション
    int virtual_session_severity = 1; // 危険度初期値（1 = 20%の確率で欺瞞発動）

    while (true) {
        // 大量ダミーログの超高速生成（メモリコピーゼロを意識）
        std::string raw_log = generator.generateDummyLog();
        std::cout << "📊 [LOG GENERATED] " << raw_log << std::endl;

        // 1-A仕様：危険度に応じた動的確率ツマミ（サイバー欺瞞判定）
        if (generator.shouldTriggerDeception(virtual_session_severity)) {
            std::cout << "🔥 [DECEPTION TRIGGERED] セッション危険度 " << virtual_session_severity 
                      << " に応じた確率の壁が発動！不審なパケットの足止めを開始します。" << std::endl;
        }

        // 実験時に画面が文字で埋め尽くされないよう、1秒ごとにログを生成（デバッグ用）
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // シミュレーション：時間の経過とともにハッカーの攻撃が激化し、危険度が動的に跳ね上がる挙動を再現
        static int loop_count = 0;
        if (++loop_count % 5 == 0 && virtual_session_severity < 5) {
            virtual_session_severity++;
            std::cout << "\n🚨 [ALERT] 敵の攻撃の激化を検知。自律変動により確率ツマミを重くします（危険度: " 
                      << virtual_session_severity << " ➔ 発動確率: " << (virtual_session_severity * 20) << "%）\n" << std::endl;
        }
    }

    return 0;
}
