#include "PlayerHandler.h"

#include "../Command/CommandMgr.h"
#include "../Game/Player.h"
#include "../GameSession.h"
#include "../proto/NetMsgId.h"
#include "../proto/proto_cpp/player_signature_edit.pb.h"
#include "../proto/proto_cpp/public.pb.h"

namespace {
constexpr uint32_t kErrConfig = 119902;
}

std::string player_learn_req__Handler(GameSession* session, const std::string& req)
{
    if (!session || !session->HasPlayer() || !session->GetPlayer())
    {
        return EncodeReply(session, player_learn_failed_ack);
    }

    proto::NewbieInfo reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, player_learn_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, player_learn_succeed_ack);
}

std::string player_signature_edit_req__Handler(GameSession* session, const std::string& req)
{
    if (!session || !session->HasPlayer())
    {
        return EncodeReply(session, player_signature_edit_failed_ack);
    }

    proto::PlayerSignatureEditReq reqPb;
    if (!reqPb.ParseFromString(req) || reqPb.signature().empty())
    {
        return EncodeReply(session, player_signature_edit_failed_ack);
    }

    const std::string& signature = reqPb.signature();
    if (CommandMgr::HasCommandPrefix(signature))
    {
        auto result = CommandMgr::Instance().Invoke(session->GetPlayer(), signature);

        proto::Error error;
        error.set_code(kErrConfig);
        error.add_arguments("\nCommand Result: " + result.message);
        session->SavePlayer();
        return EncodeReply(session, player_signature_edit_failed_ack, &error);
    }

    session->GetPlayer()->SetSignature(signature);
    session->SavePlayer();
    return EncodeReply(session, player_signature_edit_succeed_ack);
}
