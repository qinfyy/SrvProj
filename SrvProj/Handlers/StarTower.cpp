#include "StarTower.h"

#include "../Game/FormationMgr.h"
#include "../Game/Player.h"
#include "../Game/StarTowerMgr.h"
#include "../GameSession.h"
#include "../proto/NetMsgId.h"
#include "../proto/proto_cpp/player_formation.pb.h"
#include "../proto/proto_cpp/potential_preselection_delete.pb.h"
#include "../proto/proto_cpp/potential_preselection_import.pb.h"
#include "../proto/proto_cpp/potential_preselection_list.pb.h"
#include "../proto/proto_cpp/potential_preselection_name_set.pb.h"
#include "../proto/proto_cpp/potential_preselection_preference_set.pb.h"
#include "../proto/proto_cpp/potential_preselection_update.pb.h"
#include "../proto/proto_cpp/public.pb.h"
#include "../proto/proto_cpp/star_tower_apply.pb.h"
#include "../proto/proto_cpp/star_tower_build_brief_list_get.pb.h"
#include "../proto/proto_cpp/star_tower_build_delete.pb.h"
#include "../proto/proto_cpp/star_tower_build_detail_get.pb.h"
#include "../proto/proto_cpp/star_tower_build_lock_unlock.pb.h"
#include "../proto/proto_cpp/star_tower_build_name_set.pb.h"
#include "../proto/proto_cpp/star_tower_build_preference_set.pb.h"
#include "../proto/proto_cpp/star_tower_build_whether_save.pb.h"
#include "../proto/proto_cpp/star_tower_give_up.pb.h"
#include "../proto/proto_cpp/star_tower_info.pb.h"
#include "../proto/proto_cpp/star_tower_interact.pb.h"
#include "../proto/proto_cpp/star_tower_book_char_potential_get.pb.h"
#include "../proto/proto_cpp/star_tower_book_event_reward_receive.pb.h"
#include "../proto/proto_cpp/star_tower_book_potential_brief_list_get.pb.h"
#include "../proto/proto_cpp/star_tower_book_potential_reward_receive.pb.h"
#include "../proto/proto_cpp/npc_affinity_book_get.pb.h"
#include "../proto/proto_cpp/npc_affinity_plot_reward_receive.pb.h"
#include "../proto/proto_cpp/tower_book_fate_card_detail.pb.h"
#include "../proto/proto_cpp/tower_book_fate_card_reward_receive.pb.h"
#include "../proto/proto_cpp/tower_growth_detail.pb.h"
#include "../proto/proto_cpp/tower_growth_group_node_unlock.pb.h"
#include "../proto/proto_cpp/tower_growth_node_unlock.pb.h"

std::string player_formation_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, player_formation_failed_ack);
    }

    proto::PlayerFormationReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, player_formation_failed_ack);
    }

    const bool success = session->GetPlayer()->Formations().UpdateFormation(reqPb.formation());
    if (success)
    {
        session->SavePlayer();
    }

    return EncodeReply(session, success ? player_formation_succeed_ack : player_formation_failed_ack);
}

std::string potential_preselection_list_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, potential_preselection_list_failed_ack);
    }

    proto::PotentialPreselectionList rsp;
    session->GetPlayer()->StarTowers().BuildPresetList(rsp);
    return EncodeReply(session, potential_preselection_list_succeed_ack, &rsp);
}

std::string potential_preselection_import_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, potential_preselection_import_failed_ack);
    }

    proto::PotentialPreselectionImportReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, potential_preselection_import_failed_ack);
    }

    proto::PotentialPreselection rsp;
    if (!session->GetPlayer()->StarTowers().ImportPreset(reqPb.name(), reqPb.preference(), reqPb.charpotentials(), rsp))
    {
        return EncodeReply(session, potential_preselection_import_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, potential_preselection_import_succeed_ack, &rsp);
}

std::string potential_preselection_name_set_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, potential_preselection_name_set_failed_ack);
    }

    proto::PotentialPreselectionNameSetReq reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->StarTowers().SetPresetName(reqPb.id(), reqPb.name()))
    {
        return EncodeReply(session, potential_preselection_name_set_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, potential_preselection_name_set_succeed_ack);
}

std::string potential_preselection_preference_set_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, potential_preselection_preference_set_failed_ack);
    }

    proto::PotentialPreselectionPreferenceSetReq reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->StarTowers().SetPresetPreference(reqPb.checkinids(), reqPb.checkoutids()))
    {
        return EncodeReply(session, potential_preselection_preference_set_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, potential_preselection_preference_set_succeed_ack);
}

std::string potential_preselection_update_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, potential_preselection_update_failed_ack);
    }

    proto::PotentialPreselectionUpdateReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, potential_preselection_update_failed_ack);
    }

    proto::PotentialPreselection rsp;
    if (!session->GetPlayer()->StarTowers().UpdatePreset(reqPb.id(), reqPb.charpotentials(), rsp))
    {
        return EncodeReply(session, potential_preselection_update_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, potential_preselection_update_succeed_ack, &rsp);
}

std::string potential_preselection_delete_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, potential_preselection_delete_failed_ack);
    }

    proto::PotentialPreselectionDeleteReq reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->StarTowers().DeletePresets(reqPb.ids()))
    {
        return EncodeReply(session, potential_preselection_delete_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, potential_preselection_delete_succeed_ack);
}

std::string star_tower_build_brief_list_get_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_build_brief_list_get_failed_ack);
    }

    proto::StarTowerBuildBriefListGetResp rsp;
    session->GetPlayer()->StarTowers().BuildBriefList(rsp);
    return EncodeReply(session, star_tower_build_brief_list_get_succeed_ack, &rsp);
}

std::string star_tower_apply_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_apply_failed_ack);
    }

    proto::StarTowerApplyReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, star_tower_apply_failed_ack);
    }

    proto::StarTowerApplyResp rsp;
    if (!session->GetPlayer()->StarTowers().Apply(reqPb, rsp))
    {
        return EncodeReply(session, star_tower_apply_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, star_tower_apply_succeed_ack, &rsp);
}

std::string star_tower_build_delete_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_build_delete_failed_ack);
    }

    proto::StarTowerBuildDeleteReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, star_tower_build_delete_failed_ack);
    }

    proto::StarTowerBuildDeleteResp rsp;
    session->GetPlayer()->StarTowers().DeleteBuilds(reqPb.buildids(), rsp);
    session->SavePlayer();
    return EncodeReply(session, star_tower_build_delete_succeed_ack, &rsp);
}

std::string star_tower_build_detail_get_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_build_detail_get_failed_ack);
    }

    proto::StarTowerBuildDetailGetReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, star_tower_build_detail_get_failed_ack);
    }

    proto::StarTowerBuildDetailGetResp rsp;
    if (!session->GetPlayer()->StarTowers().BuildDetail(reqPb.buildid(), rsp))
    {
        return EncodeReply(session, star_tower_build_detail_get_failed_ack);
    }

    return EncodeReply(session, star_tower_build_detail_get_succeed_ack, &rsp);
}

std::string star_tower_build_lock_unlock_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_build_lock_unlock_failed_ack);
    }

    proto::StarTowerBuildLockUnlockReq reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->StarTowers().SetBuildLock(reqPb.buildid(), reqPb.lock()))
    {
        return EncodeReply(session, star_tower_build_lock_unlock_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, star_tower_build_lock_unlock_succeed_ack);
}

std::string star_tower_build_name_set_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_build_name_set_failed_ack);
    }

    proto::StarTowerBuildNameSetReq reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->StarTowers().SetBuildName(reqPb.buildid(), reqPb.name()))
    {
        return EncodeReply(session, star_tower_build_name_set_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, star_tower_build_name_set_succeed_ack);
}

std::string star_tower_build_preference_set_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_build_preference_set_failed_ack);
    }

    proto::StarTowerBuildPreferenceSetReq reqPb;
    if (!reqPb.ParseFromString(req) || !session->GetPlayer()->StarTowers().SetBuildPreference(reqPb.checkinids(), reqPb.checkoutids()))
    {
        return EncodeReply(session, star_tower_build_preference_set_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, star_tower_build_preference_set_succeed_ack);
}

std::string star_tower_build_whether_save_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_build_whether_save_failed_ack);
    }

    proto::StarTowerBuildWhetherSaveReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, star_tower_build_whether_save_failed_ack);
    }

    proto::StarTowerBuildWhetherSaveResp rsp;
    if (!session->GetPlayer()->StarTowers().SaveLastBuild(reqPb.delete_(), reqPb.buildname(), reqPb.lock(), rsp))
    {
        session->SavePlayer();
        return EncodeReply(session, star_tower_build_whether_save_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, star_tower_build_whether_save_succeed_ack, &rsp);
}

std::string star_tower_give_up_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_give_up_failed_ack);
    }

    proto::StarTowerGiveUpResp rsp;
    if (!session->GetPlayer()->StarTowers().GiveUp(rsp))
    {
        return EncodeReply(session, star_tower_give_up_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, star_tower_give_up_succeed_ack, &rsp);
}

std::string star_tower_info_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_info_failed_ack);
    }

    proto::StarTowerInfo rsp;
    if (!session->GetPlayer()->StarTowers().HandleInfo(rsp))
    {
        return EncodeReply(session, star_tower_info_failed_ack);
    }

    return EncodeReply(session, star_tower_info_succeed_ack, &rsp);
}

std::string star_tower_interact_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_interact_failed_ack);
    }

    proto::StarTowerInteractReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, star_tower_interact_failed_ack);
    }

    proto::StarTowerInteractResp rsp;
    if (!session->GetPlayer()->StarTowers().HandleInteract(reqPb, rsp))
    {
        return EncodeReply(session, star_tower_interact_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, star_tower_interact_succeed_ack, &rsp);
}

std::string star_tower_book_potential_brief_list_get_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_book_potential_brief_list_get_failed_ack);
    }

    proto::StarTowerBookPotentialBriefListResp rsp;
    if (!session->GetPlayer()->StarTowers().BuildPotentialBriefList(rsp))
    {
        return EncodeReply(session, star_tower_book_potential_brief_list_get_failed_ack);
    }

    return EncodeReply(session, star_tower_book_potential_brief_list_get_succeed_ack, &rsp);
}

std::string star_tower_book_char_potential_get_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_book_char_potential_get_failed_ack);
    }

    proto::UI32 reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, star_tower_book_char_potential_get_failed_ack);
    }

    proto::StarTowerBookPotentialGetResp rsp;
    if (!session->GetPlayer()->StarTowers().BuildCharPotential(reqPb.value(), rsp))
    {
        return EncodeReply(session, star_tower_book_char_potential_get_failed_ack);
    }

    return EncodeReply(session, star_tower_book_char_potential_get_succeed_ack, &rsp);
}

std::string star_tower_book_potential_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_book_potential_reward_receive_failed_ack);
    }

    proto::UI32 reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, star_tower_book_potential_reward_receive_failed_ack);
    }

    proto::StarTowerBookPotentialRewardReceiveResp rsp;
    if (!session->GetPlayer()->StarTowers().ReceivePotentialBookReward(reqPb.value(), rsp))
    {
        return EncodeReply(session, star_tower_book_potential_reward_receive_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, star_tower_book_potential_reward_receive_succeed_ack, &rsp);
}

std::string star_tower_book_event_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, star_tower_book_event_reward_receive_failed_ack);
    }

    proto::UI32 reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, star_tower_book_event_reward_receive_failed_ack);
    }

    proto::StarTowerBookEventRewardReceiveResp rsp;
    if (!session->GetPlayer()->StarTowers().ReceiveEventBookReward(reqPb.value(), rsp))
    {
        return EncodeReply(session, star_tower_book_event_reward_receive_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, star_tower_book_event_reward_receive_succeed_ack, &rsp);
}

std::string tower_book_fate_card_detail_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, tower_book_fate_card_detail_failed_ack);
    }

    proto::TowerBookFateCardDetailResp rsp;
    session->GetPlayer()->StarTowers().BuildFateCardDetail(rsp);
    return EncodeReply(session, tower_book_fate_card_detail_succeed_ack, &rsp);
}

std::string tower_book_fate_card_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, tower_book_fate_card_reward_receive_failed_ack);
    }

    proto::TowerBookFateCardRewardReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, tower_book_fate_card_reward_receive_failed_ack);
    }

    proto::ChangeInfo rsp;
    if (!session->GetPlayer()->StarTowers().ReceiveFateCardReward(reqPb.cardbundleid(), reqPb.questid(), rsp))
    {
        return EncodeReply(session, tower_book_fate_card_reward_receive_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, tower_book_fate_card_reward_receive_succeed_ack, &rsp);
}

std::string npc_affinity_book_get_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, npc_affinity_book_get_failed_ack);
    }

    proto::NPCAffinityBookGetResp rsp;
    if (!session->GetPlayer()->StarTowers().BuildNpcAffinityBook(rsp))
    {
        return EncodeReply(session, npc_affinity_book_get_failed_ack);
    }

    return EncodeReply(session, npc_affinity_book_get_succeed_ack, &rsp);
}

std::string npc_affinity_plot_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, npc_affinity_plot_reward_receive_failed_ack);
    }

    proto::UI32 reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, npc_affinity_plot_reward_receive_failed_ack);
    }

    proto::NPCAffinityPlotRewardReceiveResp rsp;
    if (!session->GetPlayer()->StarTowers().ReceiveNpcAffinityPlotReward(reqPb.value(), rsp))
    {
        return EncodeReply(session, npc_affinity_plot_reward_receive_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, npc_affinity_plot_reward_receive_succeed_ack, &rsp);
}

std::string tower_growth_detail_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, tower_growth_detail_failed_ack);
    }

    proto::TowerGrowthDetailResp rsp;
    session->GetPlayer()->StarTowers().BuildGrowthDetail(rsp);
    return EncodeReply(session, tower_growth_detail_succeed_ack, &rsp);
}

std::string tower_growth_group_node_unlock_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, tower_growth_group_node_unlock_failed_ack);
    }

    proto::UI32 reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, tower_growth_group_node_unlock_failed_ack);
    }

    proto::TowerGrowthGroupNodeUnlockResp rsp;
    if (!session->GetPlayer()->StarTowers().UnlockGrowthGroup(reqPb.value(), rsp))
    {
        return EncodeReply(session, tower_growth_group_node_unlock_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, tower_growth_group_node_unlock_succeed_ack, &rsp);
}

std::string tower_growth_node_unlock_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, tower_growth_node_unlock_failed_ack);
    }

    proto::UI32 reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, tower_growth_node_unlock_failed_ack);
    }

    proto::ChangeInfo rsp;
    if (!session->GetPlayer()->StarTowers().UnlockGrowthNode(reqPb.value(), rsp))
    {
        return EncodeReply(session, tower_growth_node_unlock_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, tower_growth_node_unlock_succeed_ack, &rsp);
}
