#include "Player.h"
#include <chrono>
#include "../Util.h"

#include "../proto/proto_cpp/public.pb.h"

PlayerInfo Player::ToProto() const {
	PlayerInfo info;

	// === Acc ===
	auto* pAcc = info.mutable_acc();
	pAcc->set_createtime(1779101812);
	pAcc->set_headicon(102);
	pAcc->set_nickname("me");
	pAcc->set_skinid(10301);
	pAcc->set_titleprefix(1);
	pAcc->set_titlesuffix(2);

	// Newbies
	auto* pNewbies = pAcc->mutable_newbies();

	std::vector<int> newbieGroups = {
		25, 49, 50, 8, 9, 232, 24, 26, 16, 17, 23, 18, 106, 303, 27, 47, 48, 51, 304, 302, 32, 52, 201, 46, 41, 45, 44, 42, 43,
		301, 29, 202, 4, 12, 13, 28, 102, 21, 22, 20, 104, 105, 101, 2, 6, 15, 14, 11, 10, 3, 7, 5, 1
	};

	for (int groupId : newbieGroups) {
		auto* pNewbie = pNewbies->Add();
		pNewbie->set_groupid(groupId);
		pNewbie->set_stepid(-1);
	}

	// === Achievements ===
	info.set_achievements(std::string(64, '\0'));

	// === Activities ===
	auto* pActivities = info.mutable_activities();
	auto* pActivity1 = pActivities->Add();
	pActivity1->set_id(102002);
	pActivity1->set_starttime(1);
	pActivity1->set_endtime(INT32_MAX);

	auto* pActivity2 = pActivities->Add();
	pActivity2->set_id(301031);
	pActivity2->set_starttime(1);
	pActivity2->set_endtime(INT32_MAX);

	auto* pActivity3 = pActivities->Add();
	pActivity3->set_id(700118);
	pActivity3->set_starttime(1);
	pActivity3->set_endtime(INT32_MAX);

	auto* pActivity4 = pActivities->Add();
	pActivity4->set_id(700117);
	pActivity4->set_starttime(1);
	pActivity4->set_endtime(INT32_MAX);


	// === Agent ===
	auto* pAgent = info.mutable_agent();

	// === Board ===
	auto* pBoard = info.mutable_board();
	pBoard->Add(410301);

	// === CharGemInstances ===
	auto* pCharGemInstances = info.mutable_chargeminstances();
	auto* pCharGemInstance0 = pCharGemInstances->Add();
	pCharGemInstance0->set_id(1000);
	pCharGemInstance0->set_star(1);
	auto* pCharGemInstance1 = pCharGemInstances->Add();
	pCharGemInstance1->set_id(2002);
	pCharGemInstance1->set_star(1);
	auto* pCharGemInstance2 = pCharGemInstances->Add();
	pCharGemInstance2->set_id(1002);
	pCharGemInstance2->set_star(1);
	auto* pCharGemInstance3 = pCharGemInstances->Add();
	pCharGemInstance3->set_id(2001);
	pCharGemInstance3->set_star(1);
	auto* pCharGemInstance4 = pCharGemInstances->Add();
	pCharGemInstance4->set_id(2000);
	pCharGemInstance4->set_star(1);
	auto* pCharGemInstance5 = pCharGemInstances->Add();
	pCharGemInstance5->set_id(1001);
	pCharGemInstance5->set_star(1);
	auto* pCharGemInstance6 = pCharGemInstances->Add();
	pCharGemInstance6->set_id(3002);
	pCharGemInstance6->set_star(1);
	auto* pCharGemInstance7 = pCharGemInstances->Add();
	pCharGemInstance7->set_id(3000);
	pCharGemInstance7->set_star(1);
	auto* pCharGemInstance8 = pCharGemInstances->Add();
	pCharGemInstance8->set_id(3001);
	pCharGemInstance8->set_star(1);

	// === Chars ===
	auto* pChars = info.mutable_chars();
	// Char 103
	auto* pChar0 = pChars->Add();
	// CharGemPresets
	auto* pCharGemPresets0_0 = pChar0->mutable_chargempresets();
	auto* pSlotGem0_0_0 = pCharGemPresets0_0->add_chargempresets();
	for (int i = 1; i <= 3; i++) {
		pSlotGem0_0_0->add_slotgem(-1);
	}
	// CharGemSlots
	auto* pCharGemSlots0_0 = pChar0->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot0_0_0 = pCharGemSlots0_0->Add();
		pSlot0_0_0->set_id(i);
	}
	pChar0->set_createtime(1779101812);
	pChar0->set_level(1);
	for (int i = 0; i < 5; i++) {
		pChar0->add_skilllvs(1);
	}
	pChar0->set_skin(10301);
	pChar0->set_talentnodes("\x00\x00\x00\x00\x00\x00\x00\x00");
	pChar0->set_tid(103);
	// Char 113
	auto* pChar1 = pChars->Add();
	// CharGemPresets
	auto* pCharGemPresets1_0 = pChar1->mutable_chargempresets();
	auto* pSlotGem1_0_0 = pCharGemPresets1_0->add_chargempresets();
	for (int i = 1; i <= 3; i++) {
		pSlotGem1_0_0->add_slotgem(-1);
	}
	// CharGemSlots
	auto* pCharGemSlots1_0 = pChar1->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot1_0_0 = pCharGemSlots1_0->Add();
		pSlot1_0_0->set_id(i);
	}
	pChar1->set_createtime(1779101812);
	pChar1->set_level(1);
	for (int i = 0; i < 5; i++) {
		pChar1->add_skilllvs(1);
	}
	pChar1->set_skin(11301);
	pChar1->set_talentnodes("\x00\x00\x00\x00\x00\x00\x00\x00");
	pChar1->set_tid(113);
	// Char 112
	auto* pChar2 = pChars->Add();
	// CharGemPresets
	auto* pCharGemPresets2_0 = pChar2->mutable_chargempresets();
	auto* pSlotGem2_0_0 = pCharGemPresets2_0->add_chargempresets();
	for (int i = 1; i <= 3; i++) {
		pSlotGem2_0_0->add_slotgem(-1);
	}
	// CharGemSlots
	auto* pCharGemSlots2 = pChar2->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots2->Add();
		pSlot->set_id(i);
	}
	pChar2->set_createtime(1779101812);
	pChar2->set_level(1);
	for (int i = 0; i < 5; i++) {
		pChar2->add_skilllvs(1);
	}
	pChar2->set_skin(11201);
	pChar2->set_talentnodes("\x00\x00\x00\x00\x00\x00\x00\x00");
	pChar2->set_tid(112);
	

	// === DailyInstances ===
	auto* pDailyInstances = info.mutable_dailyinstances();
	auto* pDailyInstance0 = pDailyInstances->Add();
	pDailyInstance0->set_id(1004);
	pDailyInstance0->set_star(1);
	auto* pDailyInstance1 = pDailyInstances->Add();
	pDailyInstance1->set_id(1000);
	pDailyInstance1->set_star(1);
	auto* pDailyInstance2 = pDailyInstances->Add();
	pDailyInstance2->set_id(1002);
	pDailyInstance2->set_star(1);
	auto* pDailyInstance3 = pDailyInstances->Add();
	pDailyInstance3->set_id(1005);
	pDailyInstance3->set_star(1);
	auto* pDailyInstance4 = pDailyInstances->Add();
	pDailyInstance4->set_id(1001);
	pDailyInstance4->set_star(1);
	auto* pDailyInstance5 = pDailyInstances->Add();
	pDailyInstance5->set_id(1003);
	pDailyInstance5->set_star(1);

	// === DailyMallRewardStatus ===
	info.set_dailymallrewardstatus(true);

	// === DailyShopRewardStatus ===
	info.set_dailyshoprewardstatus(true);

	// === Dictionaries ===
	auto* pDictionaries = info.mutable_dictionaries();
	// Dictionary 0 (TabId: 2)
	auto* pDictionary0 = pDictionaries->Add();
	pDictionary0->set_tabid(2);
	auto* pEntries0_0 = pDictionary0->mutable_entries();
	for (int i = 1; i <= 10; ++i)
	{
		auto* pEntry_0 = pEntries0_0->Add();
		pEntry_0->set_index(i);
		pEntry_0->set_status(2);
	}
	// Dictionary 1 (TabId: 1)
	auto* pDictionary1 = pDictionaries->Add();
	pDictionary1->set_tabid(1);
	auto* pEntries1_0 = pDictionary1->mutable_entries();
	for (int i = 1; i <= 6; ++i)
	{
		auto* pEntry_1 = pEntries1_0->Add();
		pEntry_1->set_index(i);
		pEntry_1->set_status(2);
	}
	// Dictionary 3 (TabId: 3)
	auto* pDictionary2 = pDictionaries->Add();
	pDictionary2->set_tabid(3);
	auto* pEntries2_0 = pDictionary2->mutable_entries();
	for (int i = 1; i <= 7; ++i)
	{
		auto* pEntry_2 = pEntries2_0->Add();
		pEntry_2->set_index(i);
		pEntry_2->set_status(2);
	}

	// === Discs ===
	auto* pDiscs = info.mutable_discs();
	auto* pDisc0 = pDiscs->Add();
	pDisc0->set_id(211005);
	pDisc0->set_level(1);
	pDisc0->set_createtime(1779101812);
	auto* pDisc1 = pDiscs->Add();
	pDisc1->set_id(211007);
	pDisc1->set_level(1);
	pDisc1->set_createtime(1779101812);
	auto* pDisc2 = pDiscs->Add();
	pDisc2->set_id(211001);
	pDisc2->set_level(1);
	pDisc2->set_createtime(1779101812);
	auto* pDisc3 = pDiscs->Add();
	pDisc3->set_id(211008);
	pDisc3->set_level(1);
	pDisc3->set_createtime(1779101812);

	// === Energy ===
	auto* pEnergy = info.mutable_energy();
	pEnergy->mutable_energy()->set_isprimary(true);
	pEnergy->mutable_energy()->set_nextduration(1);
	pEnergy->mutable_energy()->set_primary(240);
	pEnergy->mutable_energy()->set_updatetime(1779101813);

	// === Formation ===
	auto* pFormation = info.mutable_formation();

	// === Handbook ===
	auto* pHandbook = info.mutable_handbook();
	auto* pHandbook0 = pHandbook->Add();
	pHandbook0->set_type(1);
	pHandbook0->set_data(Base64Decode("AAAAAAABIAE="));
	auto* pHandbook1 = pHandbook->Add();
	pHandbook1->set_type(2);
	pHandbook1->set_data(Base64Decode("AAAAAAAAAAA="));
	// Handbook 2
	auto* pHandbook2 = pHandbook->Add();
	pHandbook2->set_type(3);
	pHandbook2->set_data(Base64Decode("AAAAAAAAAAA="));

	// === Honors ===
	auto* pHonors = info.mutable_honorlist();

	// === Items ===
	auto* pItems = info.mutable_items();
	auto* pItem0 = pItems->Add();
	pItem0->set_tid(30003);
	pItem0->set_qty(2);

	// === Phone ===
	auto* pPhone = info.mutable_phone();
	pPhone->set_newmessage(3);

	// === Quests ===
	auto* pQuests = info.mutable_quests();
	auto* pQuestList = pQuests->mutable_list();
	struct QuestInit
	{
		int id;
		int cur;
		int max;
		int status;
		QuestType type;
		bool hasCur;
		bool hasStatus;
	};

	static const QuestInit quests[] =
	{
		// Daily
		{2002, 0, 1,    0, Daily,  false, false},
		{2003, 0, 1,    0, Daily,  false, false},
		{2001, 0, 1,    0, Daily,  false, false},
		{2004, 0, 1,    0, Daily,  false, false},
		{2005, 0, 1,    0, Daily,  false, false},

		// Weekly
		{1005, 0, 10,   0, Weekly, false, false},
		{1004, 0, 5,    0, Weekly, false, false},
		{1007, 0, 40,   0, Weekly, false, false},
		{1006, 0, 20,   0, Weekly, false, false},

		{1001, 1, 1,    1, Weekly, true,  true},
		{1003, 0, 3,    0, Weekly, false, false},
		{1002, 1, 5,    0, Weekly, true,  false},

		// Daily
		{1002, 0, 1,    0, Daily,  false, false},
		{1003, 0, 5,    0, Daily,  false, false},

		{1001, 1, 1,    1, Daily,  true,  true},

		{1006, 0, 1,    0, Daily,  false, false},
		{1007, 0, 1,    0, Daily,  false, false},
		{1004, 0, 100,  0, Daily,  false, false},
		{1005, 0, 1,    0, Daily,  false, false},
		{1010, 0, 1,    0, Daily,  false, false},
		{1008, 0, 1,    0, Daily,  false, false},
		{1009, 0, 1,    0, Daily,  false, false},

		// Weekly
		{1013, 0, 3,    0, Weekly, false, false},
		{1012, 0, 10,   0, Weekly, false, false},
		{1009, 0, 1000, 0, Weekly, false, false},
		{1008, 0, 500,  0, Weekly, false, false},
		{1011, 0, 5,    0, Weekly, false, false},
		{1010, 0, 5,    0, Weekly, false, false},
	};

	for (const auto& q : quests)
	{
		auto* pQuest = pQuestList->Add();
		pQuest->set_id(q.id);
		pQuest->set_type(q.type);
		if (q.hasStatus)
		{
			pQuest->set_status(q.status);
		}
		auto* pProgress = pQuest->add_progress();
		if (q.hasCur)
		{
			pProgress->set_cur(q.cur);
		}

		pProgress->set_max(q.max);
	}

	// === RegionBossLevels ===
	auto* pRegionBossLevels = info.mutable_regionbosslevels();
	static const int regionBossIds[] = { 6010105, 6010107, 6010101, 6011107, 6009103, 6011105, 6011103, 6009101, 6011102, 6011101, 6009108, 6009105, 6009104, 6009107, 6010106, 6009106, 6010104, 6010102, 6011108, 6011106, 6011104, 6009102, 6010108, 6010103 };
	for (int id : regionBossIds)
	{
		auto* pLevel = pRegionBossLevels->Add();

		pLevel->set_id(id);
		pLevel->set_star(1);
	}

	// === Res ===
	auto* pRes = info.mutable_res();
	auto* pRes1 = pRes->Add();
	pRes1->set_tid(36);
	pRes1->set_qty(3);
	auto* pRes2 = pRes->Add();
	pRes2->set_tid(28);
	pRes2->set_qty(3);

	// === RglPassedIds ===
	auto* pRglPassedIds = info.mutable_rglpassedids();
	static const int rglPassedIds[] = { 107, 103, 206, 202, 309, 305, 102, 306, 401, 108, 308, 104, 106, 105, 303, 307, 109, 204, 304, 302, 203, 207, 205, 209, 208 };
	for (int id : rglPassedIds)
	{
		pRglPassedIds->Add(id);
	}

	// === ServerTs ===
	info.set_serverts(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());

	// === SignInIndex ===
	info.set_signinindex(1);

	// === SkillInstances ===
	auto* pSkillInstances = info.mutable_skillinstances();
	static const int skillInstanceIds[] = { 1004, 3005, 1000, 2003, 2002, 1002, 2001, 2000, 2005, 2004, 1005, 1001, 1003, 3004, 3003, 3002, 3000, 3001 };
	for (int id : skillInstanceIds)
	{
		auto* pSkill = pSkillInstances->Add();

		pSkill->set_id(id);
		pSkill->set_star(1);
	}
	

	// === State ===
	auto* pState = info.mutable_state();
	pState->mutable_achievement();
	auto* pBattlePass = pState->mutable_battlepass();
	pBattlePass->set_state(1);
	pState->mutable_friendenergy();
	auto* pMail = pState->mutable_mail();
	pMail->set_new_(true);
	pState->mutable_mallpackage();
	pState->mutable_scoreboss();
	pState->mutable_startower();
	pState->mutable_startowerbook();
	pState->set_storyset(true);
	auto* pTravelerDuelQuest = pState->mutable_travelerduelquest();
	pTravelerDuelQuest->set_type(TravelerDuel);
	auto* pWorldClassReward = pState->mutable_worldclassreward();
	pWorldClassReward->set_flag("\x00\x00\x00\x00\x00\x00\x00\x00", 8);


	// === Story ===
	auto* pStory = info.mutable_story();

	// === Titles ===
	auto* pTitles = info.mutable_titles();
	pTitles->Add()->set_titleid(2);
	pTitles->Add()->set_titleid(1);

	// === TourGuideQuestGroup ===
	info.set_tourguidequestgroup(9);

	// === VampireSurvivorRecord ===
	auto* pVampireSurvivorRecord = info.mutable_vampiresurvivorrecord();
	static const int vampireRecordIds[] = { 103, 101, 202, 107, 309, 204, 306, 305, 304, 302, 201, 203, 207, 102, 308, 205, 307, 303, 301, 106, 105, 104, 206 };
	auto* pRecords = pVampireSurvivorRecord->mutable_records();
	for (int id : vampireRecordIds)
	{
		auto* pRecord = pRecords->Add();
		pRecord->set_id(id);
		pRecord->set_passed(true);
	}
	pVampireSurvivorRecord->mutable_season();

	// === WeekBossLevels ===
	auto* pWeekBossLevels = info.mutable_weekbosslevels();
	static const int weekBossIds[] = { 6201103, 6204102, 6201101, 6203102, 6206102, 6206101, 6206103, 6203101, 6204101, 6201102, 6204103, 6203103, 6205103, 6202103, 6205101, 6202102, 6202101, 6205102 };
	for (int id : weekBossIds)
	{
		auto* pLevel = pWeekBossLevels->Add();
		pLevel->set_id(id);
	}

	// === WorldClass ===
	auto* pWorldClass = info.mutable_worldclass();
	pWorldClass->set_cur(1);

	return info;
}

void Player::Init() {
	return;
}