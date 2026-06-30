#include <iostream>
#include <thread>
#include <chrono>
#include "SocketServer.hpp"
#include "LogGenerator.hpp"

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "🛡️ Vestige-Filter: SIEM エージェント起動コアシステム" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "[INFO] 2-A仕様：サーバーサイド時間トークン検証コアが有効化されました。\n" << std::endl;

    LogGenerator generator;
    std::string target_user = "USR_9401";

    // 1. 【実験①】正当なトークンの生成と、即時検証（時間差0秒のクリーンアクセス）
    std::cout << "🎟️ [STEP 1] サーバーサイド時間で正当なトークンを発行します..." << std::endl;
    std::string valid_token = generator.generateSecureToken(target_user);
    std::cout << "🔗 [TOKEN GENERATED] -> " << valid_token << "\n" << std::endl;

    std::cout << "🔍 [TEST 1] 発行直後の正当なトークンを検証します（猶予5秒以内）..." << std::endl;
    generator.verifyToken(target_user, valid_token);
    std::cout << "--------------------------------------------------\n" << std::endl;

    // 2. 【実験②】ハッカーによる「過去時間偽装・再利用（リプレイ攻撃）」のシミュレーション
    std::cout << "⏳ [STEP 2] ネットワーク遅延、またはハッカーの潜伏を再現するため、あえて「6秒間」処理をフリーズさせます..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(6));
    std::cout << "⏰ [TIME PASSED] 6秒が経過しました。有効期限（5秒）が切れ、トークンは『過去の遺物』になりました。\n" << std::endl;

    std::cout << "🏴‍☠️ [TEST 2] ハッカーがこの過去のトークンを再利用してパケットを送りつけてきました..." << std::endl;
    if (!generator.verifyToken(target_user, valid_token)) {
        std::cout << "🏁 [SUCCESS] 2-A防衛壁がハッカーの過去時間偽装を完全に見破り、破砕することに成功しました！" << std::endl;
    }
    std::cout << "==================================================" << std::endl;

    return 0;
}
