#include "VampireSurvivorRes.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

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

