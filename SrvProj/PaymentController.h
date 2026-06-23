#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>

#include "HttpMessage.h"

class PaymentContextService
{
public:
    struct WebOrderContext
    {
        std::string Token;
        std::string Source;
        std::string ProductKey;
        std::string GameOrderId;
        uint32_t PlayerUid = 0;
        int64_t CreatedAt = 0;
    };

    struct BattlePassCollectContext
    {
        uint32_t PlayerUid = 0;
        uint32_t RequestedMode = 0;
        uint32_t BattlePassId = 0;
    };

    static PaymentContextService& Instance();

    PaymentContextService(const PaymentContextService&) = delete;
    PaymentContextService& operator=(const PaymentContextService&) = delete;
    PaymentContextService(PaymentContextService&&) = delete;
    PaymentContextService& operator=(PaymentContextService&&) = delete;

    void RegisterWebOrderContext(const WebOrderContext& context);
    bool GetWebOrderContext(const std::string& token, WebOrderContext& outContext);
    void RemoveWebOrderContext(const std::string& token);
    void RegisterBattlePassCollectContext(const BattlePassCollectContext& context);
    bool ConsumeBattlePassCollectContext(uint32_t playerUid, BattlePassCollectContext& outContext);

private:
    PaymentContextService() = default;
    ~PaymentContextService() = default;

    std::unordered_map<std::string, WebOrderContext> mWebOrderContexts;
    std::mutex mWebOrderMutex;
    std::unordered_map<uint32_t, BattlePassCollectContext> mBattlePassCollectContexts;
    std::mutex mBattlePassCollectMutex;
};

void OrderProductsHandler(const HttpRequest& req, HttpResponse& rsp);

void OrderCreateHandler(const HttpRequest& req, HttpResponse& rsp);

void MockPayPageHandler(const HttpRequest& req, HttpResponse& rsp);

void OrderNotifyHandler(const HttpRequest& req, HttpResponse& rsp);
