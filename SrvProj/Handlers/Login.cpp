#include "Login.h"
#include <string>
#include "../proto/NetMsgId.pb.h"
#include "../GameSession.h"
#include <google/protobuf/util/json_util.h>
#include "../Logger.h"
#include "../Util.h"
#include "../GameServices.h"

#include "../proto/proto_cpp/player_login.pb.h"
#include "../proto/proto_cpp/ike.pb.h"

using namespace proto;

std::string ike_req__Handler(GameSession* session, const std::string& req)
{
    if (session) {
		LOG_DEBUG("该令牌的会话已存在: {}", session->token.c_str());
        return GameSession::BuildMessage(ike_failed_ack);
    }

    IKEReq ikereq;
    ikereq.ParseFromString(req);

    session = GameServices::Instance().CreateSession();
	session->clientPublicKey = ikereq.pubkey();
	session->GenerateServerKey();
	session->CalKey();

    IKEResp rsp;
    rsp.set_pubkey(session->serverPublicKey);
    rsp.set_token(session->token);
    rsp.set_cipher(session->encryptFunction);
    rsp.set_serverts(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());

    return GameSession::BuildMessage(ike_succeed_ack, rsp.SerializeAsString());
}

std::string player_login_req__Handler(GameSession* session, const std::string& req) {
    LoginReq reqPb;
    reqPb.ParseFromString(req);

    std::string loginToken = "";
    if (reqPb.has_officialoverseas()) {
        loginToken = reqPb.officialoverseas().token();
    }

    session->platform = reqPb.platform();


    // Login
    bool loginSucc = session->Login(loginToken);
    if (!loginSucc) {
        Error errorPb;
        errorPb.set_code(100110); // ErrLogin
        return GameSession::BuildMessage(player_login_failed_ack, errorPb.SerializeAsString());
    }
    
    LoginResp rsp;
    rsp.set_token(session->token);

    return GameSession::BuildMessage(player_login_succeed_ack, rsp.SerializeAsString());


}

std::string player_data_req__Handler(GameSession* session, const std::string& req) {
    Sleep(0);
    return "";
}
