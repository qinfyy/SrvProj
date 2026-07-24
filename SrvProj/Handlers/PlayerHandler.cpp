#include "PlayerHandler.h"

#include "../Command/CommandMgr.h"
#include "../Game/CharacterMgr.h"
#include "../Game/InventoryMgr.h"
#include "../Game/Player.h"
#include "../Game/QuestMgr.h"
#include "../GameSession.h"
#include "../proto/NetMsgId.h"
#include "../proto/proto_cpp/notify.pb.h"
#include "../proto/proto_cpp/phone_contacts_info.pb.h"
#include "../proto/proto_cpp/phone_contacts_report.pb.h"
#include "../proto/proto_cpp/player_board.pb.h"
#include "../proto/proto_cpp/player_chars_show.pb.h"
#include "../proto/proto_cpp/player_head_info.pb.h"
#include "../proto/proto_cpp/player_headicon_set.pb.h"
#include "../proto/proto_cpp/player_honor_edit.pb.h"
#include "../proto/proto_cpp/player_name_edit.pb.h"
#include "../proto/proto_cpp/player_signature_edit.pb.h"
#include "../proto/proto_cpp/player_skin_show.pb.h"
#include "../proto/proto_cpp/player_title_edit.pb.h"
#include "../proto/proto_cpp/player_world_class_reward_receive.pb.h"
#include "../proto/proto_cpp/public.pb.h"

namespace {
constexpr uint32_t kErrConfig = 119902;

bool HasPlayer(GameSession* session)
{
    return session && session->HasPlayer() && session->GetPlayer();
}
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


std::string phone_contacts_info_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, phone_contacts_info_failed_ack);
    }

    proto::PhoneContactsInfoResp resp;
    session->GetPlayer()->Characters().BuildPhoneContactsInfo(resp);
    return EncodeReply(session, phone_contacts_info_succeed_ack, &resp);
}

std::string phone_contacts_report_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, phone_contacts_report_failed_ack);
    }

    proto::PhoneContactsReportReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, phone_contacts_report_failed_ack);
    }

    proto::ChangeInfo change;
    if (!session->GetPlayer()->Characters().ReportPhoneContact(reqPb.chatid(), reqPb.process(), reqPb.options(), reqPb.end(), change))
    {
        return EncodeReply(session, phone_contacts_report_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, phone_contacts_report_succeed_ack, &change);
}

std::string phone_contacts_top_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, phone_contacts_top_failed_ack);
    }

    proto::UI32 reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->Characters().TogglePhoneContactTop(reqPb.value()))
    {
        return EncodeReply(session, phone_contacts_top_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, phone_contacts_top_succeed_ack);
}

std::string player_board_set_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, player_board_set_failed_ack);
    }

    proto::PlayerBoardSetReq reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->SetBoard(reqPb.ids()))
    {
        return EncodeReply(session, player_board_set_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, player_board_set_succeed_ack);
}

std::string player_chars_show_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, player_chars_show_failed_ack);
    }

    proto::PlayerCharsShowReq reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->SetShowChars(reqPb.charids()))
    {
        return EncodeReply(session, player_chars_show_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, player_chars_show_succeed_ack);
}

std::string player_gender_edit_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, player_gender_edit_failed_ack);
    }

    session->GetPlayer()->EditGender();
    session->SavePlayer();
    return EncodeReply(session, player_gender_edit_succeed_ack);
}

std::string player_head_icon_info_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, player_head_icon_info_failed_ack);
    }

    proto::PlayerHeadIconInfoResp resp;
    for (uint32_t id : session->GetPlayer()->Inventory().Bin().headicons())
    {
        resp.add_list(id);
    }
    return EncodeReply(session, player_head_icon_info_succeed_ack, &resp);
}

std::string player_head_icon_set_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, player_head_icon_set_failed_ack);
    }

    proto::PlayerHeadIconSetReq reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->EditHeadIcon(reqPb.headicon()))
    {
        return EncodeReply(session, player_head_icon_set_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, player_head_icon_set_succeed_ack);
}

std::string player_honor_edit_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, player_honor_edit_failed_ack);
    }

    proto::PlayerHonorEditReq reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->SetHonor(reqPb.list()))
    {
        return EncodeReply(session, player_honor_edit_failed_ack);
    }

    proto::HonorChangeNotify notify;
    for (int honorId : session->GetPlayer()->GetPlayerData().honor())
    {
        notify.add_honors()->set_id(static_cast<uint32_t>(honorId));
    }
    session->GetPlayer()->PushNextPackage(honor_change_notify, notify);
    session->SavePlayer();

    proto::Nil resp;
    return EncodeReply(session, player_honor_edit_succeed_ack, &resp);
}

std::string player_music_set_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, player_music_set_failed_ack);
    }

    proto::I64 reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->SetMusic(reqPb.value()))
    {
        return EncodeReply(session, player_music_set_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, player_music_set_succeed_ack);
}

std::string player_name_edit_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, player_name_edit_failed_ack);
    }

    proto::PlayerNameEditReq reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->EditName(reqPb.name()))
    {
        return EncodeReply(session, player_name_edit_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, player_name_edit_succeed_ack);
}

std::string player_skin_show_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, player_skin_show_failed_ack);
    }

    proto::PlayerSkinShowReq reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->SetSkin(reqPb.skinid()))
    {
        return EncodeReply(session, player_skin_show_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, player_skin_show_succeed_ack);
}

std::string player_title_edit_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, player_title_edit_failed_ack);
    }

    proto::PlayerTitleEditReq reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->EditTitle(reqPb.titleprefix(), reqPb.titlesuffix()))
    {
        return EncodeReply(session, player_title_edit_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, player_title_edit_succeed_ack);
}

std::string player_world_class_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, player_world_class_reward_receive_failed_ack);
    }

    proto::PlayerWorldClassRewardReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, player_world_class_reward_receive_failed_ack);
    }

    proto::ChangeInfo change;
    if (!session->GetPlayer()->Quests().ReceiveWorldClassReward(reqPb.class_(), change))
    {
        return EncodeReply(session, player_world_class_reward_receive_failed_ack);
    }

    proto::WorldClassRewardState notify;
    notify.set_flag(session->GetPlayer()->Quests().GetWorldClassRewardFlag());
    session->GetPlayer()->PushNextPackage(world_class_reward_state_notify, notify);
    session->SavePlayer();
    return EncodeReply(session, player_world_class_reward_receive_succeed_ack, &change);
}
