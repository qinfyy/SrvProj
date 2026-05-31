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