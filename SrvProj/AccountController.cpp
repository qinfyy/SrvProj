#include "AccountController.h"
#include "AccountServer.h"
#include <iostream>
#include "AeadTool.h"
#include <nlohmann/json.hpp>
#include <google/protobuf/util/json_util.h>
#include <openssl/rand.h>
#include "Util.h"
#include "Logger.h"
#include <vector>
#include <unordered_map>
#include "proto/dump.pb.h"
#include "DbMgr.h"
#include "ResultCode.h"
#include "GameServices.h"
#include "GameTime.h"


AsyncTask<std::optional<ServerListMeta>> GetServerList(RouteContext& context)
{
    const HttpClient::Response upstream = co_await context.Http().Get(
        L"nova-static.stargazer-games.com", L"/meta/serverlist.html",
        { { "User-Agent", "UnityPlayer/2022.3.62f2 (UnityWebRequest/1.0, libcurl/8.10.1-DEV)" } });
    if (!upstream.Succeeded() || upstream.response.statusCode != 200 || upstream.response.body.size() <= 16)
    {
        co_return std::nullopt;
    }

    std::optional<ServerListMeta> result;
    try
    {
        co_await context.Runtime().RunBlocking([&upstream, &result]
            {
                std::array<char, 16> iv;
                memcpy(iv.data(), upstream.response.body.data(), iv.size());
                const std::string cipher(upstream.response.body.data() + iv.size(),
                    upstream.response.body.size() - iv.size());
                const std::string plain = AeadTool::DecryptAesCBCInfo(AeadTool::twServerMetaKey,
                    std::string_view(iv.data(), iv.size()), cipher);
                ServerListMeta meta;
                if (meta.ParseFromString(plain))
                {
                    result = std::move(meta);
                }
            });
    }
    catch (...)
    {
        co_return std::nullopt;
    }

    co_return result;
}

AsyncTask<void> ServerListHandler(RouteContext& context, const HttpRequest& request, HttpResponseWriter& writer)
{
    HttpResponse response;
    co_await context.Runtime().RunBlocking([&request, &response]
        {
            const HttpRequest& req = request;
            HttpResponse& rsp = response;
            try {
                ServerListMeta meta;

                meta.set_version(129);

                ServerAgent* agent = meta.add_agent();
                agent->set_name(U8("星塔旅人"));
                agent->set_addr("https://nova.stargazer-games.com/agent-zone-1/");
                agent->set_status(1);
                agent->set_zone(1);

                meta.set_reportendpoint("https://nova.stargazer-games.com/report/");

                Rule* rule1 = meta.add_rules();
                rule1->set_platform(Platform_Ios);
                rule1->set_channel("Official");
                rule1->set_version("1.9.0");
                rule1->set_op(OP_Lt);
                rule1->set_action(Action_Download);
                rule1->set_url("https://apps.apple.com/tw/app/%E6%98%9F%E5%A1%94%E6%97%85%E4%BA%BA/id6738902933");
                rule1->set_enable(1);

                Rule* rule2 = meta.add_rules();
                rule2->set_platform(Platform_Android);
                rule2->set_channel("Official");
                rule2->set_version("1.9.0");
                rule2->set_op(OP_Lt);
                rule2->set_action(Action_Download);
                rule2->set_url("https://play.google.com/store/apps/details?id=com.Stargazer.StellaSora");
                rule2->set_enable(1);

                Rule* rule3 = meta.add_rules();
                rule3->set_platform(Platform_PC);
                rule3->set_channel("Official");
                rule3->set_version("1.9.0");
                rule3->set_op(OP_Lt);
                rule3->set_action(Action_Download);
                rule3->set_url(U8("text://請在PC啟動器內點選更新按鈕完成版本更新"));
                rule3->set_enable(1);

                std::string plain;
                meta.SerializeToString(&plain);

                std::array<char, 16> iv;
                RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), iv.size());

                std::string cipher = AeadTool::EncryptAesCBCInfo(AeadTool::twServerMetaKey, std::string_view(iv.data(), iv.size()), plain);

                std::string output;
                output.reserve(cipher.size() + 16);
                output.append(reinterpret_cast<const char*>(iv.data()), 16);
                output.append(cipher);

                rsp.statusCode = 200;
                rsp.headers["Content-Type"] = "text/html";
                rsp.body = output;

            }
            catch (const std::exception& e)
            {
                rsp.statusCode = 500;
                LOG_ERROR("发生错误: {}", e.what());
                rsp.body = std::string("Exception: ") + e.what();
            }
        });
    co_await writer.WriteResponse(response);
}

//void QuickLoginHandler(const HttpRequest& req, HttpResponse& rsp) {
//    std::string uid;
//    std::string token;
//
//    auto it = req.headers.find("Authorization");
//    if (it != req.headers.end()) {
//        try {
//            nlohmann::json authJson = nlohmann::json::parse(it->second);
//            if (authJson.contains("Head")) {
//                uid = authJson["Head"].value("UID", "");
//                token = authJson["Head"].value("Token", "");
//            }
//        }
//        catch (const std::exception& e) {
//            LOG_WARNING("请求解析失败: {}", e.what());
//            rsp.statusCode = 400;
//            rsp.headers["Content-Type"] = "application/json";
//            rsp.body = R"({"Code":400,"Msg":"Invalid JSON"})";
//            return;
//        }
//    }
//
//    time_t now = time(NULL);
//    nlohmann::json resp = {
//        {"Code", 200},
//        {"Msg", "OK"},
//        {"Data", {
//            {"AgeVerifyMethod", 0},
//            {"Destroy", nullptr},
//            {"IsTestAccount", false},
//            {"Keys", nlohmann::json::array({
//                {
//                    {"ID", uid},
//                    {"Type", "yostar"},
//                    {"Key", "qinfyy233@gmail.com"},
//                    {"NickName", "qi***33@gmail.com"},
//                    {"CreatedAt", 0}
//                }
//            })},
//            {"ServerNowAt", now},
//            {"UserInfo", {
//                {"ID", uid},
//                {"UID2", 0},
//                {"PID", "TW-NOVA"},
//                {"Token", "1e91af080c107c0ec8bbb52b45a3245a79242093"},
//                {"Birthday", ""},
//                {"RegChannel", "googleplay"},
//                {"TransCode", ""},
//                {"State", 1},
//                {"DeviceID", ""},
//                {"CreatedAt", 1760971305}
//            }},
//            {"Yostar", {
//                {"ID", "Y" + uid},
//                {"Country", "TW"},
//                {"Nickname", "user44151568718"},
//                {"Picture", ""},
//                {"State", 1},
//                {"AgreeAd", 0},
//                {"CreatedAt", 0}
//            }},
//            {"YostarDestroy", nullptr}
//        }}
//    };
//
//    rsp.statusCode = 200;
//    rsp.headers["Content-Type"] = "application/json";
//    rsp.body = resp.dump();
//}

AsyncTask<void> QuickLoginHandler(RouteContext& context, const HttpRequest& request, HttpResponseWriter& writer)
{
    HttpResponse response;
    co_await context.Runtime().RunBlocking([&request, &response]
        {
            const HttpRequest& req = request;
            HttpResponse& rsp = response;
            std::string uid;
            std::string token;

            auto it = req.headers.find("Authorization");
            if (it != req.headers.end()) {
                try {
                    nlohmann::json authJson = nlohmann::json::parse(it->second);
                    if (authJson.contains("Head")) {
                        uid = authJson["Head"].value("UID", "");
                        token = authJson["Head"].value("Token", "");
                    }
                }
                catch (const std::exception& e) {
                    LOG_WARNING("请求解析失败: {}", e.what());
                    rsp.statusCode = 200;
                    rsp.headers["Content-Type"] = "application/json";
                    rsp.body = "{\"Code\":" + std::to_string(ResultCode::CLIENT_PARAMETER_ERROR) + ",\"Msg\":\"请求无效\"}";
                    return;
                }
            }

            DbMgr::User user;
            if (!DbMgr::Instance().LoginByUidToken(uid, token, user))
            {
                rsp.statusCode = 200;
                rsp.body = "{\"Code\":" + std::to_string(ResultCode::TOKEN_AUTH_FAILED) + ",\"Msg\":\"授权过期\"}";
                rsp.headers["Content-Type"] = "application/json";
                return;
            }

            time_t now = time(NULL);
            nlohmann::json resp = {
                {"Code", 200},
                {"Msg", "OK"},
                {"Data", {
                    {"AgeVerifyMethod", 0},
                    {"Destroy", nullptr},
                    {"IsTestAccount", false},
                    {"Keys", nlohmann::json::array({
                        {
                            {"ID", user.uid},
                            {"Type", "yostar"},
                            {"Key", user.openId},
                            {"NickName", user.openId},
                            {"CreatedAt", 0}
                        }
                    })},
                    {"ServerNowAt", now},
                    {"UserInfo", {
                        {"ID", uid},
                        {"UID2", 0},
                        {"PID", "TW-NOVA"},
                        {"Token", user.token},
                        {"Birthday", ""},
                        {"RegChannel", "googleplay"},
                        {"TransCode", ""},
                        {"State", 1},
                        {"DeviceID", ""},
                        {"CreatedAt", 0}
                    }},
                    {"Yostar", {
                        {"ID", "Y" + user.uid},
                        {"Country", "TW"},
                        {"Nickname", "user" + user.uid},
                        {"Picture", ""},
                        {"State", 1},
                        {"AgreeAd", 0},
                        {"CreatedAt", 0}
                    }},
                    {"YostarDestroy", nullptr}
                }}
            };

            rsp.statusCode = 200;
            rsp.headers["Content-Type"] = "application/json";
            rsp.body = resp.dump();
        });
    co_await writer.WriteResponse(response);
}

AsyncTask<void> LoginHandler(RouteContext& context, const HttpRequest& request, HttpResponseWriter& writer)
{
    HttpResponse response;
    co_await context.Runtime().RunBlocking([&request, &response]
        {
            const HttpRequest& req = request;
            HttpResponse& rsp = response;
            nlohmann::json bodyJson;

            try {
                bodyJson = nlohmann::json::parse(req.body);
            }
            catch (const std::exception& e) {
                LOG_WARNING("请求解析失败: {}", e.what());

                rsp.statusCode = 200;
                rsp.headers["Content-Type"] = "application/json";
                rsp.body = "{\"Code\":" + std::to_string(ResultCode::CLIENT_PARAMETER_ERROR) + ",\"Data\":{},\"Msg\":\"请求无效\"}";
                return;
            }

            std::string openId = bodyJson.value("OpenID", "");
            std::string reqToken = bodyJson.value("Token", "");

            DbMgr::User user;
            if (!DbMgr::Instance().LoginByOpenId(openId, user))
            {
                if (!DbMgr::Instance().RegisterByOpenId(openId, user))
                {
                    rsp.statusCode = 200;
                    rsp.body = "{\"Code\":" + std::to_string(ResultCode::PARAM_PID_INVALID) + ",\"Msg\":\"自动注册失败\"}";
                    rsp.headers["Content-Type"] = "application/json";
                    return;
                }
            }

            nlohmann::json resp = {
                {"Code", 200},
                {"Msg", "OK"},
                {"Data", {
                    {"AgeVerifyMethod", 0},
                    {"IsNew", 0},
                    {"UserInfo", {
                        {"ID", user.uid},
                        {"UID2", 0},
                        {"PID", "TW-NOVA"},
                        {"Token", user.token},
                        {"Birthday", ""},
                        {"RegChannel", "googleplay"},
                        {"TransCode", ""},
                        {"State", 1},
                        {"DeviceID", ""},
                        {"CreatedAt", 0}
                    }},
                    {"Yostar", {
                        {"ID", "Y" + user.uid},
                        {"Country", "TW"},
                        {"Nickname", "user" + user.uid},
                        {"Picture", ""},
                        {"State", 1},
                        {"AgreeAd", 0},
                        {"CreatedAt", 0}
                    }}
                }}
            };

            rsp.statusCode = 200;
            rsp.headers["Content-Type"] = "application/json";
            rsp.body = resp.dump();
        });
    co_await writer.WriteResponse(response);
}

AsyncTask<void> DetailHandler(RouteContext& context, const HttpRequest& request, HttpResponseWriter& writer)
{
    co_await QuickLoginHandler(context, request, writer);
}

AsyncTask<void> SmsHandler(RouteContext& context, const HttpRequest& request, HttpResponseWriter& writer)
{
    HttpResponse response;
    co_await context.Runtime().RunBlocking([&response]
        {
            HttpResponse& rsp = response;
            nlohmann::json rspJson = {
                {"Code", 200},
                {"Msg", "OK"},
                {"Data", nlohmann::json::object()}
            };

            rsp.statusCode = 200;
            rsp.headers["Content-Type"] = "application/json";
            rsp.body = rspJson.dump();
        });
    co_await writer.WriteResponse(response);
}

AsyncTask<void> AuthHandler(RouteContext& context, const HttpRequest& request, HttpResponseWriter& writer)
{
    HttpResponse response;
    co_await context.Runtime().RunBlocking([&request, &response]
        {
            const HttpRequest& req = request;
            HttpResponse& rsp = response;
            nlohmann::json reqJson;

            try {
                reqJson = nlohmann::json::parse(req.body);
            }
            catch (const std::exception& e) {
                rsp.statusCode = 400;
                rsp.headers["Content-Type"] = "application/json";
                rsp.body = R"({"Code":400,"Msg":"Invalid JSON"})";
                return;
            }

            std::string account = reqJson.value("Account", "");
            std::string code = reqJson.value("Code", "");
            std::string token = "123456";

            nlohmann::json resp = {
                {"Code", 200},
                {"Msg", "OK"},
                {"Data", {
                    {"UID", account},
                    {"Token", token},
                    {"Account", account}
                }}
            };

            rsp.statusCode = 200;
            rsp.headers["Content-Type"] = "application/json";
            rsp.body = resp.dump();
        });
    co_await writer.WriteResponse(response);
}

AsyncTask<void> CommonConfigHandler(RouteContext& context, const HttpRequest& request,
    HttpResponseWriter& writer)
{
    HttpResponse response;
    co_await context.Runtime().RunBlocking([&response]
        {
            HttpResponse& rsp = response;
            const char* rspText = U8("{\"Code\":200,\"Data\":{\"AppConfig\":{\"ACCOUNT_RETRIEVAL\":{\"FIRST_LOGIN_POPUP\":false,\"LOGIN_POPUP\":false,\"PAGE_URL\":\"\"},\"AGREEMENT_POPUP_TYPE\":\"Browser\",\"APPLE_CURRENCY_BLOCK_LIST\":null,\"APPLE_TYPE_KEY\":\"apple\",\"APP_CLIENT_LANG\":[\"zh-Hant\"],\"APP_DEBUG\":1,\"APP_GL\":\"zh-Hant\",\"BIND_METHOD\":[\"google\",\"apple\"],\"CAPTCHA_ENABLED\":false,\"CLIENT_LOG_REPORTING\":{\"ENABLE\":true},\"CREDIT_INVESTIGATION\":\"0.0\",\"DESTROY_USER_DAYS\":15,\"DESTROY_USER_ENABLE\":1,\"DETECTION_ADDRESS\":{\"AUTO\":{\"DNS\":[\"https://nova-static.stargazer-games.com/meta/serverlist.html\",\"https://nova.stargazer-games.com\",\"https://jp-sdk-api.yostarplat.com\"],\"HTTP\":[\"https://nova-static.stargazer-games.com/meta/serverlist.html\",\"https://nova.stargazer-games.com\",\"https://jp-sdk-api.yostarplat.com\"],\"MTR\":[\"https://nova-static.stargazer-games.com/meta/serverlist.html\",\"https://nova.stargazer-games.com\",\"https://jp-sdk-api.yostarplat.com\"],\"PING\":[\"https://nova-static.stargazer-games.com/meta/serverlist.html\",\"https://nova.stargazer-games.com\",\"https://jp-sdk-api.yostarplat.com\"],\"TCP\":[\"https://nova-static.stargazer-games.com/meta/serverlist.html\",\"https://nova.stargazer-games.com\",\"https://jp-sdk-api.yostarplat.com\"]},\"ENABLE\":true,\"ENABLE_MANUAL\":true,\"INTERNET\":\"https://www.google.com\",\"INTERNET_ADDRESS\":\"https://www.google.com\",\"NETWORK_ENDPORINT\":\"https://ap-southeast-1.log.aliyuncs.com\",\"NETWORK_PROJECT\":\"yostar-oversea-netsdk-logging\",\"NETWORK_SECRET_KEY\":\"eyJhbGl5dW5fdWlkIjoiMTI5NDA1ODU3MDYyMTk5MCIsImlwYV9hcHBfaWQiOiJMNFFSSG1zNzdqdW5WSGNCWTZVd1ZLIiwic2VjX2tleSI6ImM2YzAxZDZhNTZkZjdlMTY3Yjg2MmFjM2EwYzQ5MTJlN2RmZTRmNjIxMTc1YTZkOGI5ZjcxYWJhYWY2YWNjYmQ2MTg1ZjVmMmYxMTVkMTczNjg5MGRlYWU0Nzg0MTI0NzFmZGNjMmRlOWUwMWMyNmJhOTdmZDA0YTJkM2IxZjUwIiwic2lnbiI6ImQxOGQwZTc0YjFhYWIzZmVlYWNmNDY2ZTYyYjQyMDZmYzA4NWFmMjJiN2ZjODQ1MDYzMjM3MDNlOGVkOGUxNGU5ZWI0ZGM3YjllOTFiNzE3NmUxZTBmYjBhOTU1OWQxMTFhM2QyMzU2YTQyNWQ1YTlkNGI1ZWMxMWQxYjY0NTBjIn0=\"},\"ENABLE_AGREEMENT\":true,\"ENABLE_MULTI_LANG_AGREEMENT\":false,\"ENABLE_TEXT_REVIEW\":true,\"ERROR_CODE\":\"3.0\",\"FILE_DOMAIN\":\"https://storage.googleapis.com/sdkplat-jp-prod\",\"GEETEST_ENABLE\":false,\"GEETEST_ID\":\"\",\"GOOGLE_ANALYTICS_MEASUREMENT_ID\":\"\",\"MIGRATE_POPUP\":true,\"NICKNAME_REG\":\"^[A-Za-z0-9]{2,20}$\",\"PASSPORT_DESTROY_DAYS\":15,\"POPUP\":{\"Data\":[{\"Lang\":\"ja\",\"Text\":\"Yostar IDを作成\"},{\"Lang\":\"en\",\"Text\":\"Create a Yostar account\"},{\"Lang\":\"kr\",\"Text\":\"YOSTAR 계정 가입하기\"},{\"Lang\":\"fr\",\"Text\":\"Créez votre compte Yostar\"},{\"Lang\":\"de\",\"Text\":\"Einen Yostar-Account erstellen\"}],\"Enable\":true},\"PRIVACY_AGREEMENT\":\"0.1\",\"RECHARGE_LIMIT\":{\"Enable\":false,\"IsOneLimit\":false,\"Items\":[],\"OneLimitAmount\":0},\"SHARE\":{\"CaptureScreen\":{\"AutoCloseDelay\":0,\"Enabled\":false},\"Facebook\":{\"AppID\":\"\",\"Enabled\":true},\"Instagram\":{\"Enabled\":false},\"Kakao\":{\"AppKey\":\"\",\"Enabled\":false},\"Naver\":{\"Enabled\":false},\"Twitter\":{\"Enabled\":true}},\"SLS\":{\"ACCESS_KEY_ID\":\"7b5d0ffd0943f26704fc547a871c68b1b5d56b5c9caeb354205b81f445d7af59\",\"ACCESS_KEY_SECRET\":\"4a5e9cc8a50819290c9bfa1fedc79da7c50e85189a05eb462a3d28a7688eabb0\",\"ENABLE\":true},\"SURVEY_POPUP_TYPE\":\"Browser\",\"UDATA\":{\"Enable\":true,\"URL\":\"https://udata-api-jp.open.yo-star.com\"},\"USER_AGREEMENT\":\"0.1\",\"YOSTAR_PREFIX\":\"user\"},\"EuropeUnion\":false,\"StoreConfig\":{\"ADJUST_APPID\":\"\",\"ADJUST_CHARGEEVENTTOKEN\":\"\",\"ADJUST_ENABLED\":0,\"ADJUST_EVENTTOKENS\":null,\"ADJUST_ISDEBUG\":0,\"AIRWALLEX_ENABLED\":false,\"AI_HELP\":{\"AihelpAppID\":\"yostar1_platform_0f3a2fdbf61ff427d92b6696a0cf8e30\",\"AihelpAppKey\":\"YOSTAR1_app_abf38d0a88924a40ac990b58865fa64d\",\"AihelpDomain\":\"yostar1.aihelp.net\",\"CustomerEmailAddr\":\"\",\"CustomerServiceURL\":\"\",\"CustomerWay\":1,\"DisplayType\":\"Browser\",\"Enable\":1,\"Mode\":\"robot\"},\"APPLEID\":\"\",\"CODA_ENABLED\":false,\"ENABLED_PAY\":{\"AIRWALLEX_ENABLED\":false,\"CODA_ENABLED\":false,\"GMOAlipay\":false,\"GMOAu\":false,\"GMOCreditcard\":false,\"GMOCvs\":false,\"GMODocomo\":false,\"GMOPaypal\":false,\"GMOPaypay\":false,\"GMOSoftbank\":false,\"MYCARD_ENABLED\":true,\"PAYPAL_ENABLED\":false,\"PINGPONG_ENABLED\":false,\"RAZER_ENABLED\":false,\"STEAM_ENABLED\":false,\"STRIPE_ENABLED\":false,\"TOSS_ENABLED\":false,\"WEBMONEY_ENABLED\":false},\"FACEBOOK_APPID\":\"\",\"FACEBOOK_CLIENT_TOKEN\":\"\",\"FACEBOOK_SECRET\":\"\",\"FIREBASE_ENABLED\":0,\"GMO_CC_JS\":\"https://\",\"GMO_CC_KEY\":\"\",\"GMO_CC_SHOPID\":\"\",\"GMO_PAY_CHANNEL\":{\"GMOAlipay\":false,\"GMOAu\":false,\"GMOCreditcard\":false,\"GMOCvs\":false,\"GMODocomo\":false,\"GMOPaypal\":false,\"GMOPaypay\":false,\"GMOSoftbank\":false},\"GMO_PAY_ENABLED\":false,\"GOOGLE_CLIENT_ID\":\"714708833202-757402ini3o2k7dq3ncht6q1bdmhd286.apps.googleusercontent.com\",\"GOOGLE_CLIENT_SECRET\":\"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\",\"GUEST_CREATE_METHOD\":0,\"GUIDE_POPUP\":{\"DATA\":null,\"ENABLE\":0},\"LOGIN\":{\"DEFAULT\":\"yostar\",\"ICON_SIZE\":\"big\",\"SORT\":[\"google\",\"apple\",\"device\"]},\"MYCARD_ENABLED\":true,\"ONE_STORE_LICENSE_KEY\":\"\",\"PAYPAL_ENABLED\":false,\"PINGPONG_ENABLED\":false,\"RAZER_ENABLED\":false,\"REMOTE_CONFIG\":[],\"SAMSUNG_SANDBOX_MODE\":false,\"STEAM_APPID\":\"\",\"STEAM_ENABLED\":false,\"STEAM_PAY_APPID\":\"\",\"STRIPE_ENABLED\":false,\"TOSS_ENABLED\":false,\"TWITTER_KEY\":\"\",\"TWITTER_SECRET\":\"\",\"WEBMONEY_ENABLED\":false}},\"Msg\":\"OK\"}");
            rsp.statusCode = 200;
            rsp.headers["Content-Type"] = "application/json";
            rsp.body = rspText;
        });
    co_await writer.WriteResponse(response);
}

AsyncTask<void> VersionHandler(RouteContext& context, const HttpRequest& request, HttpResponseWriter& writer)
{
    HttpResponse response;
    co_await context.Runtime().RunBlocking([&response]
        {
            HttpResponse& rsp = response;
            const char* rspText = U8("{\"Code\":200,\"Data\":{\"Agreement\":[{\"Version\":\"0.1\",\"Type\":\"privacy_agreement\",\"Title\":\"隐私政策\",\"Content\":\"\",\"Lang\":\"zh-Hant\"},{\"Version\":\"0.1\",\"Type\":\"user_agreement\",\"Title\":\"用户协议\",\"Content\":\"\",\"Lang\":\"zh-Hant\"}],\"ErrorCode\":\"3.0\"},\"Msg\":\"OK\"}");
            rsp.statusCode = 200;
            rsp.headers["Content-Type"] = "application/json";
            rsp.body = rspText;
        });
    co_await writer.WriteResponse(response);
}

AsyncTask<void> NoticeListHandler(RouteContext& context, const HttpRequest& request, HttpResponseWriter& writer)
{
    co_await context.Http().ProxyGet(request, writer);
}
