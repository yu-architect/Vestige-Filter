#ifndef LOG_GENERATOR_HPP
#define LOG_GENERATOR_HPP

#include <string>
#include <random>

class LogGenerator {
public:
    LogGenerator();
    ~LogGenerator() = default;

    // 大量ダミーログの高速生成ロジック
    std::string generateDummyLog();

    // 1-A仕様：ハッカーの確定性を2進数から破壊する確率ツマミ（サイバー欺瞞）
    bool shouldTriggerDeception(int sessionSeverity);

private:
    // std::rand の短い周期を克服する、天文学的周期のメルセンヌ・ツイスタ疑似乱数生成器
    std::mt19937 m_twister;
};

#endif // LOG_GENERATOR_HPP
