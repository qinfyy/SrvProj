#pragma once

#include "ManagerBase.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/player_data.pb.h"

#include <cstdint>
#include <string>
#include <vector>

namespace proto {
class ChangeInfo;
}

class QuestMgr : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;

    void OnCreate() override;
    void OnLoad() override;
    void OnLogin() override;
    void EncodePlayerInfo(proto::PlayerInfo& out) const override;

    void Trigger(uint32_t condition, uint32_t progress, uint32_t param1 = 0, uint32_t param2 = 0);

    bool ClaimDailyQuestReward(uint32_t questId, proto::ChangeInfo& out);
    bool ClaimWeeklyQuestReward(uint32_t questId, proto::ChangeInfo& out);
    bool ClaimDailyActiveRewards(std::vector<uint32_t>& activeIds, proto::ChangeInfo& out);
    bool ClaimWeeklyActiveRewards(std::vector<uint32_t>& activeIds, proto::ChangeInfo& out);
    bool ClaimDailyShopGift(proto::ChangeInfo& out);
    bool ClaimDailyMallGift(proto::ChangeInfo& out);
    bool ReceiveWorldClassReward(uint32_t id, proto::ChangeInfo& out);
    std::string GetWorldClassRewardFlag() const;

    bool HasDailyShopReward() const;
    bool HasDailyMallReward() const;

private:
    ServerProto::QuestCompBin* MutableBin();
    const ServerProto::QuestCompBin& Bin() const;

    void InitializeDefaultQuests(bool markFirstLoginDone);
    void ResetDailyQuests(bool resetWeekly);
    void PushFirstLoginNotifications();
    ServerProto::QuestInfoBin* FindQuest(proto::QuestType type, uint32_t id);
    const ServerProto::QuestInfoBin* FindQuest(proto::QuestType type, uint32_t id) const;
    ServerProto::QuestInfoBin* UpsertQuest(proto::QuestType type, uint32_t id, uint32_t maxProgress);
    void SyncQuest(const ServerProto::QuestInfoBin& quest);

    friend class Player;
};
