#include "Login.h"
#include <string>
#include "../proto/NetMsgId.pb.h"
#include "../GameSession.h"
#include "../proto/proto_cpp/ike.pb.h"

using namespace proto;

void ike_req_Handler(GameSession* session, const std::string& req, std::string& rsp)
{
    if (session) {
        rsp = GameSession::BuildMessage(ike_failed_ack);
    }

    IKEReq ikereq;
    ikereq.ParseFromString(rsp);
}
