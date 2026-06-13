#pragma once

#include "ManagerBase.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/public.pb.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace proto {
class ChangeInfo;
}

class AchievementRes;

class AchievementMgr : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;

    void OnCreate() override;
    void OnLoad() override;

    void Trigger(uint32_t condition, uint32_t progress, uint32_t param1 = 0, uint32_t param2 = 0);
    void TriggerOne(uint32_t id, uint32_t progress, uint32_t param1 = 0, uint32_t param2 = 0);
    void HandleClientEvents(const proto::Events& events);
    bool ClaimRewards(const google::protobuf::RepeatedField<uint32_t>& ids, proto::ChangeInfo& out);
    proto::Achievements ToProto() const;

    void PushFirstLoginNotificationsBeforeSignin();
    void PushFirstLoginNotificationsAfterSignin();
    bool HasNewAchievements() const;

private:
    ServerProto::AchievementCompBin* MutableBin();
    const ServerProto::AchievementCompBin& Bin() const;

    ServerProto::AchievementInfoBin* FindAchievement(uint32_t id);
    ServerProto::AchievementInfoBin* UpsertAchievement(uint32_t id, uint32_t maxProgress);
    proto::Achievement BuildAchievementProto(const ServerProto::AchievementInfoBin& achievement) const;
    void SyncAchievement(const ServerProto::AchievementInfoBin& achievement);
    uint32_t GetCompletedAchievementsCount() const;
    bool UpdateAchievement(const AchievementRes& data, uint32_t progress, uint32_t param1, uint32_t param2, bool incremental, bool& completed);
    void TriggerAchievementTotalIfNeeded(bool completed);
    const std::vector<const AchievementRes*>& GetAchievementsByCondition(uint32_t condition) const;
    void BuildConditionIndex() const;
    std::vector<uint32_t> CollectInitialAchievements(uint32_t condition, uint32_t progress, uint32_t param1, uint32_t param2, size_t limit) const;
    void PushFirstLoginAchievementGroup(uint32_t condition, uint32_t progress, uint32_t param1, uint32_t param2, size_t limit);

    bool mUpdatingAchievementTotal = false;
    mutable bool mConditionIndexBuilt = false;
    mutable std::unordered_map<uint32_t, std::vector<const AchievementRes*>> mConditionIndex;
    mutable std::vector<const AchievementRes*> mEmptyConditionList;
};
