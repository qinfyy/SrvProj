#include "GatewayController.h"
#include <sstream>
#include "../GameSession.h"
#include "../AeadTool.h"
#include "../GameServices.h"
#include "../logger.h"
#include "../proto/NetMsgId.pb.h"
#include "Login.h"
#include <openssl/rand.h>

typedef void (*ReqHandler)(GameSession*, const std::string&, std::string&);

std::unordered_map<short, ReqHandler> g_Handlers;

void SetupRoutes() {
    g_Handlers.clear();

    g_Handlers[ike_req] = ike_req_Handler;
}

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
        if (!session || session->key.empty()) {
            rsp.statusCode = 500;
            rsp.body = "";
            return;
        }

        defaultSessionKey = session->key;
        encryptFunction = session->encryptFunction;
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
            std::string iv(12, '\0');
            memcpy(iv.data(), body.data(), 12);
            std::string cipher(reinterpret_cast<const char*>(body.data() + 12), body.size() - 12);
            AeadTool::Decrypt_Static(plain, defaultSessionKey, iv, cipher, cipher.size(), true, encryptFunction);
            offset = 10; // 跳过客户端包包头
        }
        else {
            auto whitewashed = AeadUtil::Wash(body, defaultSessionKey);
            std::string iv(12, '\0');
            memcpy(iv.data(), whitewashed.data(), 12);
            std::string cipher(reinterpret_cast<const char*>(whitewashed.data() + 12), whitewashed.size() - 12);
            AeadTool::Decrypt_Static(plain, defaultSessionKey, iv, cipher, cipher.size(), true, 0);
        }

        short msgId = (static_cast<uint8_t>(plain[offset]) << 8) | (static_cast<uint8_t>(plain[offset + 1]));
        offset += 2;
        reqData = plain.substr(offset);

		LOG_DEBUG("Received request, msgId: {}, data size: {}, sessionToken: {}, hasKey3: {}, encryptFunction: {}",
            msgId, reqData.size(), sessionToken, hasKey3, encryptFunction);

        // 更新会话的最后活动时间, 以便于会话过期机制正确工作
        //if (session) {
        //    session->UpdateLastActiveTime();
        //    session->UpdateIpAddress(ctx.ip());
        //}

        ReqHandler handler = nullptr;
        if (auto it = g_Handlers.find(msgId); it != g_Handlers.end())
        {
            handler = it->second;
        }

        if (!handler) {
            LOG_WARNING("Unhandled request: {}");
            rsp.statusCode = 500;
            rsp.body = "";
            return;
        }

        // 正式处理数据
        std::string rspOut;
        handler(session, reqData, rspOut);

        if (rspOut.empty()) {
            rsp.statusCode = 500;
            rsp.body = "";
            return;
        }

        LOG_DEBUG("Request handled successfully, response size: {}", rspOut.size());

        if (hasKey3) {
            //result = AeadHelper.encrypt(result, sessionKey, encryptMethod);

            //unsigned char iv[16];
            std::string iv(12, '\0');
            RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), iv.size());
            std::string cipher;
            AeadTool::Encrypt_Static(cipher, defaultSessionKey, iv, rspOut, static_cast<int>(rspOut.size()), true, encryptFunction);
            std::string finalResult;

            finalResult.reserve(cipher.size() + 12);
            finalResult.append(iv.data(), 12);
            finalResult.append(cipher);

            rsp.statusCode = 200;
            rsp.body = finalResult;
        }
        else {
            //result = AeadHelper.encryptGCM(result, sessionKey);

            std::string iv(16, '\0');
            RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), iv.size());
            std::string cipher;
            AeadTool::Encrypt_Static(cipher, defaultSessionKey, iv, rspOut, static_cast<int>(rspOut.size()), true, 0); // 0 = AES GCM, 1 = ChaCha20Poly1305
            std::string result;
            result.reserve(cipher.size() + 12);
            result.append(iv.data(), 12);
            result.append(cipher);

            auto finalResult = AeadUtil::Obfuscate(result, defaultSessionKey);

            rsp.statusCode = 200;
            rsp.body = finalResult;
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
