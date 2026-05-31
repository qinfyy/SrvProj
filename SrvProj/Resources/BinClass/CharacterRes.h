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