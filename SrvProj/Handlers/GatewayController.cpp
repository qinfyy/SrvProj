#include "GatewayController.h"
#include <sstream>
#include <openssl/rand.h>
#include "../GameSession.h"
#include "../AeadTool.h"
#include "../GameServices.h"
#include "../logger.h"
#include "../proto/NetMsgId.pb.h"
#include "Login.h"
#include "Activity.h"
#include "Quest.h"

typedef std::string(*ReqHandler)(GameSession*, const std::string&);

std::unordered_map<short, ReqHandler> g_HandlerMap = {
    {ike_req, ike_req__Handler},
    {player_login_req, player_login_req__Handler},
    {player_data_req, player_data_req__Handler},
    {player_reg_req, player_reg_req__Handler},
    {player_ping_req, player_ping_req__Handler},
    {energy_info_req, energy_info_req__Handler},
    {mall_package_list_req, mall_package_list_req__Handler},
    {activity_detail_req, activity_detail_req__Handler},
    {potential_preselection_list_req, potential_preselection_list_req__Handler},
    {daily_shop_reward_receive_req, daily_shop_reward_receive_req__Handler},
    {daily_mall_reward_receive_req, daily_mall_reward_receive_req__Handler},
    {quest_daily_reward_receive_req, quest_daily_reward_receive_req__Handler},
    {quest_daily_active_reward_receive_req, quest_daily_active_reward_receive_req__Handler},
    {quest_weekly_reward_receive_req, quest_weekly_reward_receive_req__Handler},
    {quest_weekly_active_reward_receive_req, quest_weekly_active_reward_receive_req__Handler},
    {achievement_info_req, achievement_info_req__Handler},
    {achievement_reward_receive_req, achievement_reward_receive_req__Handler},
    {client_event_report_req, client_event_report_req__Handler},
    {battle_pass_quest_reward_receive_req, battle_pass_quest_reward_receive_req__Handler},
};

void AgentHandler(const HttpRequest& req, HttpResponse& rsp) {
    GameSession* session = nullptr;

    std::string defaultSessionKey = AeadTool::twServerGarbleKey;
    bool hasKey3 = false;
    int encryptFunction = 0;

    std::string sessionToken;
    if (auto it = req.headers.find("X-Token"); it != req.headers.end()) {
        sessionToken = it->second;
    }

    rsp.headers["Server"] = "agent";

    if (!sessionToken.empty()) {
        session = GameServices::Instance().GetSessionByToken(sessionToken);

        // 找不到会话
        if (!session || session->mKey.empty()) {
            rsp.statusCode = 500;
            rsp.body = "";
            return;
        }

        defaultSessionKey = session->mKey;
        encryptFunction = session->mEncryptFunction;
        hasKey3 = true;
    }

    std::string reqData;
    short msgId = 0;

    try {
        std::string body = req.body;
        std::string plain;
        int offset = 0;
        if (body.size() <= 12) {
            rsp.statusCode = 500;
            rsp.body = "";
            return;
        }

        if (hasKey3) {
            std::array<char, 12> iv;
            memcpy(iv.data(), body.data(), 12);
            std::string cipher(reinterpret_cast<const char*>(body.data() + 12), body.size() - 12);
            AeadTool::Dencrypt_BouncyCastle(plain, defaultSessionKey, std::string_view(iv.data(), iv.size()), cipher, cipher.size(), true, encryptFunction);
            offset = 10; // 跳过客户端包包头
        }
        else {
            auto whitewashed = AeadUtil::Wash(body, defaultSessionKey);
            std::array<char, 12> iv;
            memcpy(iv.data(), whitewashed.data(), 12);
            std::string cipher(reinterpret_cast<const char*>(whitewashed.data() + 12), whitewashed.size() - 12);
            AeadTool::Dencrypt_BouncyCastle(plain, defaultSessionKey, std::string_view(iv.data(), iv.size()), cipher, cipher.size(), true, 0); // 0 = AES GCM, 1 = ChaCha20Poly1305
        }

        msgId = (static_cast<uint8_t>(plain[offset]) << 8) | (static_cast<uint8_t>(plain[offset + 1]));
        offset += 2;
        reqData = plain.substr(offset);

        LOG_DEBUG("Received request, msgId: {}, data size: {}, sessionToken: {}, hasKey3: {}, encryptFunction: {}",
            msgId, reqData.size(), sessionToken, hasKey3, encryptFunction);

        if (session) {
            session->mLastActiveTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        }

        ReqHandler handler = nullptr;
        if (auto it = g_HandlerMap.find(msgId); it != g_HandlerMap.end())
        {
            handler = it->second;
        }

        std::string rspOut;
        if (handler) {
            rspOut = handler(session, reqData);
        }
        else {
            rspOut = DummyHandler(msgId);
        }

        if (rspOut.empty()) {
            LOG_WARNING("Unhandled request: {}", msgId);
            rsp.statusCode = 500;
            rsp.body = "";
            return;
        }
        else {
            LOG_DEBUG("Request handled successfully, response size: {}", rspOut.size());
        }

        if (hasKey3) {
            std::array<char, 12> iv;
            RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), iv.size());
            std::string cipher;
            AeadTool::Encrypt_BouncyCastle(cipher, defaultSessionKey, std::string_view(iv.data(), iv.size()), rspOut, static_cast<int>(rspOut.size()), true, encryptFunction);
            
            std::string finalResult;
            finalResult.reserve(cipher.size() + 12);
            finalResult.append(iv.data(), 12);
            finalResult.append(cipher);

            rsp.statusCode = 200;
            rsp.body = finalResult;
        }
        else {
            std::array<char, 12> iv;
            RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), iv.size());
            std::string cipher;
            AeadTool::Encrypt_BouncyCastle(cipher, defaultSessionKey, std::string_view(iv.data(), iv.size()), rspOut, static_cast<int>(rspOut.size()), true, 0); // 0 = AES GCM, 1 = ChaCha20Poly1305
            
            std::string result;
            result.reserve(cipher.size() + 12);
            result.append(iv.data(), 12);
            result.append(cipher);

            auto finalResult = AeadUtil::Obfuscate(result, defaultSessionKey);

            rsp.statusCode = 200;
            rsp.body = finalResult;

            return;
        }
    }
    catch (const std::exception& e) {
        // error
        std::ostringstream logOs;
        logOs << "Agent 错误: ";
        const std::exception* current = &e;
        int level = 0;

        while (current) {
            logOs << std::string(level * 2, ' ') << current->what() << std::endl;

            try {
                std::rethrow_if_nested(*current);
                break;
            }
            catch (const std::exception& nested) {
                current = &nested;
                level++;
            }
            catch (...) {
                break;
            }
        }

        LOG_ERROR(logOs.str());

        rsp.statusCode = 500;
        rsp.body = "";

        return;
    }
}

std::string DummyHandler(short reqId)
{
    const auto* enumDesc = NetMsgId_descriptor();
    if (!enumDesc) {
        return "";
    }

    const auto* reqValue = enumDesc->FindValueByNumber(reqId);
    if (!reqValue) {
        return "";
    }

    std::string_view reqName = reqValue->name();
    constexpr std::string_view suffix = "req";
    if (!reqName.ends_with(suffix)) {
        return "";
    }

    std::string failedAckName = std::string(reqName.substr(0, reqName.size() - suffix.size())) + "failed_ack";
    const auto* failedAckValue = enumDesc->FindValueByName(failedAckName);
    if (!failedAckValue) {
        return "";
    }

    return GameSession::EncodeMessage(static_cast<short>(failedAckValue->number()), "");
}
