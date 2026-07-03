#include "VampireSurvivorRes.h"
#include "../ResourceJsonUtil.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool VampireSurvivorRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "Mode", Mode);
    ReadResourceJsonField(data, "NeedWorldClass", NeedWorldClass);
    ReadResourceJsonField(data, "FateCardBundle", FateCardBundle);
    ReadResourceJsonField(data, "NormalScore1", NormalScore1);
    ReadResourceJsonField(data, "EliteScore1", EliteScore1);
    ReadResourceJsonField(data, "BossScore1", BossScore1);
    ReadResourceJsonField(data, "TimeScore1", TimeScore1);
    ReadResourceJsonField(data, "TimeLimit1", TimeLimit1);
    ReadResourceJsonField(data, "NormalScore2", NormalScore2);
    ReadResourceJsonField(data, "EliteScore2", EliteScore2);
    ReadResourceJsonField(data, "BossScore2", BossScore2);
    ReadResourceJsonField(data, "TimeScore2", TimeScore2);
    ReadResourceJsonField(data, "TimeLimit2", TimeLimit2);
    return true;
}

bool VampireSurvivorRes::LoadFromPb(std::string data)
{
    VampireSurvivor vampireSurvivor;
    if (!vampireSurvivor.ParseFromString(data)) {
        return false;
    }

    Id = vampireSurvivor.id();
    Mode = vampireSurvivor.mode();
    NeedWorldClass = vampireSurvivor.needworldclass();
    FateCardBundle.clear();
    for (int fateCardBundleId : vampireSurvivor.fatecardbundle()) {
        FateCardBundle.push_back(fateCardBundleId);
    }
    NormalScore1 = vampireSurvivor.normalscore1();
    EliteScore1 = vampireSurvivor.elitescore1();
    BossScore1 = vampireSurvivor.bossscore1();
    TimeScore1 = vampireSurvivor.timescore1();
    TimeLimit1 = vampireSurvivor.timelimit1();
    NormalScore2 = vampireSurvivor.normalscore2();
    EliteScore2 = vampireSurvivor.elitescore2();
    BossScore2 = vampireSurvivor.bossscore2();
    TimeScore2 = vampireSurvivor.timescore2();
    TimeLimit2 = vampireSurvivor.timelimit2();

    return true;
}

bool VampireTalentRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "Prev", Prev);
    ReadResourceJsonField(data, "Point", Point);
    return true;
}

bool VampireTalentRes::LoadFromPb(std::string data)
{
    VampireTalent vampireTalent;
    if (!vampireTalent.ParseFromString(data)) {
        return false;
    }

    Id = vampireTalent.id();
    Prev.clear();
    for (int prevId : vampireTalent.prev()) {
        Prev.push_back(prevId);
    }
    Point = vampireTalent.point();

    return true;
}

