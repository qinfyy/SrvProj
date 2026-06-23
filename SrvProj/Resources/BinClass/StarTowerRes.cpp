#include "StarTowerRes.h"
#include "../../proto/table_cpp/client_table.pb.h"

#include "../GameData.h"

#include <algorithm>
#include <random>

using namespace nova::client;

std::unordered_map<int, std::vector<int>> SubNoteSkillDropGroupRes::Groups;

void StarTowerRes::OnLoad()
{
    MaxFloors = 0;
    for (int floor : FloorNum)
    {
        MaxFloors += (std::max)(floor, 0);
    }
}

int StarTowerRes::GetMaxFloor(int stageNum) const
{
    const int index = stageNum - 1;
    if (index < 0 || index >= static_cast<int>(FloorNum.size()))
    {
        return 0;
    }

    return FloorNum[static_cast<size_t>(index)];
}

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
    GroupId = sts.groupid();
    Floor = sts.floor();
    InteriorCurrencyQuantity = sts.interiorcurrencyquantity();
    RoomType = sts.roomtype();
    GuaranteedMapId = sts.guaranteedmapid();
    GuaranteedMonsterPlanId = sts.guaranteedmonsterplanid();

    return true;
}

void StarTowerStageRes::OnLoad()
{
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
    OptionsRulesId = ste.optionsrulesid();
    EventType = ste.eventtype();
    GuaranteedMapId = ste.guaranteedmapid();
    EventResType = ste.eventrestype();

    RelatedNPCs.clear();
    for (const auto& fn : ste.relatednpcs()) {
        RelatedNPCs.push_back(fn);
    }

    return true;
}

void StarTowerEventRes::OnLoad()
{
    OptionIds.clear();
}

bool EventOptionsRes::LoadFromPb(std::string data)
{
    EventOptions value;
    if (!value.ParseFromString(data)) {
        return false;
    }

    Id = value.id();
    Desc = value.desc();
    IgnoreInterActive = value.ignoreinteractive();
    return true;
}

void EventOptionsRes::OnLoad()
{
    const int eventId = Id / 100;
    const auto eventIt = GameData::StarTowerEventDataTable.find(std::to_string(eventId));
    if (eventIt == GameData::StarTowerEventDataTable.end())
    {
        return;
    }

    eventIt->second.OptionIds.push_back(Id);
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

void SubNoteSkillPromoteGroupRes::OnLoad()
{
    Items = ItemParamMap::FromJsonString(SubNoteSkills);
}

bool SubNoteSkillDropGroupRes::LoadFromPb(std::string data)
{
    SubNoteSkillDropGroup value;
    if (!value.ParseFromString(data)) {
        return false;
    }

    Id = value.id();
    GroupId = value.groupid();
    SubNoteSkillId = value.subnoteskillid();
    return true;
}

void SubNoteSkillDropGroupRes::OnLoad()
{
    Groups[GroupId].push_back(SubNoteSkillId);
}

int SubNoteSkillDropGroupRes::GetRandomDrop(int groupId)
{
    const auto it = Groups.find(groupId);
    if (it == Groups.end() || it->second.empty())
    {
        return 0;
    }

    static thread_local std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<size_t> dist(0, it->second.size() - 1);
    return it->second[dist(rng)];
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
    BriefDesc = p.briefdesc();
    BuildScore.clear();
    for (const auto& bs : p.buildscore()) {
        BuildScore.push_back(bs);
    }

    return true;
}

bool PotentialRes::IsSpecial() const
{
    return BranchType != 3;
}

int PotentialRes::GetMaxLevel() const
{
    if (BranchType == 3)
    {
        return 6;
    }

    return MaxLevel;
}

int PotentialRes::GetMaxLevel(int extraLevels) const
{
    if (BranchType == 3)
    {
        return MaxLevel + (std::max)(extraLevels, 0);
    }

    return MaxLevel;
}

int PotentialRes::GetBuildScore(int level) const
{
    if (BuildScore.empty() || level <= 0)
    {
        return 0;
    }

    int index = level - 1;
    if (index >= static_cast<int>(BuildScore.size()))
    {
        index = static_cast<int>(BuildScore.size()) - 1;
    }

    return BuildScore[static_cast<size_t>(index)];
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

std::vector<int> CharPotentialRes::GetPotentialList(bool main, bool special) const
{
    std::vector<int> out;

    auto append = [&out](const std::vector<int>& values) {
        out.insert(out.end(), values.begin(), values.end());
    };

    if (main)
    {
        append(special ? MasterSpecificPotentialIds : MasterNormalPotentialIds);
    }
    else
    {
        append(special ? AssistSpecificPotentialIds : AssistNormalPotentialIds);
    }

    if (!special)
    {
        append(CommonPotentialIds);
    }

    return out;
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

void StarTowerBookFateCardRes::OnLoad()
{
    const auto fateCardIt = GameData::FateCardDataTable.find(std::to_string(Id));
    if (fateCardIt == GameData::FateCardDataTable.end())
    {
        return;
    }

    fateCardIt->second.BundleId = BundleId;

    const auto bundleIt = GameData::StarTowerBookFateCardBundleDataTable.find(std::to_string(BundleId));
    if (bundleIt == GameData::StarTowerBookFateCardBundleDataTable.end())
    {
        return;
    }

    auto& cardIds = bundleIt->second.CardIds;
    if (std::find(cardIds.begin(), cardIds.end(), Id) == cardIds.end())
    {
        cardIds.push_back(Id);
    }
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

bool NPCAffinityGroupRes::LoadFromPb(std::string data)
{
    NPCAffinityGroup value;
    if (!value.ParseFromString(data)) {
        return false;
    }

    Id = value.id();
    Level = value.level();
    AffinityValue = value.affinityvalue();
    AffinityGroupId = value.affinitygroupid();
    RelationshipName = value.relationshipname();
    Icon = value.icon();
    AffinityLevelStage = value.affinitylevelstage();
    Reward = value.reward();
    return true;
}

bool NPCAffinityPlotRes::LoadFromPb(std::string data)
{
    NPCAffinityPlot value;
    if (!value.ParseFromString(data)) {
        return false;
    }

    Id = value.id();
    Name = value.name();
    Desc = value.desc();
    PlotSum = value.plotsum();
    AvgId = value.avgid();
    NPCId = value.npcid();
    AffinityLevel = value.affinitylevel();
    ItemId = value.itemid();
    ItemQty = value.itemqty();
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

