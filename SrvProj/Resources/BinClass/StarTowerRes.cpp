#include "StarTowerRes.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool StarTowerGrowthNodeRes::LoadFromPb(std::string data)
{
    StarTowerGrowthNode s;
    if (!s.ParseFromString(data)) {
        return false;
    }

    Id = s.id();
    NodeId = s.nodeid();
    Group = s.group();
    ItemId1 = s.itemid1();
    ItemQty1 = s.itemqty1();

    return true;
}

bool StarTowerRes::LoadFromPb(std::string data)
{
    StarTower st;
    if (!st.ParseFromString(data)) {
        return false;
    }
    
    Id = st.id();
    GroupId = st.groupid();
    Difficulty = st.difficulty();
    SubNoteSkillDropGroupId = st.subnoteskilldropgroupid();

    FloorNum.clear();
    for (const auto& fn : st.floornum()) {
        FloorNum.push_back(fn);
    }

    return true;
}

bool StarTowerStageRes::LoadFromPb(std::string data)
{
    StarTowerStage sts;
    if (!sts.ParseFromString(data)) {
        return false;
    }

    Id = sts.id();
    Stage = sts.stage();
    Floor = sts.floor();
    InteriorCurrencyQuantity = sts.interiorcurrencyquantity();
    RoomType = sts.roomtype();

    return true;
}

bool StarTowerFloorExpRes::LoadFromPb(std::string data)
{
    StarTowerFloorExp stfe;
    if (!stfe.ParseFromString(data)) {
        return false;
    }

    Id = stfe.id();
    StarTowerId = stfe.startowerid();
    Stage = stfe.stage();

    NormalExp = stfe.normalexp();
    EliteExp = stfe.eliteexp();
    BossExp = stfe.bossexp();
    FinalBossExp = stfe.finalbossexp();
    return true;
}

bool StarTowerTeamExpRes::LoadFromPb(std::string data)
{
    StarTowerTeamExp stte;
    if (!stte.ParseFromString(data)) {
        return false;
    }

    Id = stte.id();
    GroupId = stte.groupid();
    Level = stte.level();
    NeedExp = stte.needexp();

    return true;
}

bool StarTowerEventRes::LoadFromPb(std::string data)
{
    StarTowerEvent ste;
    if (!ste.ParseFromString(data)) {
        return false;
    }

    Id = ste.id();

    RelatedNPCs.clear();
    for (const auto& fn : ste.relatednpcs()) {
        RelatedNPCs.push_back(fn);
    }

    return true;
}

bool StarTowerBuildRankRes::LoadFromPb(std::string data)
{
    StarTowerBuildRank stbr;
    if (!stbr.ParseFromString(data)) {
        return false;
    }
    Id = stbr.id();
    MinGrade = stbr.mingrade();
    Rarity = stbr.rarity();

    return true;
}

bool SubNoteSkillPromoteGroupRes::LoadFromPb(std::string data)
{
    SubNoteSkillPromoteGroup snspg;
    if (!snspg.ParseFromString(data)) {
        return false;
    }

    Id = snspg.id();
    SubNoteSkills = snspg.subnoteskills();

    return true;
}

bool PotentialRes::LoadFromPb(std::string data)
{
    Potential p;
    if (!p.ParseFromString(data)) {
        return false;
    }

    Id = p.id();
    CharId = p.charid();
    Build = p.build();
    BranchType = p.branchtype();
    MaxLevel = p.maxlevel();
    BuildScore.clear();
    for (const auto& bs : p.buildscore()) {
        BuildScore.push_back(bs);
    }

    return true;
}

bool CharPotentialRes::LoadFromPb(std::string data)
{
    CharPotential cp;
    if (!cp.ParseFromString(data)) {
        return false;
    }
    Id = cp.id();

    MasterSpecificPotentialIds.clear();
    for (const auto& mspid : cp.masterspecificpotentialids()) {
        MasterSpecificPotentialIds.push_back(mspid);
    }

    AssistSpecificPotentialIds.clear();
    for (const auto& aspid : cp.assistspecificpotentialids()) {
        AssistSpecificPotentialIds.push_back(aspid);
    }

    CommonPotentialIds.clear();
    for (const auto& cpid : cp.commonpotentialids()) {
        CommonPotentialIds.push_back(cpid);
    }

    MasterNormalPotentialIds.clear();
    for (const auto& mnspid : cp.masternormalpotentialids()) {
        MasterNormalPotentialIds.push_back(mnspid);
    }

    AssistNormalPotentialIds.clear();
    for (const auto& anspid : cp.assistnormalpotentialids()) {
        AssistNormalPotentialIds.push_back(anspid);
    }

    return true;
}

bool StarTowerBookFateCardBundleRes::LoadFromPb(std::string data)
{
    StarTowerBookFateCardBundle stbfcb;
    if (!stbfcb.ParseFromString(data)) {
        return false;
    }

    Id = stbfcb.id();
    //BundleId = stbfcb.();

    return true;
}

bool StarTowerBookFateCardQuestRes::LoadFromPb(std::string data)
{
    StarTowerBookFateCardQuest stbfcq;
    if (!stbfcq.ParseFromString(data)) {
        return false;
    }

    Id = stbfcq.id();

    return true;
}

bool StarTowerBookFateCardRes::LoadFromPb(std::string data)
{
    StarTowerBookFateCard stbfc;
    if (!stbfc.ParseFromString(data)) {
        return false;
    }

    Id = stbfc.id();
    BundleId = stbfc.bundleid();

    return true;
}

bool FateCardRes::LoadFromPb(std::string data)
{
    FateCard fc;
    if (!fc.ParseFromString(data)) {
        return false;
    }

    Id = fc.id();
    IsTower = fc.istower();
    IsVampire = fc.isvampire();
    IsVampireSpecial = fc.isvampirespecial();
    Removable = fc.removable();

    return true;
}

bool InfinityTowerLevelRes::LoadFromPb(std::string data)
{
    InfinityTowerLevel itl;
    if (!itl.ParseFromString(data)) {
        return false;
    }

    Id = itl.id();
    DifficultyId = itl.difficultyid();
    BaseAwardPreview = itl.baseawardpreview();

    return true;
}

bool InfinityTowerDifficultyRes::LoadFromPb(std::string data)
{
    InfinityTowerDifficulty itd;
    if (!itd.ParseFromString(data)) {
        return false;
    }

    Id = itd.id();
    TowerId = itd.towerid();

    return true;
}
