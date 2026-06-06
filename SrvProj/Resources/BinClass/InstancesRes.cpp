#include "InstancesRes.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool DailyInstanceRes::LoadFromPb(std::string data)
{
    DailyInstance di;
    if (!di.ParseFromString(data)) {
        return false;
    }

	Id = di.id();
	AwardDropId = di.awarddropid();
    PreLevelId = di.prelevelid();
    PreLevelStar = di.prelevelstar();
    OneStarEnergyConsume = di.onestarenergyconsume();
    NeedWorldClass = di.needworldclass();

	return true;
}

bool DailyInstanceRewardGroupRes::LoadFromPb(std::string data)
{
    DailyInstanceRewardGroup dailyInstanceRewardGroup;
    if (!dailyInstanceRewardGroup.ParseFromString(data)) {
        return false;
    }

    GroupId = dailyInstanceRewardGroup.groupid();
    DailyRewardType = dailyInstanceRewardGroup.dailyrewardtype();
    BaseAwardPreview = dailyInstanceRewardGroup.baseawardpreview();

    return true;
}

bool RegionBossLevelRes::LoadFromPb(std::string data)
{
    RegionBossLevel regionBossLevel;
    if (!regionBossLevel.ParseFromString(data)) {
        return false;
    }

    Id = regionBossLevel.id();
    PreLevelId = regionBossLevel.prelevelid();
    PreLevelStar = regionBossLevel.prelevelstar();
    NeedWorldClass = regionBossLevel.needworldclass();
    EnergyConsume = regionBossLevel.energyconsume();
    BaseAwardPreview = regionBossLevel.baseawardpreview();

    return true;
}

bool SkillInstanceRes::LoadFromPb(std::string data)
{
    SkillInstance skillInstance;
    if (!skillInstance.ParseFromString(data)) {
        return false;
    }

    Id = skillInstance.id();
    PreLevelId = skillInstance.prelevelid();
    PreLevelStar = skillInstance.prelevelstar();
    NeedWorldClass = skillInstance.needworldclass();
    EnergyConsume = skillInstance.energyconsume();
    BaseAwardPreview = skillInstance.baseawardpreview();

    return true;
}

bool CharGemInstanceRes::LoadFromPb(std::string data)
{
    CharGemInstance charGemInstance;
    if (!charGemInstance.ParseFromString(data)) {
        return false;
    }

    Id = charGemInstance.id();
    PreLevelId = charGemInstance.prelevelid();
    PreLevelStar = charGemInstance.prelevelstar();
    NeedWorldClass = charGemInstance.needworldclass();
    EnergyConsume = charGemInstance.energyconsume();
    BaseAwardPreview = charGemInstance.baseawardpreview();

    return true;
}

bool WeekBossLevelRes::LoadFromPb(std::string data)
{
    WeekBossLevel weekBossLevel;
    if (!weekBossLevel.ParseFromString(data)) {
        return false;
    }

    Id = weekBossLevel.id();
    Difficulty = weekBossLevel.difficulty();
    PreLevelId = weekBossLevel.prelevelid();
    NeedWorldClass = weekBossLevel.needworldclass();
    BaseAwardPreview = weekBossLevel.baseawardpreview();

    return true;
}

void DailyInstanceRes::OnLoad()
{
}

void DailyInstanceRewardGroupRes::OnLoad()
{
}

void RegionBossLevelRes::OnLoad()
{
}

void SkillInstanceRes::OnLoad()
{
}

void CharGemInstanceRes::OnLoad()
{
}

void WeekBossLevelRes::OnLoad()
{
}
