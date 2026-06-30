#include "LogGenerator.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>

LogGenerator::LogGenerator() {
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    m_twister.seed(static_cast<unsigned int>(seed));
}

std::string LogGenerator::generateDummyLog() {
    static const std::string ips[] = {"192.168.1.10", "10.0.0.5", "172.16.254.1", "185.220.101.5"};
    static const std::string ids[] = {"USR_9401", "USR_0002", "USR_7777", "UNKNOWN_HACKER"};
    static const std::string msgs[] = {"Login successful", "Password attempt failed", "SQL syntax error near 'OR'", "Token expired"};

    std::uniform_int_distribution<size_t> dist(0, 3);
    
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");

    std::stringstream log_stream;
    log_stream << "[" << ss.str() << "] " << ips[dist(m_twister)] << " | " << ids[dist(m_twister)] << " | " << msgs[dist(m_twister)];
    return log_stream.str();
}

bool LogGenerator::shouldTriggerDeception(int sessionSeverity) {
    int threshold = sessionSeverity * 20; 
    std::uniform_int_distribution<int> dist(0, 100);
    return dist(m_twister) <= threshold;
}

std::string LogGenerator::generateSecureToken(const std::string& userId) {
    // 1. クライアント時間は無視。サーバーの厳格な「今この瞬間」のUNIXタイムスタンプを取得
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

    // 2. ユーザーID ＋ サーバー時間 ＋ 秘密のソルト をドッキング
    std::stringstream raw_token_stream;
    raw_token_stream << userId << "_" << seconds << "_" << m_secret_salt;

    // 3. C++の標準ハッシュ関数(std::hash)を用いて、一撃で高速なローリングトークン(16進数文字列)へ変換
    std::hash<std::string> hasher;
    size_t hashed_val = hasher(raw_token_stream.str());

    std::stringstream token_hex;
    token_hex << std::hex << std::setw(16) << std::setfill('0') << hashed_val;

    // トークン内部に「生成されたサーバー時間」を埋め込んでクライアントへ渡す（改ざん検知仕様）
    return token_hex.str() + "." + std::to_string(seconds);
}

bool LogGenerator::verifyToken(const std::string& userId, const std::string& token) {
    // 1. トークンをハッシュ値とタイムスタンプ部分に分離 (std::string_viewでゼロコピー分解)
    size_t dot_pos = token.find('.');
    if (dot_pos == std::string::npos) return false;

    std::string token_hash = token.substr(0, dot_pos);
    long long embedded_time = std::stoll(token.substr(dot_pos + 1));

    // 2. 【防衛壁】現在のサーバー時間を取得し、埋め込まれた時間との「差分」を計算
    auto now = std::chrono::system_clock::now();
    auto current_time = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    // 有効期限を「5秒」に厳格制限。ハッカーが過去時間を偽装して再利用してきたら一瞬で検知
    long long time_diff = current_time - embedded_time;
    if (time_diff < 0 || time_diff > 5) {
        std::cerr << "🚨 [2-A ANTI-REPLAY] 過去時間の偽装、または有効期限切れ(タイムスタンプ差分: " 
                  << time_diff << "秒)を検知。パケットを破砕します。" << std::endl;
        return false;
    }

    // 3. 改ざん検証：サーバー側で全く同じ条件でハッシュ値を再計算し、一致するかチェック
    std::stringstream verify_stream;
    verify_stream << userId << "_" << embedded_time << "_" << m_secret_salt;
    
    std::hash<std::string> hasher;
    size_t expected_hashed_val = hasher(verify_stream.str());

    std::stringstream expected_hex;
    expected_hex << std::hex << std::setw(16) << std::setfill('0') << expected_hashed_val;

    if (expected_hex.str() == token_hash) {
        std::cout << "✅ [2-A VERIFIED] サーバーサイド時間との整合性確認。正当なトークンです。(猶予残り: " 
                  << (5 - time_diff) << "秒)" << std::endl;
        return true;
    }

    std::cerr << "🚨 [2-A CRYPTO_ERROR] トークンの署名（ハッシュ値）が不一致です。改ざんを検知しました。" << std::endl;
    return false;
}
