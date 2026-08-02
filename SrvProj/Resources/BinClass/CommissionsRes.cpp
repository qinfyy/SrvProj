#include "CommissionsRes.h"
#include "../ResourceJsonUtil.h"
#include "../../proto/table_cpp/client_table.pb.h"

#include <array>

using namespace nova::client;

bool AgentRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "Level", Level);
    ReadResourceJsonField(data, "MemberLimit", MemberLimit);
    ReadResourceJsonField(data, "Tags", Tags);
    ReadResourceJsonField(data, "ExtraTags", ExtraTags);
    ReadResourceJsonField(data, "Time1", Time1);
    ReadResourceJsonField(data, "RewardPreview1", RewardPreview1);
    ReadResourceJsonField(data, "BonusPreview1", BonusPreview1);
    ReadResourceJsonField(data, "Time2", Time2);
    ReadResourceJsonField(data, "RewardPreview2", RewardPreview2);
    ReadResourceJsonField(data, "BonusPreview2", BonusPreview2);
    ReadResourceJsonField(data, "Time3", Time3);
    ReadResourceJsonField(data, "RewardPreview3", RewardPreview3);
    ReadResourceJsonField(data, "BonusPreview3", BonusPreview3);
    ReadResourceJsonField(data, "Time4", Time4);
    ReadResourceJsonField(data, "RewardPreview4", RewardPreview4);
    ReadResourceJsonField(data, "BonusPreview4", BonusPreview4);
    return true;
}

bool AgentRes::LoadFromPb(std::string data)
{
    Agent a;
    if (!a.ParseFromString(data)) {
        return false;
    }
    Id = a.id();
    Level = a.level();
    MemberLimit = a.memberlimit();

    Tags.clear();
    for (const auto& tag : a.tags()) {
        Tags.push_back(tag);
    }

    ExtraTags.clear();
    for (const auto& tag : a.extratags()) {
        ExtraTags.push_back(tag);
    }

    Time1 = a.time1();
    RewardPreview1 = a.rewardpreview1();
    BonusPreview1 = a.bonuspreview1();

    Time2 = a.time2();
    RewardPreview2 = a.rewardpreview2();
    BonusPreview2 = a.bonuspreview2();

    Time3 = a.time3();
    RewardPreview3 = a.rewardpreview3();
    BonusPreview3 = a.bonuspreview3();

    Time4 = a.time4();
    RewardPreview4 = a.rewardpreview4();
    BonusPreview4 = a.bonuspreview4();

    return true;
}



void AgentRes::OnLoad()
{
    DurationRewards.clear();
    DurationBonusRewards.clear();
    TagCounts.clear();
    ExtraTagCounts.clear();

    const std::array<std::pair<int, std::pair<const std::string*, const std::string*>>, 4> durations = {{
        {Time1, {&RewardPreview1, &BonusPreview1}},
        {Time2, {&RewardPreview2, &BonusPreview2}},
        {Time3, {&RewardPreview3, &BonusPreview3}},
        {Time4, {&RewardPreview4, &BonusPreview4}}
    }};

    for (const auto& [time, previews] : durations)
    {
        ItemRewardList rewards;
        ItemRewardList bonusRewards;
        for (const auto& row : ParseIntMatrixJson(*previews.first))
        {
            if (row.size() < 2)
            {
                continue;
            }
            rewards.emplace_back(row[0], row[1], row.size() >= 3 ? row[2] : row[1]);
        }
        for (const auto& row : ParseIntMatrixJson(*previews.second))
        {
            if (row.size() < 2)
            {
                continue;
            }
            bonusRewards.emplace_back(row[0], row[1], row.size() >= 3 ? row[2] : row[1]);
        }
        DurationRewards[time] = std::move(rewards);
        DurationBonusRewards[time] = std::move(bonusRewards);
    }

    for (int tag : Tags)
    {
        ++TagCounts[tag];
    }
    for (int tag : ExtraTags)
    {
        ++ExtraTagCounts[tag];
    }
}
