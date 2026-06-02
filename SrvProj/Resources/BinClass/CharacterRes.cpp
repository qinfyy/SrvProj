#include "CharacterRes.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool CharacterRes::LoadFromPb(std::string data) {
    nova::client::Character character;
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
    GemSlots.clear();
    for (const auto& slot : character.gemslots()) {
        GemSlots.push_back(slot);
    }

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
    SkillsUpgradeGroup.clear();
    for (const auto& slot : character.gemslots()) {
        SkillsUpgradeGroup.push_back(slot);
    }

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
    //pass
}

bool ChatRes::LoadFromPb(std::string data) {
    nova::client::Chat chat;
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

bool CharacterAdvanceRes::LoadFromPb(std::string data) {
    nova::client::CharacterAdvance chat;
    if (!chat.ParseFromString(data)) {
        return false;
    }

    Id = chat.id();
    Group = chat.group();
    AdvanceLvl = chat.advancelvl();
    Tid1 = chat.tid1();
    Qty1 = chat.qty1();
    Tid2 = chat.tid2();
    Qty2 = chat.qty2();
    Tid3 = chat.tid3();
    Qty3 = chat.qty3();
    Tid4 = chat.tid4();
    Qty4 = chat.qty4();
    GoldQty = chat.goldqty();

    return true;
}

bool CharacterSkillUpgradeRes::LoadFromPb(std::string data) {
    nova::client::CharacterSkillUpgrade csu;
    if (!csu.ParseFromString(data)) {
        return false;
    }

    Id = csu.id();
    Group = csu.group();
    AdvanceNum = csu.advancenum();
    Tid1 = csu.tid1();
    Qty1 = csu.qty1();
    Tid2 = csu.tid2();
    Qty2 = csu.qty2();
    Tid3 = csu.tid3();
    Qty3 = csu.qty3();
    Tid4 = csu.tid4();
    Qty4 = csu.qty4();
    GoldQty = csu.goldqty();

    return true;
}

bool CharacterUpgradeRes::LoadFromPb(std::string data) {
    nova::client::CharacterUpgrade cu;
    if (!cu.ParseFromString(data)) {
        return false;
    }
    Level = cu.level();
    Exp = cu.exp();
    return true;
}

bool AffinityLevelRes::LoadFromPb(std::string data)
{
    nova::client::AffinityLevel al;
    if (!al.ParseFromString(data)) {
        return false;
    }
    AffinityLevel = al.affinitylevel();
    NeedExp = al.needexp();

    return true;
}

bool DatingCharacterEventRes::LoadFromPb(std::string data)
{
    nova::client::DatingCharacterEvent dce;
    if (!dce.ParseFromString(data)) {
        return false;
    }
    Id = dce.id();
    return true;
}

bool DatingLandmarkEventRes::LoadFromPb(std::string data)
{
    nova::client::DatingLandmarkEvent dle;
    if (!dle.ParseFromString(data)) {
        return false;
    }
    Id = dle.id();
    DatingEventType = dle.datingeventtype();
    Affinity = dle.affinity();
    DatingEventParams.clear();
    for (const auto& param : dle.datingeventparams()) {
        DatingEventParams.push_back(param);
    }
    return true;
}

bool DatingLandmarkRes::LoadFromPb(std::string data)
{
    nova::client::DatingLandmark dl;
    if (!dl.ParseFromString(data)) {
        return false;
    }
    Id = dl.id();
    return true;
}


bool AffinityGiftRes::LoadFromPb(std::string data)
{
    AffinityGift ag;
    if (!ag.ParseFromString(data)) {
        return false;
    }
    Id = ag.id();
    BaseAffinity = ag.baseaffinity();
    Tags.clear();
    for (const auto& tag : ag.tags()) {
        Tags.push_back(tag);
    }
        
    return true;
}

bool CharGemAttrValueRes::LoadFromPb(std::string data)
{
    CharGemAttrValue cga;
    if (!cga.ParseFromString(data)) {
        return false;
    }
    Id = cga.id();
    TypeId = cga.typeid_();
    AttrType = cga.attrtype();
    AttrTypeFirstSubtype = cga.attrtypefirstsubtype();
    OverlockCount = cga.overlockcount();
    Rarity = cga.rarity();


    return true;
}

bool CharGemSlotControlRes::LoadFromPb(std::string data)
{
    CharGemSlotControl cgsc;
    if (!cgsc.ParseFromString(data)) {
        return false;
    }
    Id = cgsc.id();
    Position = cgsc.position();
    MaxAlterNum = cgsc.maxalternum();
    UnlockLevel = cgsc.unlocklevel();
    GeneratenCostQty = cgsc.generatencostqty();
    RefreshCostQty = cgsc.refreshcostqty();

    OverlockCostQty = cgsc.overlockcostqty();
    OverlockDoraCostQty = cgsc.overlockdoracostqty();
    LockableNum = cgsc.lockablenum();
    LockItemTid = cgsc.lockitemtid();
    LockItemQty = cgsc.lockitemqty();

    return true;
}

bool CharGemRes::LoadFromPb(std::string data)
{
    CharGem cg;
    if (!cg.ParseFromString(data)) {
        return false;
    }
    Id = cg.id();
    GenerateCostTid = cg.generatecosttid();
    RefreshCostTid = cg.refreshcosttid();
    OverlockCostTid = cg.overlockcosttid();
    Type = cg.type();

    return true;
}

bool TalentRes::LoadFromPb(std::string data)
{
    Talent t;
    if (!t.ParseFromString(data)) {
        return false;
    }
    Id = t.id();
    Index = t.index();
    Type = t.type();
    GroupId = t.groupid();
    Sort = t.sort();

    return true;
}

bool CharacterSkinRes::LoadFromPb(std::string data)
{
    CharacterSkin cs;
    if (!cs.ParseFromString(data)) {
        return false;
    }
    Id = cs.id();
    CharId = cs.charid();
    Type = cs.type();

    return true;
}

bool CharItemExpRes::LoadFromPb(std::string data)
{
    CharItemExp cie;
    if (!cie.ParseFromString(data)) {
        return false;
    }
    ItemId = cie.itemid();
    ExpValue = cie.expvalue();

    return true;
}

bool PlotRes::LoadFromPb(std::string data)
{
    Plot p;
    if (!p.ParseFromString(data)) {
        return false;
    }
    Id = p.id();
    Char = p.char_();
    UnlockAffinityLevel = p.unlockaffinitylevel();
    Rewards = p.rewards();
    return true;
}

bool TalentGroupRes::LoadFromPb(std::string data)
{
    TalentGroup tg;
    if (!tg.ParseFromString(data)) {
        return false;
    }
    Id = tg.id();
    CharId = tg.charid();
    PreGroup = tg.pregroup();

    return true;
}
