#include "LogGenerator.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>

LogGenerator::LogGenerator() {
    // 起動時のナノ秒単位のローカル時間（内部時計）をシード（ソルト）として投入し、乱数を完全初期化
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    m_twister.seed(static_cast<unsigned int>(seed));
}

std::string LogGenerator::generateDummyLog() {
    // 実験用ログの高速自動生成。User ID や IP、エラーメッセージのテンプレートを定義
    static const std::string ips[] = {"192.168.1.10", "10.0.0.5", "172.16.254.1", "185.220.101.5"};
    static const std::string ids[] = {"USR_9401", "USR_0002", "USR_7777", "UNKNOWN_HACKER"};
    static const std::string msgs[] = {"Login successful", "Password attempt failed", "SQL syntax error near 'OR'", "Token expired"};

    // メルセンヌ・ツイスタからインデックスを安全に抽出（分布の均一性を保証）
    std::uniform_int_distribution<size_t> dist(0, 3);
    
    // 現在時刻のタイムスタンプ生成
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");

    // ゼロコピーを意識した中間文字列ストリーム集約
    std::stringstream log_stream;
    log_stream << "[" << ss.str() << "] " << ips[dist(m_twister)] << " | " << ids[dist(m_twister)] << " | " << msgs[dist(m_twister)];
    return log_stream.str();
}

bool LogGenerator::shouldTriggerDeception(int sessionSeverity) {
    // 1-A仕様：セッションの危険度（0〜5）に応じて確率の壁を動的に重くする自律変動ロジック
    // 危険度0 ➔ 0%発動、危険度1 ➔ 20%、危険度5 ➔ 100%（ハッカーを泥沼に引きずり込む）
    int threshold = sessionSeverity * 20; 

    // 0〜100の整数を統計的均一性を保ってランダム抽出
    std::uniform_int_distribution<int> dist(0, 100);
    int rolledValue = dist(m_twister);

    // 乱数の発生パターンがrolledValue以下の場合は「欺瞞の壁」を起動
    return rolledValue <= threshold;
}
