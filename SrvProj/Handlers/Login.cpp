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
#include "../proto/proto_cpp/player_ping.pb.h"

#include <chrono>

using namespace proto;

std::string ike_req__Handler(GameSession* session, const std::string& req)
{
    if (session) {
		LOG_ERROR("该令牌的会话已存在: {}", session->mToken);
        return GameSession::BuildMessage(ike_failed_ack);
    }

    IKEReq ikereq;
    ikereq.ParseFromString(req);

    session = GameServices::Instance().CreateSession();
	session->mClientPublicKey = ikereq.pubkey();
	bool succ1 = session->GenerateServerKey();
	if (!succ1) {
		return GameSession::BuildMessage(ike_failed_ack);
	}

    bool succ2 = session->CalKey();
	if (!succ2) {
		return GameSession::BuildMessage(ike_failed_ack);
	}

    IKEResp rsp;
    rsp.set_pubkey(session->mServerPublicKey);
    rsp.set_token(session->mToken);
    rsp.set_cipher(session->mEncryptFunction);
    rsp.set_serverts(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());

    return GameSession::BuildMessage(ike_succeed_ack, rsp.SerializeAsString());
}

std::string player_login_req__Handler(GameSession* session, const std::string& req) {
    if (!session) {
        return GameSession::BuildMessage(player_login_failed_ack);
    }

    LoginReq reqPb;
    reqPb.ParseFromString(req);

    std::string loginToken = "";
    if (reqPb.has_officialoverseas()) {
        loginToken = reqPb.officialoverseas().token();
    }
    else if (reqPb.has_official()) {
        loginToken = reqPb.official().token();
    }
    else if (reqPb.has_token()) {
        loginToken = reqPb.token();
    }

    bool loginSucc = session->Login(loginToken);
    if (!loginSucc) {
        Error errorPb;
        errorPb.set_code(100110); // ErrLogin
        return GameSession::BuildMessage(player_login_failed_ack, errorPb.SerializeAsString());
    }
    
    LoginResp rsp;
    rsp.set_token(session->mToken);

    return GameSession::BuildMessage(player_login_succeed_ack, rsp.SerializeAsString());
}

std::string player_data_req__Handler(GameSession* session, const std::string& req) {
	if (!session || !session->mPlayer) {
		return GameSession::BuildMessage(player_data_failed_ack);
	}

	auto playerData = session->mPlayer->ToProto();
    return GameSession::BuildMessage(player_data_succeed_ack, playerData.SerializeAsString());
}

std::string player_ping_req__Handler(GameSession* session, const std::string& req) {
    if (!session || !session->mPlayer) {
        return GameSession::BuildMessage(player_ping_failed_ack);
    }

    Pong pong;
    pong.set_serverts(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    session->SavePlayer();
    return GameSession::BuildMessage(player_ping_succeed_ack, pong.SerializeAsString());
}

std::string mall_package_list_req__Handler(GameSession* session, const std::string& req) {
    if (!session || !session->mPlayer) {
        return GameSession::BuildMessage(mall_package_list_failed_ack);
    }

    return GameSession::BuildMessage(mall_package_list_succeed_ack);
}
