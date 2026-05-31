#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class CharacterRes : public ResBase {
public:
    CharacterRes() = default;
    ~CharacterRes() override = default;

    CharacterRes(const CharacterRes&) = delete;
    CharacterRes& operator=(const CharacterRes&) = delete;

    CharacterRes(CharacterRes&&) noexcept = default;
    CharacterRes& operator=(CharacterRes&&) noexcept = default;

    int GetId() const override { return Id; }

    void OnLoad() override;

    bool LoadFromPb(std::string data) override;

    int AIId;
    int AdvanceGroup;
    int AdvanceSkinId;
    int AdvanceSkinUnlockLevel;
    int Ammo;
    int AssistAIId;
    int AssistDodgeId;
    int AssistNormalAtkId;
    int AssistSkillAngle;
    int AssistSkillId;
    int AssistSkillOnStageType;
    int AssistSkillRadius;
    int AssistSpecialSkillId;
    int AssistUltimateAngle;
    int AssistUltimateId;
    int AssistUltimateOnStageOrientation;
    int AssistUltimateOnStageType;
    int AssistUltimateRadius;
    int AtkSpd;
    std::string AttributeId;
    bool Available;
    int BulletType;
    int CharacterAttackType;
    int ChargingRate;
    int Class1;
    int DefaultSkinId;
    int DodgeId;
    bool DodgeToRunAccelerationOrNot;
    int EET;
    int EnergyConsume;
    int EnergyConvRatio;
    int EnergyEfficiency;
    int Faction;
    int FragmentsId;
    int FrozenTimeHighlightUnit;
    std::vector<int> GemSlots;
    int Grade;
    int HearAttackRng;
    int HearRng;
    int Id;
    int MovAcc;
    int MovType;
    std::string Name;
    int NormalAtkId;
    int PresentsTraitId;
    int RaiseGunRng;
    int RecruitmentQty;
    int RotAcc;
    int RotSpd;
    int RunSpd;
    int SearchTargetType;
    int SkillId;
    int SkillSemiAutoRng;
    std::vector<int> SkillsUpgradeGroup;
    int SpRunSpd;
    int SpecialSkillId;
    int SwitchCD;
    int TalentSkillId;
    int TransSpd;
    int TransformQty;
    int UltimateId;
    int UltimateSemiAutoRng;
    int ViewId;
    bool Visible;
    int VisionAttackRng;
    int VisionDeg;
    int VisionRng;
    int WalkSpd;
    int WalkToRunDuration;
    int Weight;
};

class CharacterAdvanceRes : public ResBase {
public:
    int Id;
    int Group;
    int AdvanceLvl;

    int Tid1;
    int Qty1;
    int Tid2;
    int Qty2;
    int Tid3;
    int Qty3;
    int Tid4;
    int Qty4;
    int GoldQty;

    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};


class CharacterSkillUpgradeRes : public ResBase {
public:
    int Id;
    int Group;
    int AdvanceNum;

    int Tid1;
    int Qty1;
    int Tid2;
    int Qty2;
    int Tid3;
    int Qty3;
    int Tid4;
    int Qty4;
    int GoldQty;

    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};

class CharacterUpgradeRes : public ResBase {
public:
    int Level;
    int Exp;

    int GetId() const override { return Level; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};

class CharItemExpRes : public ResBase {
public:
    int ItemId;
    int ExpValue;

    int GetId() const override { return ItemId; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};

class CharacterSkinRes : public ResBase {
public:
    int Id;
    int CharId;
    int Type;

    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};

class TalentGroupRes : public ResBase {
public:
    int Id;
    int CharId;
    int PreGroup;

    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};


class TalentRes : public ResBase {
public:
    int Id;
    int Index;
    int Type;
    int GroupId;
    int Sort;

    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};

class CharGemRes : public ResBase {
public:
    int Id;
    int GenerateCostTid;
    int RefreshCostTid;
    int OverlockCostTid;
    int Type;

    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};

class CharGemSlotControlRes : public ResBase {
public:
    int Id;
    int Position;
    int MaxAlterNum;
    int UnlockLevel;

    int GeneratenCostQty;
    int RefreshCostQty;
    int OverlockCostQty;
    int OverlockDoraCostQty;

    int LockableNum;
    int LockItemTid;
    int LockItemQty;

    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};

class CharGemAttrValueRes : public ResBase {
public:

    int Id;
    int TypeId;
    int AttrType;
    int AttrTypeFirstSubtype;
    int OverlockCount;
    int Rarity;
    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};

class AffinityLevelRes : public ResBase {
public:
    AffinityLevelRes() = default;

    int AffinityLevel;
    int NeedExp;
    int GetId() const override { return AffinityLevel; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};

class AffinityGiftRes : public ResBase {
public:
    int Id;
    int BaseAffinity;
    std::vector<int> Tags;
    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};

class PlotRes : public ResBase {
public:
    int Id;
    int Char;
    int UnlockAffinityLevel;
    std::string Rewards;
    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};


class ChatRes : public ResBase {
public:
    int Id;
    int AddressBookId;
    int PreChatId;

    int TriggerType;
    int TriggerCond;
    std::string TriggerCondParam;

    int Reward1;
    int RewardQty1;

    int GetId() const override { return Id; }

    void OnLoad() override {}

    bool LoadFromPb(std::string data) override;
};

class DatingLandmarkRes : public ResBase {
public:
    int Id;

    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};

class DatingLandmarkEventRes : public ResBase {
public:
    int Id;
    int DatingEventType;
    int Affinity;
    std::vector<int> DatingEventParams;
    std::string Response;
    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};

class DatingCharacterEventRes : public ResBase {
public:
    int Id;
    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
};