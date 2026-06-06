#include "ScoreBossRes.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

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
