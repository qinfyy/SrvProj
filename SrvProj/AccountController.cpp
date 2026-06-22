#include "AccountController.h"
#include <iostream>
#include "AeadTool.h"
#include <nlohmann/json.hpp>
#include <google/protobuf/util/json_util.h>
#include <openssl/rand.h>
#include <cpprest/http_client.h>
#include <cpprest/http_msg.h>
#include "Util.h"
#include "Logger.h"
#include <vector>
#include <unordered_map>
#include "proto/dump.pb.h"
#include "DbMgr.h"
#include "ResultCode.h"
#include "GameServices.h"
#include "GameTime.h"


std::optional<ServerListMeta> GetServerList()
{
    try
    {
        web::http::client::http_client_config config;
        config.set_timeout(std::chrono::seconds(5));
        web::http::client::http_client client(U("https://nova-static.stargazer-games.com"), config);
        web::http::http_request request(web::http::methods::GET);
        request.set_request_uri(U("/meta/serverlist.html"));
        request.headers().add(U("User-Agent"), U("UnityPlayer/2022.3.62f2 (UnityWebRequest/1.0, libcurl/8.10.1-DEV)"));
        auto response = client.request(request).get();
        if (response.status_code() != 200)
            return std::nullopt;

        auto body = response.extract_vector().get();
        if (body.size() <= 16) {
            return std::nullopt;
        }
    
        std::array<char, 16> iv;
        memcpy(iv.data(), body.data(), 16);
        std::string cipher(reinterpret_cast<const char*>(body.data() + 16), body.size() - 16);
        std::string plain = AeadTool::DecryptAesCBCInfo(AeadTool::twServerMetaKey, std::string_view(iv.data(), iv.size()) , cipher);
        ServerListMeta obj;
        if (!obj.ParseFromString(plain))
            return std::nullopt;

        return obj;
    }
    catch (...)
    {
        return std::nullopt;
    }
}

void ServerListHandler(const HttpRequest& req, HttpResponse& rsp) {
    try {
        ServerListMeta meta;

        meta.set_version(128);

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
}

void NoticeListHandler(const HttpRequest& req, HttpResponse& rsp)
{
    try
    {
        web::http::client::http_client_config config;
        config.set_timeout(std::chrono::seconds(5));
        web::http::client::http_client client(U("https://nova-static.stargazer-games.com"), config);
        web::http::http_request request(web::http::methods::GET);
        request.set_request_uri(utility::conversions::to_string_t(req.path));
        request.headers().add(U("User-Agent"), U("UnityPlayer/2022.3.62f2 (UnityWebRequest/1.0, libcurl/8.10.1-DEV)"));

        auto response = client.request(request).get();
        rsp.statusCode = response.status_code();
        auto body = response.extract_vector().get();
        if (body.empty())
        {
            rsp.statusCode = 502;
            rsp.body = "Upstream request failed";
            return;
        }

        auto contentType = response.headers().content_type();
        if (!contentType.empty())
        {
            rsp.headers["Content-Type"] = utility::conversions::to_utf8string(contentType);
        }

        rsp.body.assign(reinterpret_cast<const char*>(body.data()), body.size());
    }
    catch (const std::exception& e)
    {
        rsp.statusCode = 500;
        rsp.body = std::string("Exception: ") + e.what();
    }
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

void QuickLoginHandler(const HttpRequest& req, HttpResponse& rsp) {
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
}

void LoginHandler(const HttpRequest& req, HttpResponse& rsp)
{
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
}

void DetailHandler(const HttpRequest& req, HttpResponse& rsp) {
    QuickLoginHandler(req, rsp);
}

void SmsHandler(const HttpRequest& req, HttpResponse& rsp)
{
    nlohmann::json rspJson = {
        {"Code", 200},
        {"Msg", "OK"},
        {"Data", nlohmann::json::object()}
    };

    rsp.statusCode = 200;
    rsp.headers["Content-Type"] = "application/json";
    rsp.body = rspJson.dump();
}

void AuthHandler(const HttpRequest& req, HttpResponse& rsp)
{
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
}

void CommonConfigHandler(const HttpRequest& req, HttpResponse& rsp) {
    const char* rspText = U8("{\"Code\":200,\"Data\":{\"AppConfig\":{\"ACCOUNT_RETRIEVAL\":{\"FIRST_LOGIN_POPUP\":false,\"LOGIN_POPUP\":false,\"PAGE_URL\":\"\"},\"AGREEMENT_POPUP_TYPE\":\"Browser\",\"APPLE_CURRENCY_BLOCK_LIST\":null,\"APPLE_TYPE_KEY\":\"apple\",\"APP_CLIENT_LANG\":[\"zh-Hant\"],\"APP_DEBUG\":1,\"APP_GL\":\"zh-Hant\",\"BIND_METHOD\":[\"google\",\"apple\"],\"CAPTCHA_ENABLED\":false,\"CLIENT_LOG_REPORTING\":{\"ENABLE\":true},\"CREDIT_INVESTIGATION\":\"0.0\",\"DESTROY_USER_DAYS\":15,\"DESTROY_USER_ENABLE\":1,\"DETECTION_ADDRESS\":{\"AUTO\":{\"DNS\":[\"https://nova-static.stargazer-games.com/meta/serverlist.html\",\"https://nova.stargazer-games.com\",\"https://jp-sdk-api.yostarplat.com\"],\"HTTP\":[\"https://nova-static.stargazer-games.com/meta/serverlist.html\",\"https://nova.stargazer-games.com\",\"https://jp-sdk-api.yostarplat.com\"],\"MTR\":[\"https://nova-static.stargazer-games.com/meta/serverlist.html\",\"https://nova.stargazer-games.com\",\"https://jp-sdk-api.yostarplat.com\"],\"PING\":[\"https://nova-static.stargazer-games.com/meta/serverlist.html\",\"https://nova.stargazer-games.com\",\"https://jp-sdk-api.yostarplat.com\"],\"TCP\":[\"https://nova-static.stargazer-games.com/meta/serverlist.html\",\"https://nova.stargazer-games.com\",\"https://jp-sdk-api.yostarplat.com\"]},\"ENABLE\":true,\"ENABLE_MANUAL\":true,\"INTERNET\":\"https://www.google.com\",\"INTERNET_ADDRESS\":\"https://www.google.com\",\"NETWORK_ENDPORINT\":\"https://ap-southeast-1.log.aliyuncs.com\",\"NETWORK_PROJECT\":\"yostar-oversea-netsdk-logging\",\"NETWORK_SECRET_KEY\":\"eyJhbGl5dW5fdWlkIjoiMTI5NDA1ODU3MDYyMTk5MCIsImlwYV9hcHBfaWQiOiJMNFFSSG1zNzdqdW5WSGNCWTZVd1ZLIiwic2VjX2tleSI6ImM2YzAxZDZhNTZkZjdlMTY3Yjg2MmFjM2EwYzQ5MTJlN2RmZTRmNjIxMTc1YTZkOGI5ZjcxYWJhYWY2YWNjYmQ2MTg1ZjVmMmYxMTVkMTczNjg5MGRlYWU0Nzg0MTI0NzFmZGNjMmRlOWUwMWMyNmJhOTdmZDA0YTJkM2IxZjUwIiwic2lnbiI6ImQxOGQwZTc0YjFhYWIzZmVlYWNmNDY2ZTYyYjQyMDZmYzA4NWFmMjJiN2ZjODQ1MDYzMjM3MDNlOGVkOGUxNGU5ZWI0ZGM3YjllOTFiNzE3NmUxZTBmYjBhOTU1OWQxMTFhM2QyMzU2YTQyNWQ1YTlkNGI1ZWMxMWQxYjY0NTBjIn0=\"},\"ENABLE_AGREEMENT\":true,\"ENABLE_MULTI_LANG_AGREEMENT\":false,\"ENABLE_TEXT_REVIEW\":true,\"ERROR_CODE\":\"3.0\",\"FILE_DOMAIN\":\"https://storage.googleapis.com/sdkplat-jp-prod\",\"GEETEST_ENABLE\":false,\"GEETEST_ID\":\"\",\"GOOGLE_ANALYTICS_MEASUREMENT_ID\":\"\",\"MIGRATE_POPUP\":true,\"NICKNAME_REG\":\"^[A-Za-z0-9]{2,20}$\",\"PASSPORT_DESTROY_DAYS\":15,\"POPUP\":{\"Data\":[{\"Lang\":\"ja\",\"Text\":\"Yostar IDを作成\"},{\"Lang\":\"en\",\"Text\":\"Create a Yostar account\"},{\"Lang\":\"kr\",\"Text\":\"YOSTAR 계정 가입하기\"},{\"Lang\":\"fr\",\"Text\":\"Créez votre compte Yostar\"},{\"Lang\":\"de\",\"Text\":\"Einen Yostar-Account erstellen\"}],\"Enable\":true},\"PRIVACY_AGREEMENT\":\"0.1\",\"RECHARGE_LIMIT\":{\"Enable\":false,\"IsOneLimit\":false,\"Items\":[],\"OneLimitAmount\":0},\"SHARE\":{\"CaptureScreen\":{\"AutoCloseDelay\":0,\"Enabled\":false},\"Facebook\":{\"AppID\":\"\",\"Enabled\":true},\"Instagram\":{\"Enabled\":false},\"Kakao\":{\"AppKey\":\"\",\"Enabled\":false},\"Naver\":{\"Enabled\":false},\"Twitter\":{\"Enabled\":true}},\"SLS\":{\"ACCESS_KEY_ID\":\"7b5d0ffd0943f26704fc547a871c68b1b5d56b5c9caeb354205b81f445d7af59\",\"ACCESS_KEY_SECRET\":\"4a5e9cc8a50819290c9bfa1fedc79da7c50e85189a05eb462a3d28a7688eabb0\",\"ENABLE\":true},\"SURVEY_POPUP_TYPE\":\"Browser\",\"UDATA\":{\"Enable\":true,\"URL\":\"https://udata-api-jp.open.yo-star.com\"},\"USER_AGREEMENT\":\"0.1\",\"YOSTAR_PREFIX\":\"user\"},\"EuropeUnion\":false,\"StoreConfig\":{\"ADJUST_APPID\":\"\",\"ADJUST_CHARGEEVENTTOKEN\":\"\",\"ADJUST_ENABLED\":0,\"ADJUST_EVENTTOKENS\":null,\"ADJUST_ISDEBUG\":0,\"AIRWALLEX_ENABLED\":false,\"AI_HELP\":{\"AihelpAppID\":\"yostar1_platform_0f3a2fdbf61ff427d92b6696a0cf8e30\",\"AihelpAppKey\":\"YOSTAR1_app_abf38d0a88924a40ac990b58865fa64d\",\"AihelpDomain\":\"yostar1.aihelp.net\",\"CustomerEmailAddr\":\"\",\"CustomerServiceURL\":\"\",\"CustomerWay\":1,\"DisplayType\":\"Browser\",\"Enable\":1,\"Mode\":\"robot\"},\"APPLEID\":\"\",\"CODA_ENABLED\":false,\"ENABLED_PAY\":{\"AIRWALLEX_ENABLED\":false,\"CODA_ENABLED\":false,\"GMOAlipay\":false,\"GMOAu\":false,\"GMOCreditcard\":false,\"GMOCvs\":false,\"GMODocomo\":false,\"GMOPaypal\":false,\"GMOPaypay\":false,\"GMOSoftbank\":false,\"MYCARD_ENABLED\":true,\"PAYPAL_ENABLED\":false,\"PINGPONG_ENABLED\":false,\"RAZER_ENABLED\":false,\"STEAM_ENABLED\":false,\"STRIPE_ENABLED\":false,\"TOSS_ENABLED\":false,\"WEBMONEY_ENABLED\":false},\"FACEBOOK_APPID\":\"\",\"FACEBOOK_CLIENT_TOKEN\":\"\",\"FACEBOOK_SECRET\":\"\",\"FIREBASE_ENABLED\":0,\"GMO_CC_JS\":\"https://\",\"GMO_CC_KEY\":\"\",\"GMO_CC_SHOPID\":\"\",\"GMO_PAY_CHANNEL\":{\"GMOAlipay\":false,\"GMOAu\":false,\"GMOCreditcard\":false,\"GMOCvs\":false,\"GMODocomo\":false,\"GMOPaypal\":false,\"GMOPaypay\":false,\"GMOSoftbank\":false},\"GMO_PAY_ENABLED\":false,\"GOOGLE_CLIENT_ID\":\"714708833202-757402ini3o2k7dq3ncht6q1bdmhd286.apps.googleusercontent.com\",\"GOOGLE_CLIENT_SECRET\":\"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\",\"GUEST_CREATE_METHOD\":0,\"GUIDE_POPUP\":{\"DATA\":null,\"ENABLE\":0},\"LOGIN\":{\"DEFAULT\":\"yostar\",\"ICON_SIZE\":\"big\",\"SORT\":[\"google\",\"apple\",\"device\"]},\"MYCARD_ENABLED\":true,\"ONE_STORE_LICENSE_KEY\":\"\",\"PAYPAL_ENABLED\":false,\"PINGPONG_ENABLED\":false,\"RAZER_ENABLED\":false,\"REMOTE_CONFIG\":[],\"SAMSUNG_SANDBOX_MODE\":false,\"STEAM_APPID\":\"\",\"STEAM_ENABLED\":false,\"STEAM_PAY_APPID\":\"\",\"STRIPE_ENABLED\":false,\"TOSS_ENABLED\":false,\"TWITTER_KEY\":\"\",\"TWITTER_SECRET\":\"\",\"WEBMONEY_ENABLED\":false}},\"Msg\":\"OK\"}");
    rsp.statusCode = 200;
    rsp.headers["Content-Type"] = "application/json";
    rsp.body = rspText;
}

void VersionHandler(const HttpRequest& req, HttpResponse& rsp) {
    const char* rspText = U8("{\"Code\":200,\"Data\":{\"Agreement\":[{\"Version\":\"0.1\",\"Type\":\"privacy_agreement\",\"Title\":\"隐私政策\",\"Content\":\"\",\"Lang\":\"zh-Hant\"},{\"Version\":\"0.1\",\"Type\":\"user_agreement\",\"Title\":\"用户协议\",\"Content\":\"\",\"Lang\":\"zh-Hant\"}],\"ErrorCode\":\"3.0\"},\"Msg\":\"OK\"}");
    rsp.statusCode = 200;
    rsp.headers["Content-Type"] = "application/json";
    rsp.body = rspText;
}

namespace
{
const char* kOrderProductsResponse = U8(
    "{\"Code\":200,\"Data\":{\"List\":["
    "{\"ID\":\"3742098088\",\"Name\":\"希娅_养成礼包\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.02_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742162634\",\"Name\":\"75 星之彩\",\"Price\":33,\"Desc\":\"\",\"StoreProductID\":\"com.yostar.stellasora.stellanitelumina75\",\"GameProductID\":\"gem.tier7\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742180328\",\"Name\":\"千都世_角色资源礼包\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.01_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742190926\",\"Name\":\"1015 星之彩\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.yostar.stellasora.stellanitelumina1015\",\"GameProductID\":\"gem.tier4\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742197373\",\"Name\":\"每周_角色资源礼包\",\"Price\":190,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_w\",\"GameProductID\":\"pack.01_role_w\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742208918\",\"Name\":\"皮肤_98\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin.98\",\"GameProductID\":\"skin.98.01\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742267540\",\"Name\":\"希娅_pu星盘券礼包\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc\",\"GameProductID\":\"pack.02_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742268311\",\"Name\":\"490 星之彩\",\"Price\":190,\"Desc\":\"\",\"StoreProductID\":\"com.yostar.stellasora.stellanitelumina490\",\"GameProductID\":\"gem.tier5\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742291527\",\"Name\":\"每月_pu角色券礼包\",\"Price\":600,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_m\",\"GameProductID\":\"pack.01_role_m\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742292395\",\"Name\":\"希娅_pu角色券礼包\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role\",\"GameProductID\":\"pack.02_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742319192\",\"Name\":\"8500 星之彩\",\"Price\":3000,\"Desc\":\"\",\"StoreProductID\":\"com.yostar.stellasora.stellanitelumina8500\",\"GameProductID\":\"gem.tier1\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742392240\",\"Name\":\"新手_SR角色自选礼包\",\"Price\":150,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.sr\",\"GameProductID\":\"pack.sr\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742392588\",\"Name\":\"开服_pu星盘券礼包\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.op_disc\",\"GameProductID\":\"pack.op_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742398399\",\"Name\":\"希娅_礼物礼包\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.02_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742410933\",\"Name\":\"98_BP\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.battlepass.98\",\"GameProductID\":\"battlepass.98\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742421531\",\"Name\":\"千都世_礼物礼包\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.01_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742464254\",\"Name\":\"每月_pu星盘券礼包\",\"Price\":600,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_m\",\"GameProductID\":\"pack.01_disc_m\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742528647\",\"Name\":\"新手_pu角色券礼包\",\"Price\":340,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role\",\"GameProductID\":\"pack.role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742542814\",\"Name\":\"新手_6元破冰礼包\",\"Price\":33,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.first\",\"GameProductID\":\"pack.first\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742543170\",\"Name\":\"4300 星之彩\",\"Price\":1600,\"Desc\":\"\",\"StoreProductID\":\"com.yostar.stellasora.stellanitelumina4300\",\"GameProductID\":\"gem.tier2\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742556595\",\"Name\":\"每周_星盘资源礼包\",\"Price\":190,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_w\",\"GameProductID\":\"pack.01_disc_w\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742577471\",\"Name\":\"开服_pu角色券礼包\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.op_role\",\"GameProductID\":\"pack.op_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742639592\",\"Name\":\"新手_pu星盘券礼包\",\"Price\":340,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc\",\"GameProductID\":\"pack.disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742663664\",\"Name\":\"月卡\",\"Price\":150,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.monthlycard.small\",\"GameProductID\":\"monthlyCard.small\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742807914\",\"Name\":\"2200 星之彩\",\"Price\":840,\"Desc\":\"\",\"StoreProductID\":\"com.yostar.stellasora.stellanitelumina2200\",\"GameProductID\":\"gem.tier3\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742822198\",\"Name\":\"230 星之彩\",\"Price\":90,\"Desc\":\"\",\"StoreProductID\":\"com.yostar.stellasora.stellanitelumina230\",\"GameProductID\":\"gem.tier6\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742909739\",\"Name\":\"新手_普池角色券礼包\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_common\",\"GameProductID\":\"pack.role_common\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742925108\",\"Name\":\"68_BP\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.battlepass.58\",\"GameProductID\":\"battlepass.58\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},"
    "{\"ID\":\"3742929455\",\"Name\":\"38_BP\",\"Price\":250,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.battlepass.50\",\"GameProductID\":\"battlepass.50\",\"CurrencyCode\":\"TWD\",\"ProductType\":1}"
    "]},\"Msg\":\"OK\"}");

struct HttpOrderProduct
{
    std::string ProductId;
    std::string StoreProductId;
    std::string GameProductId;
    int Price = 0;
    std::string Name;
};

const std::unordered_map<std::string, HttpOrderProduct>& GetHttpOrderProducts()
{
    static const std::unordered_map<std::string, HttpOrderProduct> kProducts = []() {
        std::unordered_map<std::string, HttpOrderProduct> out;
        try
        {
            const auto root = nlohmann::json::parse(kOrderProductsResponse);
            const auto& list = root["Data"]["List"];
            for (const auto& item : list)
            {
                HttpOrderProduct product;
                product.ProductId = item.value("ID", "");
                product.StoreProductId = item.value("StoreProductID", "");
                product.GameProductId = item.value("GameProductID", "");
                product.Price = item.value("Price", 0);
                product.Name = item.value("Name", "");
                if (!product.ProductId.empty())
                {
                    out.emplace(product.ProductId, std::move(product));
                }
            }
        }
        catch (const std::exception& e)
        {
            LOG_WARNING("解析商品列表失败: {}", e.what());
        }

        return out;
    }();

    return kProducts;
}

std::string MakeMockRedirectUrl(const std::string& orderId, const std::string& productId)
{
    return "http://127.0.0.1:21000/mock-pay?orderId=" + orderId + "&productId=" + productId;
}

std::string BuildLocalNotifyUrl()
{
    return "http://127.0.0.1:21000/order/notify";
}
}

void OrderProductsHandler(const HttpRequest& req, HttpResponse& rsp) {
    rsp.statusCode = 200;
    rsp.headers["Content-Type"] = "application/json; charset=utf-8";
    rsp.body = kOrderProductsResponse;
}

void OrderCreateHandler(const HttpRequest& req, HttpResponse& rsp)
{
    nlohmann::json reqJson;
    try
    {
        reqJson = nlohmann::json::parse(req.body);
    }
    catch (const std::exception& e)
    {
        LOG_WARNING("订单创建请求解析失败: {}", e.what());
        rsp.statusCode = 200;
        rsp.headers["Content-Type"] = "application/json; charset=utf-8";
        rsp.body = U8("{\"Code\":") + std::to_string(ResultCode::CLIENT_PARAMETER_ERROR) + U8(",\"Data\":{},\"Msg\":\"请求无效\"}");
        return;
    }

    const std::string productId = reqJson.value("ProductId", "");
    const std::string extraData = reqJson.value("ExtraData", "");
    const auto& products = GetHttpOrderProducts();
    const auto productIt = products.find(productId);
    if (productIt == products.end())
    {
        rsp.statusCode = 200;
        rsp.headers["Content-Type"] = "application/json; charset=utf-8";
        rsp.body = "{\"Code\":" + std::to_string(ResultCode::PAY_PRODUCTID_NOT_EXIST) + ",\"Data\":{},\"Msg\":\"商品不存在\"}";
        return;
    }

    GameServices::WebOrderContext context;
    const bool hasContext = GameServices::Instance().GetWebOrderContext(extraData, context);

    std::string orderId;
    if (!GenerateToken(orderId, false))
    {
        rsp.statusCode = 200;
        rsp.headers["Content-Type"] = "application/json; charset=utf-8";
        rsp.body = "{\"Code\":" + std::to_string(ResultCode::SERVER_ERROR) + ",\"Data\":{},\"Msg\":\"订单创建失败\"}";
        return;
    }

    const int64_t createdAt = GameTime::NowSeconds();
    const std::string redirectUrl = MakeMockRedirectUrl(orderId, productId);

    nlohmann::json order = {
        {"CreatedAt", createdAt},
        {"GameExtraData", extraData},
        {"ID", orderId},
        {"StoreName", "mock"},
        {"StoreProductID", productIt->second.StoreProductId}
    };

    if (hasContext)
    {
        order["LinkedGameOrderID"] = context.GameOrderId;
        order["LinkedSource"] = context.Source;
        order["LinkedProductKey"] = context.ProductKey;
        order["LinkedPlayerUID"] = context.PlayerUid;
    }

    nlohmann::json resp = {
        {"Code", 200},
        {"Data", {
            {"Order", order},
            {"PC", {
                {"RedirectURL", redirectUrl}
            }}
        }},
        {"Msg", "OK"}
    };

    rsp.statusCode = 200;
    rsp.headers["Content-Type"] = "application/json; charset=utf-8";
    rsp.body = resp.dump();
}

void MockPayPageHandler(const HttpRequest& req, HttpResponse& rsp)
{
    const std::string orderId = req.GetQueryParam("orderId");
    const std::string productId = req.GetQueryParam("productId");

    std::string html = U8(
        "<!doctype html><html><head><meta charset=\"utf-8\">"
        "<title>Mock Pay</title>"
        "<style>"
        "body{font-family:Segoe UI,Microsoft YaHei,sans-serif;background:#f6f3ed;color:#222;margin:0;}"
        ".wrap{max-width:720px;margin:64px auto;padding:32px;background:#fff;border:1px solid #ddd;border-radius:16px;box-shadow:0 8px 30px rgba(0,0,0,.06);}"
        "h1{margin-top:0;font-size:44px;}"
        "p{font-size:24px;line-height:1.6;}"
        "code{background:#f3f3f3;padding:2px 6px;border-radius:6px;font-size:22px;}"
        ".btn{display:inline-block;margin-top:20px;padding:12px 18px;background:#1f6feb;color:#fff;text-decoration:none;border-radius:10px;}"
        ".muted{color:#666;}"
        ".close-hint{font-size:20px;color:#888;margin-top:20px;}"
        "</style></head><body><div class=\"wrap\">"
        "<h1>Mock Pay</h1>"
        "<p>订单号：<code>") + orderId + U8("</code></p>"
            "<p>商品 ID：<code>") + productId + U8("</code></p>"
                "<p class=\"close-hint\">请点击左上角按钮关闭</p>"
                "</div></body></html>");

    rsp.statusCode = 200;
    rsp.headers["Content-Type"] = "text/html; charset=utf-8";
    rsp.body = html;
}

void OrderNotifyHandler(const HttpRequest& req, HttpResponse& rsp)
{
    nlohmann::json resp = {
        {"Code", 200},
        {"Data", nlohmann::json::object()},
        {"Msg", "OK"}
    };

    rsp.statusCode = 200;
    rsp.headers["Content-Type"] = "application/json; charset=utf-8";
    rsp.body = resp.dump();
}
