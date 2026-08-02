#include "Character.h"

#include "../Game/CharacterMgr.h"
#include "../Game/InventoryMgr.h"
#include "../Game/Player.h"
#include "../GameSession.h"
#include "../proto/NetMsgId.h"
#include "../proto/proto_cpp/char_affinity_gift_send.pb.h"
#include "../proto/proto_cpp/char_gem_equip_gem.pb.h"
#include "../proto/proto_cpp/char_gem_refresh.pb.h"
#include "../proto/proto_cpp/char_gem_replace_attribute.pb.h"
#include "../proto/proto_cpp/char_gem_update_gem_lock_status.pb.h"
#include "../proto/proto_cpp/char_gem_use_overlock.pb.h"
#include "../proto/proto_cpp/char_gem_use_preset.pb.h"
#include "../proto/proto_cpp/char_skill_upgrade.pb.h"
#include "../proto/proto_cpp/char_skin_set.pb.h"
#include "../proto/proto_cpp/char_upgrade.pb.h"
#include "../proto/proto_cpp/public.pb.h"

namespace {

ItemParamMap FromItemTpls(const google::protobuf::RepeatedPtrField<proto::ItemTpl>& items)
{
    ItemParamMap out;
    for (const auto& item : items)
    {
        out.Add(static_cast<int>(item.tid()), item.qty());
    }
    return out;
}
}

std::string char_upgrade_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, char_upgrade_failed_ack);
    }

    proto::CharUpgradeReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, char_upgrade_failed_ack);
    }

    proto::CharUpgradeResp response;
    if (!session->GetPlayer()->Characters().UpgradeCharacter(request.charid(), FromItemTpls(request.items()), response))
    {
        return EncodeReply(session, char_upgrade_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, char_upgrade_succeed_ack, &response);
}

std::string char_advance_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, char_advance_failed_ack);
    }

    proto::UI32 request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, char_advance_failed_ack);
    }

    proto::ChangeInfo change;
    if (!session->GetPlayer()->Characters().AdvanceCharacter(request.value(), change))
    {
        return EncodeReply(session, char_advance_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, char_advance_succeed_ack, &change);
}

std::string char_skill_upgrade_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, char_skill_upgrade_failed_ack);
    }

    proto::CharSkillUpgradeReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, char_skill_upgrade_failed_ack);
    }

    proto::ChangeInfo change;
    if (!session->GetPlayer()->Characters().UpgradeCharacterSkill(request.charid(), request.index(), change))
    {
        return EncodeReply(session, char_skill_upgrade_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, char_skill_upgrade_succeed_ack, &change);
}

std::string char_skin_set_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, char_skin_set_failed_ack);
    }

    proto::CharSkinSetReq request;
    if (!request.ParseFromString(req) || !session->GetPlayer()->Characters().SetCharacterSkin(request.charid(), request.skinid()))
    {
        return EncodeReply(session, char_skin_set_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, char_skin_set_succeed_ack);
}

std::string char_affinity_gift_send_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, char_affinity_gift_send_failed_ack);
    }

    proto::CharAffinityGiftSendReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, char_affinity_gift_send_failed_ack);
    }

    proto::CharAffinityGiftSendResp response;
    if (!session->GetPlayer()->Characters().SendAffinityGift(request.charid(), FromItemTpls(request.items()), response))
    {
        return EncodeReply(session, char_affinity_gift_send_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, char_affinity_gift_send_succeed_ack, &response);
}

std::string char_favorite_set_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, char_favorite_set_failed_ack);
    }

    proto::UI32 request;
    if (!request.ParseFromString(req) || !session->GetPlayer()->Characters().ToggleCharacterFavorite(request.value()))
    {
        return EncodeReply(session, char_favorite_set_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, char_favorite_set_succeed_ack);
}

std::string char_gem_use_preset_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, char_gem_use_preset_failed_ack);
    }

    proto::CharGemUsePresetReq request;
    if (!request.ParseFromString(req) || !session->GetPlayer()->Characters().UseGemPreset(request.charid(), request.presetid()))
    {
        return EncodeReply(session, char_gem_use_preset_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, char_gem_use_preset_succeed_ack);
}

std::string char_gem_equip_gem_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, char_gem_equip_gem_failed_ack);
    }

    proto::CharGemEquipGemReq request;
    if (!request.ParseFromString(req) || !session->GetPlayer()->Characters().EquipGem(request.charid(), request.slotid(), request.gemindex(), request.presetid()))
    {
        return EncodeReply(session, char_gem_equip_gem_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, char_gem_equip_gem_succeed_ack);
}

std::string char_gem_refresh_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, char_gem_refresh_failed_ack);
    }

    proto::CharGemRefreshReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, char_gem_refresh_failed_ack);
    }

    proto::CharGemRefreshResp response;
    if (!session->GetPlayer()->Characters().RefreshGem(request.charid(), request.slotid(), request.gemindex(), request.lockattrs(), response))
    {
        return EncodeReply(session, char_gem_refresh_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, char_gem_refresh_succeed_ack, &response);
}

std::string char_gem_replace_attribute_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, char_gem_replace_attribute_failed_ack);
    }

    proto::CharGemReplaceAttributeReq request;
    if (!request.ParseFromString(req) || !session->GetPlayer()->Characters().ReplaceGemAttribute(request.charid(), request.slotid(), request.gemindex()))
    {
        return EncodeReply(session, char_gem_replace_attribute_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, char_gem_replace_attribute_succeed_ack);
}

std::string char_gem_update_gem_lock_status_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, char_gem_update_gem_lock_status_failed_ack);
    }

    proto::CharGemUpdateGemLockStatusReq request;
    if (!request.ParseFromString(req) || !session->GetPlayer()->Characters().SetGemLock(request.charid(), request.slotid(), request.gemindex(), request.lock()))
    {
        return EncodeReply(session, char_gem_update_gem_lock_status_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, char_gem_update_gem_lock_status_succeed_ack);
}

std::string char_gem_overlock_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, char_gem_overlock_failed_ack);
    }

    proto::CharGemOverlockReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, char_gem_overlock_failed_ack);
    }

    proto::ChangeInfo change;
    if (!session->GetPlayer()->Characters().OverlockGem(request.charid(), request.slotid(), request.gemindex(), request.attrindex(), change))
    {
        return EncodeReply(session, char_gem_overlock_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, char_gem_overlock_succeed_ack, &change);
}
