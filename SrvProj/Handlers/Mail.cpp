#include "Mail.h"

#include "../Game/MailMgr.h"
#include "../Game/Player.h"
#include "../GameSession.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/mail_pin.pb.h"
#include "../proto/proto_cpp/mail_recv.pb.h"
#include "../proto/proto_cpp/mail_remove.pb.h"
#include "../proto/proto_cpp/public.pb.h"

#include <vector>

namespace {
bool HasLoggedInPlayer(GameSession* session)
{
    return session && session->HasPlayer() && session->GetPlayer();
}

bool ParseMailRequest(const std::string& req, proto::MailRequest& out)
{
    if (req.empty())
    {
        return false;
    }

    return out.ParseFromString(req) && out.id() > 0;
}
}

std::string mail_list_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, mail_list_failed_ack);
    }

    auto rsp = session->GetPlayer()->Mails().ToProto();
    return EncodeReply(session, mail_list_succeed_ack, &rsp);
}

std::string mail_read_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, mail_read_failed_ack);
    }

    proto::MailRequest reqPb;
    if (!ParseMailRequest(req, reqPb) || !session->GetPlayer()->Mails().MarkRead(reqPb.id()))
    {
        return EncodeReply(session, mail_read_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, mail_read_succeed_ack);
}

std::string mail_recv_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, mail_recv_failed_ack);
    }

    proto::MailRecvResp rsp;
    if (req.empty())
    {
        if (!session->GetPlayer()->Mails().ReceiveAll(rsp))
        {
            return EncodeReply(session, mail_recv_failed_ack);
        }
    }
    else
    {
        proto::MailRequest reqPb;
        if (!ParseMailRequest(req, reqPb) || !session->GetPlayer()->Mails().Receive(reqPb.id(), rsp))
        {
            return EncodeReply(session, mail_recv_failed_ack);
        }
    }

    session->SavePlayer();
    return EncodeReply(session, mail_recv_succeed_ack, &rsp);
}

std::string mail_remove_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, mail_remove_failed_ack);
    }

    proto::MailRequest reqPb;
    if (!ParseMailRequest(req, reqPb))
    {
        return EncodeReply(session, mail_remove_failed_ack);
    }

    std::vector<uint32_t> removedIds;
    if (!session->GetPlayer()->Mails().Remove(reqPb.id(), removedIds))
    {
        return EncodeReply(session, mail_remove_failed_ack);
    }

    proto::MailRemoveResp rsp;
    for (uint32_t id : removedIds)
    {
        rsp.add_ids(id);
    }

    session->SavePlayer();
    return EncodeReply(session, mail_remove_succeed_ack, &rsp);
}

std::string mail_pin_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, mail_pin_failed_ack);
    }

    proto::MailPinRequest reqPb;
    if (!reqPb.ParseFromString(req) || reqPb.id() == 0 ||
        !session->GetPlayer()->Mails().SetPin(reqPb.id(), reqPb.pin(), reqPb.flag()))
    {
        return EncodeReply(session, mail_pin_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, mail_pin_succeed_ack);
}
