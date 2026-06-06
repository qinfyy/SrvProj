#include "CharacterRes.h"
#include "../GameData.h"
#include "ItemsRes.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

int AffinityLevelRes::MaxLevel = 0;

bool CharacterRes::LoadFromPb(std::string data) {
    Character character;
    if (!character.ParseFromString(data)) {
        return false;
    }

    AIId = character.aiid();
    AdvanceGroup = character.advancegroup();
    AdvanceSkinId = character.advanceskinid();
    AdvanceSkinUnlockLevel = character.advanceskinunlocklevel();
    Ammo = character.ammo();
    AssistAIId = character.assistaiid();
    AssistDodgeId = character.assistdodgeid();
    AssistNormalAtkId = character.assistnormalatkid();
    AssistSkillAngle = character.assistskillangle();
    AssistSkillId = character.assistskillid();
    AssistSkillOnStageType = character.assistskillonstagetype();
    AssistSkillRadius = character.assistskillradius();
    AssistSpecialSkillId = character.assistspecialskillid();
    AssistUltimateAngle = character.assistultimateangle();
    AssistUltimateId = character.assistultimateid();
    AssistUltimateOnStageOrientation = character.assistultimateonstageorientation();
    AssistUltimateOnStageType = character.assistultimateonstagetype();
    AssistUltimateRadius = character.assistultimateradius();
    AtkSpd = character.atkspd();
    AttributeId = character.attributeid();
    Available = character.available();
    BulletType = character.bullettype();
    CharacterAttackType = character.characterattacktype();
    ChargingRate = character.chargingrate();
    Class1 = character.class_();
    DefaultSkinId = character.defaultskinid();
    DodgeId = character.dodgeid();
    DodgeToRunAccelerationOrNot = character.dodgetorunaccelerationornot();
    EET = character.eet();
    EnergyConsume = character.energyconsume();
    EnergyConvRatio = character.energyconvratio();
    EnergyEfficiency = character.energyefficiency();
    Faction = character.faction();
    FragmentsId = character.fragmentsid();
    FrozenTimeHighlightUnit = character.frozentimehighlightunit();
    GemSlots.assign(character.gemslots().begin(), character.gemslots().end());
    Grade = character.grade();
    HearAttackRng = character.hearattackrng();
    HearRng = character.hearrng();
    Id = character.id();
    MovAcc = character.movacc();
    MovType = character.movtype();
    Name = character.name();
    NormalAtkId = character.normalatkid();
    PresentsTraitId = character.presentstraitid();
    RaiseGunRng = character.raisegunrng();
    RecruitmentQty = character.recruitmentqty();
    RotAcc = character.rotacc();
    RotSpd = character.rotspd();
    RunSpd = character.runspd();
    SearchTargetType = character.searchtargettype();
    SkillId = character.skillid();
    SkillSemiAutoRng = character.skillsemiautorng();
    SkillsUpgradeGroup.assign(character.skillsupgradegroup().begin(), character.skillsupgradegroup().end());
    SpRunSpd = character.sprunspd();
    SpecialSkillId = character.specialskillid();
    SwitchCD = character.switchcd();
    TalentSkillId = character.talentskillid();
    TransSpd = character.transspd();
    TransformQty = character.transformqty();
    UltimateId = character.ultimateid();
    UltimateSemiAutoRng = character.ultimatesemiautorng();
    ViewId = character.viewid();
    Visible = character.visible();
    VisionAttackRng = character.visionattackrng();
    VisionDeg = character.visiondeg();
    VisionRng = character.visionrng();
    WalkSpd = character.walkspd();
    WalkToRunDuration = character.walktorunduration();
    Weight = character.weight();

    return true;
}

void CharacterRes::OnLoad() {
    ElementType = EET;
    Chats.clear();
}

bool CharacterDesRes::LoadFromPb(std::string data) {
    CharacterDes des;
    if (!des.ParseFromString(data)) {
        return false;
    }

    Id = des.id();
    Alias = des.alias();
    CnCv = des.cncv();
    JpCv = des.jpcv();
    CharColor = des.charcolor();
    CharSkillColor = des.charskillcolor();
    CharDes = des.chardes();
    Tag.assign(des.tag().begin(), des.tag().end());
    Force = des.force();
    PreferTags.assign(des.prefertags().begin(), des.prefertags().end());
    HateTags.assign(des.hatetags().begin(), des.hatetags().end());
    Birthday = des.birthday();
    PotentialMain1 = des.potentialmain1();
    PotentialMain2 = des.potentialmain2();
    PotentialAssistant1 = des.potentialassistant1();
    PotentialAssistant2 = des.potentialassistant2();
    PotentialMainContent1 = des.potentialmaincontent1();
    PotentialMainContent2 = des.potentialmaincontent2();
    PotentialAssistantContent1 = des.potentialassistantcontent1();
    PotentialAssistantContent2 = des.potentialassistantcontent2();

    return true;
}

void CharacterDesRes::OnLoad() {
    auto it = GameData::CharacterDataTable.find(std::to_string(Id));
    if (it != GameData::CharacterDataTable.end()) {
        it->second.Des = this;
    }
}

bool CharacterAdvanceRes::LoadFromPb(std::string data) {
    CharacterAdvance advance;
    if (!advance.ParseFromString(data)) {
        return false;
    }

    Id = advance.id();
    Group = advance.group();
    AdvanceLvl = advance.advancelvl();
    Tid1 = advance.tid1();
    Qty1 = advance.qty1();
    Tid2 = advance.tid2();
    Qty2 = advance.qty2();
    Tid3 = advance.tid3();
    Qty3 = advance.qty3();
    Tid4 = advance.tid4();
    Qty4 = advance.qty4();
    GoldQty = advance.goldqty();

    return true;
}

void CharacterAdvanceRes::OnLoad() {
    Materials.Items.clear();
    Materials.Add(Tid1, Qty1);
    Materials.Add(Tid2, Qty2);
    Materials.Add(Tid3, Qty3);
    Materials.Add(Tid4, Qty4);
    Materials.Add(GOLD_ITEM_ID, GoldQty);
}

bool CharacterSkillUpgradeRes::LoadFromPb(std::string data) {
    CharacterSkillUpgrade upgrade;
    if (!upgrade.ParseFromString(data)) {
        return false;
    }

    Id = upgrade.id();
    Group = upgrade.group();
    AdvanceNum = upgrade.advancenum();
    Tid1 = upgrade.tid1();
    Qty1 = upgrade.qty1();
    Tid2 = upgrade.tid2();
    Qty2 = upgrade.qty2();
    Tid3 = upgrade.tid3();
    Qty3 = upgrade.qty3();
    Tid4 = upgrade.tid4();
    Qty4 = upgrade.qty4();
    GoldQty = upgrade.goldqty();

    return true;
}

void CharacterSkillUpgradeRes::OnLoad() {
    Materials.Items.clear();
    Materials.Add(Tid1, Qty1);
    Materials.Add(Tid2, Qty2);
    Materials.Add(Tid3, Qty3);
    Materials.Add(Tid4, Qty4);
    Materials.Add(GOLD_ITEM_ID, GoldQty);

    UpgradeId = (Group * 100) + AdvanceNum;
    while (GameData::CharacterSkillUpgradeDataTable.find(std::to_string(UpgradeId)) != GameData::CharacterSkillUpgradeDataTable.end()) {
        ++UpgradeId;
    }
}

bool CharacterUpgradeRes::LoadFromPb(std::string data) {
    CharacterUpgrade upgrade;
    if (!upgrade.ParseFromString(data)) {
        return false;
    }

    Level = upgrade.level();
    Exp = upgrade.exp();
    return true;
}

void CharacterUpgradeRes::OnLoad() {
}

bool CharItemExpRes::LoadFromPb(std::string data) {
    CharItemExp exp;
    if (!exp.ParseFromString(data)) {
        return false;
    }

    ItemId = exp.itemid();
    ExpValue = exp.expvalue();
    return true;
}

void CharItemExpRes::OnLoad() {
}

bool CharacterSkinRes::LoadFromPb(std::string data) {
    CharacterSkin skin;
    if (!skin.ParseFromString(data)) {
        return false;
    }

    Id = skin.id();
    CharId = skin.charid();
    Type = skin.type();
    return true;
}

void CharacterSkinRes::OnLoad() {
    Released = false;
}

bool TalentGroupRes::LoadFromPb(std::string data) {
    TalentGroup group;
    if (!group.ParseFromString(data)) {
        return false;
    }

    Id = group.id();
    CharId = group.charid();
    PreGroup = group.pregroup();
    return true;
}

void TalentGroupRes::OnLoad() {
    MainTalent = nullptr;
    Talents.clear();
}

bool TalentRes::LoadFromPb(std::string data) {
    Talent talent;
    if (!talent.ParseFromString(data)) {
        return false;
    }

    Id = talent.id();
    Index = talent.index();
    Type = talent.type();
    GroupId = talent.groupid();
    Sort = talent.sort();
    return true;
}

void TalentRes::OnLoad() {
    auto it = GameData::TalentGroupDataTable.find(std::to_string(GroupId));
    if (it == GameData::TalentGroupDataTable.end()) {
        return;
    }

    if (Type == 1) {
        it->second.MainTalent = this;
    }
    else if (Type == 2) {
        it->second.Talents.push_back(this);
    }
}

bool CharGemRes::LoadFromPb(std::string data) {
    CharGem gem;
    if (!gem.ParseFromString(data)) {
        return false;
    }

    Id = gem.id();
    GenerateCostTid = gem.generatecosttid();
    RefreshCostTid = gem.refreshcosttid();
    OverlockCostTid = gem.overlockcosttid();
    Type = gem.type();
    return true;
}

void CharGemRes::OnLoad() {
}

bool CharGemSlotControlRes::LoadFromPb(std::string data) {
    CharGemSlotControl control;
    if (!control.ParseFromString(data)) {
        return false;
    }

    Id = control.id();
    Position = control.position();
    MaxAlterNum = control.maxalternum();
    UnlockLevel = control.unlocklevel();
    GeneratenCostQty = control.generatencostqty();
    RefreshCostQty = control.refreshcostqty();
    OverlockCostQty = control.overlockcostqty();
    OverlockDoraCostQty = control.overlockdoracostqty();
    LockableNum = control.lockablenum();
    LockItemTid = control.lockitemtid();
    LockItemQty = control.lockitemqty();
    return true;
}

void CharGemSlotControlRes::OnLoad() {
    UniqueAttrGroupProb = 0;
    UniqueAttrGroupId = 0;
    AttrGroupId.clear();

    switch (Id) {
    case 3:
        UniqueAttrGroupProb = 3300;
        UniqueAttrGroupId = 12;
        AttrGroupId = { 9, 10 };
        break;
    case 2:
        UniqueAttrGroupProb = 4000;
        UniqueAttrGroupId = 11;
        AttrGroupId = { 5, 6, 7, 8 };
        break;
    case 1:
    default:
        AttrGroupId = { 1, 2, 3, 4 };
        break;
    }
}

bool CharGemAttrValueRes::LoadFromPb(std::string data) {
    CharGemAttrValue attrValue;
    if (!attrValue.ParseFromString(data)) {
        return false;
    }

    Id = attrValue.id();
    TypeId = attrValue.typeid_();
    AttrType = attrValue.attrtype();
    AttrTypeFirstSubtype = attrValue.attrtypefirstsubtype();
    OverlockCount = attrValue.overlockcount();
    Rarity = attrValue.rarity();
    return true;
}

void CharGemAttrValueRes::OnLoad() {
}

bool AffinityLevelRes::LoadFromPb(std::string data) {
    AffinityLevel level;
    if (!level.ParseFromString(data)) {
        return false;
    }

    AffinityLevel = level.affinitylevel();
    NeedExp = level.needexp();
    return true;
}

void AffinityLevelRes::OnLoad() {
    if (AffinityLevel > MaxLevel) {
        MaxLevel = AffinityLevel;
    }
}

bool AffinityGiftRes::LoadFromPb(std::string data) {
    AffinityGift gift;
    if (!gift.ParseFromString(data)) {
        return false;
    }

    Id = gift.id();
    BaseAffinity = gift.baseaffinity();
    Tags.assign(gift.tags().begin(), gift.tags().end());
    return true;
}

void AffinityGiftRes::OnLoad() {
}

bool PlotRes::LoadFromPb(std::string data) {
    Plot plot;
    if (!plot.ParseFromString(data)) {
        return false;
    }

    Id = plot.id();
    Char = plot.char_();
    UnlockAffinityLevel = plot.unlockaffinitylevel();
    Rewards = plot.rewards();
    return true;
}

void PlotRes::OnLoad() {
    RewardItems = ItemParamMap::FromJsonString(Rewards);
}

bool ChatRes::LoadFromPb(std::string data) {
    Chat chat;
    if (!chat.ParseFromString(data)) {
        return false;
    }

    Id = chat.id();
    PreChatId = chat.prechatid();
    AddressBookId = chat.addressbookid();
    TriggerType = chat.triggertype();
    TriggerCond = chat.triggercond();
    TriggerCondParam = chat.triggercondparam();
    Reward1 = chat.reward1();
    RewardQty1 = chat.rewardqty1();
    return true;
}

void ChatRes::OnLoad() {
    auto it = GameData::CharacterDataTable.find(std::to_string(AddressBookId));
    if (it != GameData::CharacterDataTable.end()) {
        it->second.Chats.push_back(this);
    }
}

bool DatingLandmarkRes::LoadFromPb(std::string data) {
    DatingLandmark landmark;
    if (!landmark.ParseFromString(data)) {
        return false;
    }

    Id = landmark.id();
    return true;
}

void DatingLandmarkRes::OnLoad() {
    AfterBranches.clear();
    CharacterEvents.clear();
    LandmarkEvents.clear();
}

bool DatingLandmarkEventRes::LoadFromPb(std::string data) {
    DatingLandmarkEvent event;
    if (!event.ParseFromString(data)) {
        return false;
    }

    Id = event.id();
    DatingEventType = event.datingeventtype();
    Affinity = event.affinity();
    DatingEventParams.assign(event.datingeventparams().begin(), event.datingeventparams().end());
    Response = event.response();
    return true;
}

int DatingLandmarkEventRes::GetLandmarkId() const {
    return DatingEventParams.empty() ? 0 : DatingEventParams.front();
}

void DatingLandmarkEventRes::OnLoad() {
    Type = DatingEventType;

    auto it = GameData::DatingLandmarkDataTable.find(std::to_string(GetLandmarkId()));
    if (it == GameData::DatingLandmarkDataTable.end()) {
        return;
    }

    switch (Type) {
    case 3:
        it->second.LandmarkEvents[Response].push_back(this);
        break;
    case 4:
        it->second.CharacterEvents[Response].push_back(this);
        break;
    case 9:
        it->second.AfterBranches.push_back(this);
        break;
    default:
        break;
    }
}

bool DatingCharacterEventRes::LoadFromPb(std::string data) {
    DatingCharacterEvent event;
    if (!event.ParseFromString(data)) {
        return false;
    }

    Id = event.id();
    return true;
}

void DatingCharacterEventRes::OnLoad() {
}

bool DatingBranchRes::LoadFromPb(std::string data) {
    DatingBranch branch;
    if (!branch.ParseFromString(data)) {
        return false;
    }

    Id = branch.id();
    DatingEventType = branch.datingeventtype();
    DatingEventParams.assign(branch.datingeventparams().begin(), branch.datingeventparams().end());
    DatingEventExclude.assign(branch.datingeventexclude().begin(), branch.datingeventexclude().end());
    return true;
}

int DatingBranchRes::GetLandmarkId() const {
    return DatingEventParams.empty() ? 0 : DatingEventParams.front();
}

void DatingBranchRes::OnLoad() {
}
