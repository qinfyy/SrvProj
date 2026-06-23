#pragma once

#include "ManagerBase.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/battle_pass_info.pb.h"
#include "../proto/proto_cpp/battle_pass_level_buy.pb.h"
#include "../proto/proto_cpp/battle_pass_order_collect.pb.h"
#include "../proto/proto_cpp/battle_pass_reward_receive.pb.h"
#include "../proto/proto_cpp/mall_gem_order.pb.h"
#include "../proto/proto_cpp/player_data.pb.h"
#include "../proto/proto_cpp/public.pb.h"

#include <cstdint>
#include <string>

class BattlePassRes;
class BattlePassRewardRes;

class BattlePassMgr : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;

    void OnCreate() override;
    void OnLoad() override;
    void OnLogin() override;
    void EncodePlayerInfo(proto::PlayerInfo& out) const override;

    void ResetDailyQuests(bool resetWeekly);
    void Trigger(uint32_t condition, uint32_t progress, uint32_t param1 = 0, uint32_t param2 = 0);

    bool ClaimQuestReward(uint32_t questId, uint32_t& level, uint32_t& exp, uint32_t& expThisWeek);
    bool ClaimReward(const proto::BattlePassRewardReceiveReq& req, proto::BattlePassRewardReceiveResp& rsp);
    bool BuyLevels(uint32_t levels, proto::BattlePassLevelBuyResp& rsp);
    bool CreateOrder(uint32_t mode, proto::OrderInfo& rsp);
    bool CollectOrder(proto::BattlePassOrderCollectResp& rsp);

    int32_t GetClientState() const;
    proto::BattlePassInfo ToProto() const;

private:
    ServerProto::BattlePassCompBin* MutableBin();
    const ServerProto::BattlePassCompBin& Bin() const;

    void InitializeDefault();
    uint32_t GetActiveBattlePassId() const;
    const BattlePassRes* GetCurrentSeason() const;
    const BattlePassRewardRes* GetRewardData(uint32_t level) const;
    int64_t GetDeadline() const;
    bool IsUnlocked() const;

    ServerProto::QuestInfoBin* FindQuest(proto::QuestType type, uint32_t id);
    const ServerProto::QuestInfoBin* FindQuest(proto::QuestType type, uint32_t id) const;
    ServerProto::QuestInfoBin* UpsertQuest(const class BattlePassQuestRes& data);
    void SyncQuest(const ServerProto::QuestInfoBin& quest) const;

    uint32_t GetMaxExpForNextLevel() const;
    void AddExp(uint32_t amount);
    bool HasClaimableQuest() const;
    bool HasClaimableReward() const;
    bool ReceiveSingleReward(bool premium, uint32_t level, proto::ChangeInfo& change);
    bool ReceiveAllRewards(proto::ChangeInfo& change);
    bool AddRewardItems(uint32_t level, bool premium, proto::ChangeInfo& change);
    void PushStateNotify() const;
};
