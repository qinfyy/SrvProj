#include "DiscRes.h"
#include "../GameData.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../../proto/table_cpp/client_table.pb.h"

bool DiscRes::LoadFromPb(std::string data) {
    nova::client::Disc disc;
    if (!disc.ParseFromString(data)) {
        return false;
    }

    Id = disc.id();
    Visible = disc.visible();
    Available = disc.available();
    EET = disc.eet();
    StrengthenGroupId = disc.strengthengroupid();
    PromoteGroupId = disc.promotegroupid();
    TransformItemId = disc.transformitemid();
    MaxStarTransformItem.clear();
    for (const auto& item : disc.maxstartransformitem()) {
        MaxStarTransformItem.push_back(item);
    }

    ReadReward.clear();
    for (const auto& item : disc.readreward()) {
        ReadReward.push_back(item);
    }

    SecondarySkillGroupId1 = disc.secondaryskillgroupid1();
    SecondarySkillGroupId2 = disc.secondaryskillgroupid2();
    SubNoteSkillGroupId = disc.subnoteskillgroupid();
    return true;
}

bool DiscStrengthenRes::LoadFromPb(std::string data) {
    nova::client::DiscStrengthen disc;
    if (!disc.ParseFromString(data)) {
        return false;
    }

    Id = disc.id();
    Exp = disc.exp();

    return true;
}

bool DiscItemExpRes::LoadFromPb(std::string data) {
    nova::client::DiscItemExp disc;
    if (!disc.ParseFromString(data)) {
        return false;
    }

    ItemId = disc.itemid();
    Exp = disc.exp();

    return true;
}

bool DiscPromoteRes::LoadFromPb(std::string data) {
    nova::client::DiscPromote disc;
    if (!disc.ParseFromString(data)) {
        return false;
    }

    Id = disc.id();
    ItemId1 = disc.itemid1();
    Num1 = disc.num1();
    ItemId2 = disc.itemid2();
    Num2 = disc.num2();
    ItemId3 = disc.itemid3();
    Num3 = disc.num3();
    ExpenseGold = disc.expensegold();

    return true;
}

bool DiscPromoteLimitRes::LoadFromPb(std::string data) {
    nova::client::DiscPromoteLimit dpl;
    if (!dpl.ParseFromString(data)) {
        return false;
    }

    Id = dpl.id();
    Rarity = dpl.rarity();
    Phase = dpl.phase();
    MaxLevel = dpl.maxlevel();
    WorldClassLimit = dpl.worldclasslimit();
    return true;

}

bool SecondarySkillRes::LoadFromPb(std::string data) {
    nova::client::SecondarySkill ss;
    if (!ss.ParseFromString(data)) {
        return false;
    }

    Id = ss.id();
    GroupId = ss.groupid();
    Level = ss.level();
    Score = ss.score();
    NeedSubNoteSkills = ss.needsubnoteskills();
    return true;

}

void SecondarySkillRes::OnLoad()
{
    NeedSubNotes = ItemParamMap::FromJsonString(NeedSubNoteSkills);
}

bool SecondarySkillRes::Match(const ItemParamMap& subNotes) const
{
    for (const auto& [itemId, count] : NeedSubNotes.Items)
    {
        const auto it = subNotes.Items.find(itemId);
        if (it == subNotes.Items.end() || it->second < count)
        {
            return false;
        }
    }

    return true;
}

int SecondarySkillRes::GetSecondarySkill(const ItemParamMap& subNotes, int groupId)
{
    if (groupId <= 0)
    {
        return 0;
    }

    std::vector<const SecondarySkillRes*> group;
    for (const auto& [_, value] : GameData::SecondarySkillDataTable)
    {
        if (value.GroupId == groupId)
        {
            group.push_back(&value);
        }
    }

    std::sort(group.begin(), group.end(), [](const SecondarySkillRes* left, const SecondarySkillRes* right) {
        return left->Level < right->Level;
    });

    for (auto it = group.rbegin(); it != group.rend(); ++it)
    {
        if ((*it)->Match(subNotes))
        {
            return (*it)->Id;
        }
    }

    return 0;
}

std::vector<int> SecondarySkillRes::CalculateSecondarySkills(const std::vector<uint32_t>& discIds, const ItemParamMap& subNotes)
{
    std::vector<int> result;
    const size_t count = (std::min)(discIds.size(), static_cast<size_t>(3));
    for (size_t i = 0; i < count; ++i)
    {
        const auto it = GameData::DiscDataTable.find(std::to_string(discIds[i]));
        if (it == GameData::DiscDataTable.end())
        {
            continue;
        }

        const int skill1 = SecondarySkillRes::GetSecondarySkill(subNotes, it->second.SecondarySkillGroupId1);
        if (skill1 > 0 && std::find(result.begin(), result.end(), skill1) == result.end())
        {
            result.push_back(skill1);
        }

        const int skill2 = SecondarySkillRes::GetSecondarySkill(subNotes, it->second.SecondarySkillGroupId2);
        if (skill2 > 0 && std::find(result.begin(), result.end(), skill2) == result.end())
        {
            result.push_back(skill2);
        }
    }

    return result;
}

