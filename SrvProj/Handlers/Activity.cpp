#include "Activity.h"
#include "../proto/NetMsgId.pb.h"

std::string activity_detail_req__Handler(GameSession* session, const std::string& req) {
    if (!session || !session->mPlayer) {
        return session->BuildMessage(activity_detail_failed_ack);
    }

    return session->BuildMessage(activity_detail_succeed_ack);
}
