#include "DiscRes.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../../proto/table_cpp/client_table.pb.h"

void DiscRes::OnLoad() {

}

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

//void DiscItemExpRes::OnLoad() {
//}

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
    Score = ss.score();
    NeedSubNoteSkills = ss.needsubnoteskills();
    return true;

}
