#include "Login.h"

#include <string>
#include <span>
#include <vector>

#include "../DbMgr.h"
#include "../GameTime.h"
#include "../GameServices.h"
#include "../GameSession.h"
#include "../Logger.h"
#include "../Util.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/ike.pb.h"
#include "../proto/proto_cpp/player_login.pb.h"
#include "../proto/proto_cpp/player_ping.pb.h"
#include "../proto/proto_cpp/player_reg.pb.h"

using namespace proto;

std::string ike_req__Handler(GameSession* session, const std::string& req)
{
    if (session) {
        LOG_ERROR("该令牌的会话已存在: {}", session->mToken);
        return EncodeReply(session, ike_failed_ack);
    }

    IKEReq ikereq;
    ikereq.ParseFromString(req);

    session = GameServices::Instance().CreateSession();
    session->mClientPublicKey = ikereq.pubkey();
    bool succ1 = session->GenerateServerKey();
    if (!succ1) {
        return EncodeReply(session, ike_failed_ack);
    }

    bool succ2 = session->CalKey();
    if (!succ2) {
        return EncodeReply(session, ike_failed_ack);
    }

    IKEResp rsp;
    rsp.set_pubkey(session->mServerPublicKey);
    rsp.set_token(session->mToken);
    rsp.set_cipher(session->mEncryptFunction);
    rsp.set_serverts(GameTime::NowSeconds());

    return EncodeReply(session, ike_succeed_ack, &rsp);
}

std::string player_login_req__Handler(GameSession* session, const std::string& req) {
    if (!session) {
        return EncodeReply(session, player_login_failed_ack);
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
        errorPb.set_code(100110);
        return EncodeReply(session, player_login_failed_ack, &errorPb);
    }
    
    LoginResp rsp;
    rsp.set_token(session->mToken);

    return EncodeReply(session, player_login_succeed_ack, &rsp);
}

std::string player_data_req__Handler(GameSession* session, const std::string& req) {
    if (!session) {
        return EncodeReply(session, player_data_failed_ack);
    }

    if (!session->HasPlayer()) {
        return EncodeReply(session, player_new_notify);
    }

    auto playerData = session->GetPlayer()->ToProto();
    return EncodeReply(session, player_data_succeed_ack, &playerData);
}

std::string player_reg_req__Handler(GameSession* session, const std::string& req) {
    if (!session || session->HasPlayer() || session->mAccountUid.empty()) {
        return EncodeReply(session, player_reg_failed_ack);
    }

    PlayerReg regReq;
    if (!regReq.ParseFromString(req)) {
        return EncodeReply(session, player_reg_failed_ack);
    }

    if (regReq.nickname().empty()) {
        return EncodeReply(session, player_reg_failed_ack);
    }

    std::string nickname = regReq.nickname();
    if (nickname.size() > 20) {
        nickname.resize(20);
    }

    auto player = std::make_unique<Player>(session);
    if (!player->InitNewPlayer(0, nickname, regReq.gender())) {
        return EncodeReply(session, player_reg_failed_ack);
    }

    auto saveData = player->SaveToBlob();
    if (!DbMgr::Instance().CreatePlayer(0, session->mAccountUid, std::span<const uint8_t>(saveData.data(), saveData.size()))) {
        LOG_ERROR("无法创建玩家存档数据, accountUid: {}", session->mAccountUid);
        return EncodeReply(session, player_reg_failed_ack);
    }

    uint32_t assignedUid = 0;
    std::vector<uint8_t> createdBlob;
    if (!DbMgr::Instance().LoadPlayerByAccountUid(session->mAccountUid, assignedUid, createdBlob)) {
        LOG_ERROR("无法加载新创建的玩家数据, accountUid: {}", session->mAccountUid);
        return EncodeReply(session, player_reg_failed_ack);
    }

    player->SetUid(assignedUid);
    session->SetPlayer(std::move(player));
    session->SavePlayer();

    auto playerData = session->GetPlayer()->ToProto();
    return EncodeReply(session, player_data_succeed_ack, &playerData);
}

std::string player_ping_req__Handler(GameSession* session, const std::string& req) {
    if (!session || !session->HasPlayer()) {
        return EncodeReply(session, player_ping_failed_ack);
    }

    Pong pong;
    pong.set_serverts(GameTime::NowSeconds());
    session->SavePlayer();
    return EncodeReply(session, player_ping_succeed_ack, &pong);
}

std::string energy_info_req__Handler(GameSession* session, const std::string& req) {
    if (!session || !session->HasPlayer()) {
        return EncodeReply(session, energy_info_failed_ack);
    }

    auto energy = session->GetPlayer()->GetEnergyProto();
    session->SavePlayer();
    return EncodeReply(session, energy_info_succeed_ack, &energy);
}

std::string potential_preselection_list_req__Handler(GameSession* session, const std::string& req) {
    if (!session || !session->HasPlayer()) {
        return EncodeReply(session, potential_preselection_list_failed_ack);
    }

    return EncodeReply(session, potential_preselection_list_succeed_ack);
}
