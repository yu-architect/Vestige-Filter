#ifndef LOG_GENERATOR_HPP
#define LOG_GENERATOR_HPP

#include <string>
#include <random>

class LogGenerator {
public:
    LogGenerator();
    ~LogGenerator() = default;

    std::string generateDummyLog();
    bool shouldTriggerDeception(int sessionSeverity);

    // 2-A仕様：サーバーサイド時間に基づく自律変動暗号トークンの生成
    std::string generateSecureToken(const std::string& userId);

    // 2-A仕様：過去時間偽装をナノ秒単位で検知して破砕する検証ロジック
    bool verifyToken(const std::string& userId, const std::string& token);

private:
    std::mt19937 m_twister;
    
    // トークンの偽造を防ぐためのサーバー内秘匿のソルト（秘密鍵）
    const std::string m_secret_salt = "Vestige_Secret_Salt_2026_Anti_Replay";
};

#endif // LOG_GENERATOR_HPP
