#include "ActivityRes.h"
#include "../ResourceJsonUtil.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool ActivityRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "ActivityType", ActivityType);
    return true;
}

bool ActivityRes::LoadFromPb(std::string data)
{
    Activity activity;
    if (!activity.ParseFromString(data)) {
        return false;
    }

    Id = activity.id();
    ActivityType = activity.activitytype();

    return true;
}

bool LoginRewardGroupControlRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "RewardId1", RewardId1);
    ReadResourceJsonField(data, "Qty1", Qty1);
    ReadResourceJsonField(data, "RewardId2", RewardId2);
    ReadResourceJsonField(data, "Qty2", Qty2);
    return true;
}

bool LoginRewardGroupControlRes::LoadFromPb(std::string data)
{
    LoginRewardGroup loginRewardGroup;
    if (!loginRewardGroup.ParseFromString(data)) {
        return false;
    }

    Id = loginRewardGroup.id();
    RewardId1 = loginRewardGroup.rewardid1();
    Qty1 = loginRewardGroup.qty1();
    RewardId2 = loginRewardGroup.rewardid2();
    Qty2 = loginRewardGroup.qty2();

    return true;
}

bool TowerDefenseLevelRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "Condition2", Condition2);
    ReadResourceJsonField(data, "Condition3", Condition3);
    ReadResourceJsonField(data, "Item1", Item1);
    ReadResourceJsonField(data, "Qty1", Qty1);
    ReadResourceJsonField(data, "Item2", Item2);
    ReadResourceJsonField(data, "Qty2", Qty2);
    return true;
}

bool TowerDefenseLevelRes::LoadFromPb(std::string data)
{
    TowerDefenseLevel towerDefenseLevel;
    if (!towerDefenseLevel.ParseFromString(data)) {
        return false;
    }

    Id = towerDefenseLevel.id();
    Condition2 = towerDefenseLevel.condition2();
    Condition3 = towerDefenseLevel.condition3();
    Item1 = towerDefenseLevel.item1();
    Qty1 = towerDefenseLevel.qty1();
    Item2 = towerDefenseLevel.item2();
    Qty2 = towerDefenseLevel.qty2();

    return true;
}

bool TrialControlRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "GroupIds", GroupIdSet);
    return true;
}

bool TrialControlRes::LoadFromPb(std::string data)
{
    TrialControl trialControl;
    if (!trialControl.ParseFromString(data)) {
        return false;
    }

    Id = trialControl.id();
    GroupIdSet.clear();
    for (int groupId : trialControl.groupids()) {
        GroupIdSet.insert(groupId);
    }

    return true;
}

bool TrialGroupRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "RewardId1", RewardId1);
    ReadResourceJsonField(data, "Qty1", Qty1);
    ReadResourceJsonField(data, "RewardId2", RewardId2);
    ReadResourceJsonField(data, "Qty2", Qty2);
    ReadResourceJsonField(data, "RewardId3", RewardId3);
    ReadResourceJsonField(data, "Qty3", Qty3);
    return true;
}

bool TrialGroupRes::LoadFromPb(std::string data)
{
    TrialGroup trialGroup;
    if (!trialGroup.ParseFromString(data)) {
        return false;
    }

    Id = trialGroup.id();
    RewardId1 = trialGroup.rewardid1();
    Qty1 = trialGroup.qty1();
    RewardId2 = trialGroup.rewardid2();
    Qty2 = trialGroup.qty2();
    RewardId3 = trialGroup.rewardid3();
    Qty3 = trialGroup.qty3();

    return true;
}

bool JointDrill2LevelRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "BattleTime", BattleTime);
    ReadResourceJsonField(data, "TimeScore", TimeScore);
    ReadResourceJsonField(data, "ScorePerSec", ScorePerSec);
    ReadResourceJsonField(data, "LevelScore", LevelScore);
    ReadResourceJsonField(data, "BaseHpScore", BaseHpScore);
    ReadResourceJsonField(data, "RewardPreview", RewardPreview);
    return true;
}

bool JointDrill2LevelRes::LoadFromPb(std::string data)
{
    JointDrill_2_Level jointDrillLevel;
    if (!jointDrillLevel.ParseFromString(data)) {
        return false;
    }

    Id = jointDrillLevel.id();
    BattleTime = jointDrillLevel.battletime();
    TimeScore = jointDrillLevel.timescore();
    ScorePerSec = jointDrillLevel.scorepersec();
    LevelScore = jointDrillLevel.levelscore();
    BaseHpScore = jointDrillLevel.basehpscore();
    RewardPreview = jointDrillLevel.rewardpreview();

    return true;
}

bool ActivityLevelsLevelRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "ActivityId", ActivityId);
    ReadResourceJsonField(data, "EnergyConsume", EnergyConsume);
    ReadResourceJsonField(data, "CompleteRewardPreview", CompleteRewardPreview);
    return true;
}

bool ActivityLevelsLevelRes::LoadFromPb(std::string data)
{
    ActivityLevelsLevel activityLevelsLevel;
    if (!activityLevelsLevel.ParseFromString(data)) {
        return false;
    }

    Id = activityLevelsLevel.id();
    ActivityId = activityLevelsLevel.activityid();
    EnergyConsume = activityLevelsLevel.energyconsume();
    CompleteRewardPreview = activityLevelsLevel.completerewardpreview();

    return true;
}

bool ActivityTaskRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "ActivityTaskGroupId", ActivityTaskGroupId);
    ReadResourceJsonField(data, "CompleteCond", CompleteCond);
    ReadResourceJsonField(data, "AimNumShow", AimNumShow);
    ReadResourceJsonField(data, "Tid1", Tid1);
    ReadResourceJsonField(data, "Qty1", Qty1);
    ReadResourceJsonField(data, "Tid2", Tid2);
    ReadResourceJsonField(data, "Qty2", Qty2);
    return true;
}

bool ActivityTaskRes::LoadFromPb(std::string data)
{
    ActivityTask activityTask;
    if (!activityTask.ParseFromString(data)) {
        return false;
    }

    Id = activityTask.id();
    ActivityTaskGroupId = activityTask.activitytaskgroupid();
    CompleteCond = activityTask.completecond();
    AimNumShow = activityTask.aimnumshow();
    Tid1 = activityTask.tid1();
    Qty1 = activityTask.qty1();
    Tid2 = activityTask.tid2();
    Qty2 = activityTask.qty2();

    return true;
}

bool ActivityTaskGroupRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "ActivityId", ActivityId);
    ReadResourceJsonField(data, "Reward1", Reward1);
    ReadResourceJsonField(data, "RewardQty1", RewardQty1);
    ReadResourceJsonField(data, "Reward2", Reward2);
    ReadResourceJsonField(data, "RewardQty2", RewardQty2);
    ReadResourceJsonField(data, "Reward3", Reward3);
    ReadResourceJsonField(data, "RewardQty3", RewardQty3);
    ReadResourceJsonField(data, "Reward4", Reward4);
    ReadResourceJsonField(data, "RewardQty4", RewardQty4);
    ReadResourceJsonField(data, "Reward5", Reward5);
    ReadResourceJsonField(data, "RewardQty5", RewardQty5);
    ReadResourceJsonField(data, "Reward6", Reward6);
    ReadResourceJsonField(data, "RewardQty6", RewardQty6);
    return true;
}

bool ActivityTaskGroupRes::LoadFromPb(std::string data)
{
    ActivityTaskGroup activityTaskGroup;
    if (!activityTaskGroup.ParseFromString(data)) {
        return false;
    }

    Id = activityTaskGroup.id();
    ActivityId = activityTaskGroup.activityid();
    Reward1 = activityTaskGroup.reward1();
    RewardQty1 = activityTaskGroup.rewardqty1();
    Reward2 = activityTaskGroup.reward2();
    RewardQty2 = activityTaskGroup.rewardqty2();
    Reward3 = activityTaskGroup.reward3();
    RewardQty3 = activityTaskGroup.rewardqty3();
    Reward4 = activityTaskGroup.reward4();
    RewardQty4 = activityTaskGroup.rewardqty4();
    Reward5 = activityTaskGroup.reward5();
    RewardQty5 = activityTaskGroup.rewardqty5();
    Reward6 = activityTaskGroup.reward6();
    RewardQty6 = activityTaskGroup.rewardqty6();

    return true;
}

bool ActivityShopRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "CurrencyItemId", CurrencyItemId);
    ReadResourceJsonField(data, "ExchangeItemId", ExchangeItemId);
    ReadResourceJsonField(data, "Rate", Rate);
    return true;
}

bool ActivityShopRes::LoadFromPb(std::string data)
{
    ActivityShop activityShop;
    if (!activityShop.ParseFromString(data)) {
        return false;
    }

    Id = activityShop.id();
    CurrencyItemId = activityShop.currencyitemid();
    ExchangeItemId = activityShop.exchangeitemid();
    Rate = activityShop.rate();

    return true;
}

bool ActivityShopControlRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "ShopIds", ShopIds);
    return true;
}

bool ActivityShopControlRes::LoadFromPb(std::string data)
{
    ActivityShopControl activityShopControl;
    if (!activityShopControl.ParseFromString(data)) {
        return false;
    }

    Id = activityShopControl.id();
    ShopIds.clear();
    for (int shopId : activityShopControl.shopids()) {
        ShopIds.push_back(shopId);
    }

    return true;
}

bool ActivityGoodsRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "ShopId", ShopId);
    ReadResourceJsonField(data, "ItemId", ItemId);
    ReadResourceJsonField(data, "ItemQuantity", ItemQuantity);
    ReadResourceJsonField(data, "MaximumLimit", MaximumLimit);
    ReadResourceJsonField(data, "Price", Price);
    return true;
}

bool ActivityGoodsRes::LoadFromPb(std::string data)
{
    ActivityGoods activityGoods;
    if (!activityGoods.ParseFromString(data)) {
        return false;
    }

    Id = activityGoods.id();
    ShopId = activityGoods.shopid();
    ItemId = activityGoods.itemid();
    ItemQuantity = activityGoods.itemquantity();
    MaximumLimit = activityGoods.maximumlimit();
    Price = activityGoods.price();

    return true;
}

