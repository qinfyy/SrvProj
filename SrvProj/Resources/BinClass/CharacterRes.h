#pragma once

#include "../ResBase.h"
#include "../ResourceDerivedData.h"

#include <string>
#include <unordered_map>
#include <vector>

class ChatRes;
class CharacterDesRes;
class DatingLandmarkEventRes;
class HonorRes;
class TalentRes;

class CharacterRes : public ResBase {
public:
    CharacterRes() = default;
    ~CharacterRes() override = default;

    CharacterRes(const CharacterRes&) = delete;
    CharacterRes& operator=(const CharacterRes&) = delete;

    CharacterRes(CharacterRes&&) noexcept = default;
    CharacterRes& operator=(CharacterRes&&) noexcept = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
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

    // 非序列化字段
    CharacterDesRes* Des = nullptr;
    HonorRes* Honor = nullptr;
    int ElementType = 0;
    std::vector<ChatRes*> Chats;
};

class CharacterDesRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;
    std::string Alias;
    std::string CnCv;
    std::string JpCv;
    std::string CharColor;
    std::string CharSkillColor;
    std::string CharDes;
    std::vector<int> Tag;
    int Force;
    std::vector<int> PreferTags;
    std::vector<int> HateTags;
    std::string Birthday;
    std::string PotentialMain1;
    std::string PotentialMain2;
    std::string PotentialAssistant1;
    std::string PotentialAssistant2;
    std::string PotentialMainContent1;
    std::string PotentialMainContent2;
    std::string PotentialAssistantContent1;
    std::string PotentialAssistantContent2;

    // 非序列化字段
};

class CharacterAdvanceRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
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

    // 非序列化字段
    ItemParamMap Materials;
};

class CharacterSkillUpgradeRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(UpgradeId > 0 ? UpgradeId : Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
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

    // 非序列化字段
    int UpgradeId = 0;
    ItemParamMap Materials;
};

class CharacterUpgradeRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Level); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Level;
    int Exp;

    // 非序列化字段
};

class CharItemExpRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(ItemId); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int ItemId;
    int ExpValue;

    // 非序列化字段
};

class CharacterSkinRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;
    int CharId;
    int Type;

    // 非序列化字段
    bool Released = false;
};

class TalentGroupRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;
    int CharId;
    int PreGroup;

    // 非序列化字段
    TalentRes* MainTalent = nullptr;
    std::vector<TalentRes*> Talents;
};

class TalentRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;
    int Index;
    int Type;
    int GroupId;
    int Sort;

    // 非序列化字段
};

class CharGemRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;
    int GenerateCostTid;
    int RefreshCostTid;
    int OverlockCostTid;
    int Type;

    // 非序列化字段
};

class CharGemSlotControlRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
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

    // 非序列化字段
    int UniqueAttrGroupProb = 0;
    int UniqueAttrGroupId = 0;
    std::vector<int> AttrGroupId;
};

class CharGemAttrValueRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;
    int TypeId;
    int AttrType;
    int AttrTypeFirstSubtype;
    int OverlockCount;
    int Rarity;

    // 非序列化字段
};

class AffinityLevelRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(AffinityLevel); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int AffinityLevel;
    int NeedExp;

    // 非序列化字段
    static int MaxLevel;
};

class AffinityGiftRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;
    int BaseAffinity;
    std::vector<int> Tags;

    // 非序列化字段
};

class PlotRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;
    int Char;
    int UnlockAffinityLevel;
    std::string Rewards;

    // 非序列化字段
    ItemParamMap RewardItems;
};

class ChatRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;
    int AddressBookId;
    int PreChatId;
    int TriggerType;
    int TriggerCond;
    std::string TriggerCondParam;
    int Reward1;
    int RewardQty1;

    // 非序列化字段
};

class DatingLandmarkRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;

    // 非序列化字段
    std::vector<DatingLandmarkEventRes*> AfterBranches;
    std::unordered_map<std::string, std::vector<DatingLandmarkEventRes*>> CharacterEvents;
    std::unordered_map<std::string, std::vector<DatingLandmarkEventRes*>> LandmarkEvents;
};

class DatingLandmarkEventRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    int GetLandmarkId() const;
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;
    int DatingEventType;
    int Affinity;
    std::vector<int> DatingEventParams;
    std::string Response;

    // 非序列化字段
    int Type = 0;
};

class DatingCharacterEventRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;

    // 非序列化字段
};

class DatingBranchRes : public ResBase {
public:
    std::string GetId() const override { return std::to_string(Id); }
    int GetLandmarkId() const;
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;
    int DatingEventType;
    std::vector<int> DatingEventParams;
    std::vector<int> DatingEventExclude;

    // 非序列化字段
};
