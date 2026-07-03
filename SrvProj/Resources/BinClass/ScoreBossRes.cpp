#include "ScoreBossRes.h"
#include "../ResourceJsonUtil.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool ScoreBossControlRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "StartTime", StartTime);
    ReadResourceJsonField(data, "EndTime", EndTime);
    ReadResourceJsonField(data, "LevelGroup", LevelGroup);
    return true;
}

bool ScoreBossControlRes::LoadFromPb(std::string data)
{
    ScoreBossControl scoreBossControl;
    if (!scoreBossControl.ParseFromString(data)) {
        return false;
    }

    Id = scoreBossControl.id();
    StartTime = scoreBossControl.starttime();
    EndTime = scoreBossControl.endtime();
    LevelGroup.clear();
    for (int levelId : scoreBossControl.levelgroup()) {
        LevelGroup.insert(levelId);
    }

    return true;
}

bool ScoreBossRewardRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "StarNeed", StarNeed);
    ReadResourceJsonField(data, "RewardItemId1", RewardItemId1);
    ReadResourceJsonField(data, "RewardNum1", RewardNum1);
    return true;
}

bool ScoreBossRewardRes::LoadFromPb(std::string data)
{
    ScoreBossReward scoreBossReward;
    if (!scoreBossReward.ParseFromString(data)) {
        return false;
    }

    StarNeed = scoreBossReward.starneed();
    RewardItemId1 = scoreBossReward.rewarditemid1();
    RewardNum1 = scoreBossReward.rewardnum1();

    return true;
}

