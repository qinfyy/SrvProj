#include "StarTowerRes.h"
#include "../ResourceJsonUtil.h"
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

bool StarTowerGrowthNodeRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "NodeId", NodeId);
    ReadResourceJsonField(data, "Group", Group);
    ReadResourceJsonField(data, "ItemId1", ItemId1);
    ReadResourceJsonField(data, "ItemQty1", ItemQty1);
    return true;
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

bool StarTowerRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "GroupId", GroupId);
    ReadResourceJsonField(data, "Difficulty", Difficulty);
    ReadResourceJsonField(data, "SubNoteSkillDropGroupId", SubNoteSkillDropGroupId);
    ReadResourceJsonField(data, "FloorNum", FloorNum);
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

bool StarTowerStageRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "Stage", Stage);
    ReadResourceJsonField(data, "GroupId", GroupId);
    ReadResourceJsonField(data, "Floor", Floor);
    ReadResourceJsonField(data, "InteriorCurrencyQuantity", InteriorCurrencyQuantity);
    ReadResourceJsonField(data, "RoomType", RoomType);
    ReadResourceJsonField(data, "GuaranteedMapId", GuaranteedMapId);
    ReadResourceJsonField(data, "GuaranteedMonsterPlanId", GuaranteedMonsterPlanId);
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

bool StarTowerFloorExpRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "StarTowerId", StarTowerId);
    ReadResourceJsonField(data, "Stage", Stage);
    ReadResourceJsonField(data, "NormalExp", NormalExp);
    ReadResourceJsonField(data, "EliteExp", EliteExp);
    ReadResourceJsonField(data, "BossExp", BossExp);
    ReadResourceJsonField(data, "FinalBossExp", FinalBossExp);
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

bool StarTowerTeamExpRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "GroupId", GroupId);
    ReadResourceJsonField(data, "Level", Level);
    ReadResourceJsonField(data, "NeedExp", NeedExp);
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

bool StarTowerEventRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "OptionsRulesId", OptionsRulesId);
    ReadResourceJsonField(data, "EventType", EventType);
    ReadResourceJsonField(data, "GuaranteedMapId", GuaranteedMapId);
    ReadResourceJsonField(data, "RelatedNPCs", RelatedNPCs);
    ReadResourceJsonField(data, "EventResType", EventResType);
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

bool EventOptionsRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "Desc", Desc);
    ReadResourceJsonField(data, "IgnoreInterActive", IgnoreInterActive);
    return true;
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
    const auto eventIt = GameData::StarTowerEventDataTable.find(eventId);
    if (eventIt == GameData::StarTowerEventDataTable.end())
    {
        return;
    }

    eventIt->second.OptionIds.push_back(Id);
}

bool StarTowerBuildRankRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "MinGrade", MinGrade);
    ReadResourceJsonField(data, "Rarity", Rarity);
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

bool SubNoteSkillPromoteGroupRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "SubNoteSkills", SubNoteSkills);
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

bool SubNoteSkillDropGroupRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "GroupId", GroupId);
    ReadResourceJsonField(data, "SubNoteSkillId", SubNoteSkillId);
    return true;
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

bool PotentialRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "CharId", CharId);
    ReadResourceJsonField(data, "Build", Build);
    ReadResourceJsonField(data, "BranchType", BranchType);
    ReadResourceJsonField(data, "MaxLevel", MaxLevel);
    ReadResourceJsonField(data, "BuildScore", BuildScore);
    ReadResourceJsonField(data, "BriefDesc", BriefDesc);
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

bool CharPotentialRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "MasterSpecificPotentialIds", MasterSpecificPotentialIds);
    ReadResourceJsonField(data, "AssistSpecificPotentialIds", AssistSpecificPotentialIds);
    ReadResourceJsonField(data, "CommonPotentialIds", CommonPotentialIds);
    ReadResourceJsonField(data, "MasterNormalPotentialIds", MasterNormalPotentialIds);
    ReadResourceJsonField(data, "AssistNormalPotentialIds", AssistNormalPotentialIds);
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

bool StarTowerBookFateCardBundleRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
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

bool StarTowerBookFateCardQuestRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
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

bool StarTowerBookFateCardRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "BundleId", BundleId);
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
    const auto fateCardIt = GameData::FateCardDataTable.find(Id);
    if (fateCardIt == GameData::FateCardDataTable.end())
    {
        return;
    }

    fateCardIt->second.BundleId = BundleId;

    const auto bundleIt = GameData::StarTowerBookFateCardBundleDataTable.find(BundleId);
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

bool FateCardRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "IsTower", IsTower);
    ReadResourceJsonField(data, "IsVampire", IsVampire);
    ReadResourceJsonField(data, "IsVampireSpecial", IsVampireSpecial);
    ReadResourceJsonField(data, "Removable", Removable);
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

bool NPCAffinityGroupRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "Level", Level);
    ReadResourceJsonField(data, "AffinityValue", AffinityValue);
    ReadResourceJsonField(data, "AffinityGroupId", AffinityGroupId);
    ReadResourceJsonField(data, "RelationshipName", RelationshipName);
    ReadResourceJsonField(data, "Icon", Icon);
    ReadResourceJsonField(data, "AffinityLevelStage", AffinityLevelStage);
    ReadResourceJsonField(data, "Reward", Reward);
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

bool NPCAffinityPlotRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "Name", Name);
    ReadResourceJsonField(data, "Desc", Desc);
    ReadResourceJsonField(data, "PlotSum", PlotSum);
    ReadResourceJsonField(data, "AvgId", AvgId);
    ReadResourceJsonField(data, "NPCId", NPCId);
    ReadResourceJsonField(data, "AffinityLevel", AffinityLevel);
    ReadResourceJsonField(data, "ItemId", ItemId);
    ReadResourceJsonField(data, "ItemQty", ItemQty);
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

bool InfinityTowerLevelRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "DifficultyId", DifficultyId);
    ReadResourceJsonField(data, "BaseAwardPreview", BaseAwardPreview);
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

bool InfinityTowerDifficultyRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "TowerId", TowerId);
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
