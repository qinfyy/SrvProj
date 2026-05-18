#include "Player.h"
#include <chrono>
#include "../Util.h"

#include "../proto/proto_cpp/public.pb.h"

PlayerInfo Player::ToProto() const {
	PlayerInfo info;
	info.set_serverts(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	//info.set_musicinfo(mPlayerBin.music());
	info.set_signinindex(8);

	//auto* pAcc = info.mutable_acc();
	//pAcc->set_createtime(mPlayerBin.createtime());
	//pAcc->set_headicon(mPlayerBin.headicon());
	//pAcc->set_signature(mPlayerBin.signature());
	//pAcc->set_nickname(mPlayerBin.name());
	//pAcc->set_skinid(mPlayerBin.skinid());
	//pAcc->set_titleprefix(mPlayerBin.titleprefix());
	//pAcc->set_titlesuffix(mPlayerBin.titlesuffix());

// === Acc ===
	auto* pAcc = info.mutable_acc();
	pAcc->set_createtime(1760971319);
	pAcc->set_headicon(101);
	pAcc->set_nickname("Cyt");
	pAcc->set_skinid(10301);
	pAcc->set_titleprefix(3);
	pAcc->set_titlesuffix(2);

	// Newbies (新手引导)
	auto* pNewbies = pAcc->mutable_newbies();

	// Newbie entries - GroupId 1 to 48 with StepId -1
	std::vector<int> newbieGroups = {
		1, 2, 3, 4, 21, 105, 102, 101, 106, 5, 6, 7, 8, 41, 10, 42, 9, 11, 12, 27, 25, 28, 29, 14, 16, 44, 45, 46, 51,
		17, 18, 22, 43, 23, 24, 49, 20, 47, 48, 32, 50, 52, 104, 301, 302, 303, 304
	};

	for (int groupId : newbieGroups) {
		auto* pNewbie = pNewbies->Add();
		pNewbie->set_groupid(groupId);
		pNewbie->set_stepid(-1);
	}

	// === Activities ===
	auto* pActivities = info.mutable_activities();
	auto* pActivity1 = pActivities->Add();
	pActivity1->set_id(1010703);
	pActivity1->set_starttime(1778731200);
	pActivity1->set_endtime(INT32_MAX);

	auto* pActivity2 = pActivities->Add();
	pActivity2->set_id(1010701);
	pActivity2->set_starttime(1778731200);
	pActivity2->set_endtime(INT32_MAX);

	auto* pActivity3 = pActivities->Add();
	pActivity3->set_id(400009);
	pActivity3->set_starttime(1778731200);
	pActivity3->set_endtime(INT32_MAX);

	auto* pActivity112 = pActivities->Add(); // pActivity 写错了，哈哈
	pActivity112->set_id(301033);
	pActivity112->set_starttime(1778212800);
	pActivity112->set_endtime(INT32_MAX);

	auto* pActivity4 = pActivities->Add();
	pActivity4->set_id(1010704);
	pActivity4->set_starttime(1778731200);
	pActivity4->set_endtime(INT32_MAX);

	auto* pActivity5 = pActivities->Add();
	pActivity5->set_id(101002);
	pActivity5->set_starttime(1760971319);
	pActivity5->set_endtime(INT64_MAX);

	auto* pActivity6 = pActivities->Add();
	pActivity6->set_id(101003);
	pActivity6->set_starttime(1761647851);
	pActivity6->set_endtime(INT64_MAX);

	auto* pActivity7 = pActivities->Add();
	pActivity7->set_id(201004);
	pActivity7->set_starttime(1760971319);
	pActivity7->set_endtime(INT64_MAX);

	auto* pActivity8 = pActivities->Add();
	pActivity8->set_id(700118);
	pActivity8->set_starttime(1777521600);
	pActivity8->set_endtime(INT32_MAX);

	auto* pActivity9 = pActivities->Add();
	pActivity9->set_id(700119);
	pActivity9->set_starttime(1778731200);
	pActivity9->set_endtime(INT32_MAX);

	auto* pActivity10 = pActivities->Add();
	pActivity10->set_id(1010702);
	pActivity10->set_starttime(1778731200);
	pActivity10->set_endtime(INT32_MAX);

	// === Agent ===
	auto* pAgent = info.mutable_agent();
	auto* pInfo1 = pAgent->add_infos();
	pInfo1->set_id(100201);
	pInfo1->set_processtime(1200);
	pInfo1->set_starttime(1778985981);
	pInfo1->add_charids(112);

	auto* pInfo2 = pAgent->add_infos();
	pInfo2->set_id(200201);
	pInfo2->set_processtime(1200);
	pInfo2->set_starttime(1778985981);
	pInfo2->add_charids(103);

	auto* pInfo3 = pAgent->add_infos();
	pInfo3->set_id(400201);
	pInfo3->set_processtime(1200);
	pInfo3->set_starttime(1778985981);
	pInfo3->add_charids(126);

	auto* pInfo4 = pAgent->add_infos();
	pInfo4->set_id(300201);
	pInfo4->set_processtime(1200);
	pInfo4->set_starttime(1778985981);
	pInfo4->add_charids(111);

	// === Board ===
	auto* pBoard = info.mutable_board();
	pBoard->Add(411201);
	pBoard->Add(413401);
	pBoard->Add(410301);
	pBoard->Add(414201);
	pBoard->Add(411501);

	// === CharGemInstances ===
	auto* pCharGemInstances = info.mutable_chargeminstances();
	auto* pCharGemInstance1 = pCharGemInstances->Add();
	pCharGemInstance1->set_id(1000);
	pCharGemInstance1->set_first(true);
	pCharGemInstance1->set_threestar(true);
	pCharGemInstance1->set_star(3);
	pCharGemInstance1->set_buildid(1771588091);

	auto* pCharGemInstance2 = pCharGemInstances->Add();
	pCharGemInstance2->set_id(2000);
	pCharGemInstance2->set_first(true);
	pCharGemInstance2->set_threestar(true);
	pCharGemInstance2->set_star(3);
	pCharGemInstance2->set_buildid(1771588091);

	auto* pCharGemInstance3 = pCharGemInstances->Add();
	pCharGemInstance3->set_id(3000);
	pCharGemInstance3->set_first(true);
	pCharGemInstance3->set_threestar(true);
	pCharGemInstance3->set_star(3);
	pCharGemInstance3->set_buildid(1771756242);

	auto* pCharGemInstance4 = pCharGemInstances->Add();
	pCharGemInstance4->set_id(3001);
	pCharGemInstance4->set_first(true);
	pCharGemInstance4->set_threestar(true);
	pCharGemInstance4->set_star(3);
	pCharGemInstance4->set_buildid(1771756242);

	auto* pCharGemInstance5 = pCharGemInstances->Add();
	pCharGemInstance5->set_id(1001);
	pCharGemInstance5->set_first(true);
	pCharGemInstance5->set_threestar(true);
	pCharGemInstance5->set_star(3);
	pCharGemInstance5->set_buildid(1771588091);

	auto* pCharGemInstance6 = pCharGemInstances->Add();
	pCharGemInstance6->set_id(3002);
	pCharGemInstance6->set_first(true);
	pCharGemInstance6->set_threestar(true);
	pCharGemInstance6->set_star(3);
	pCharGemInstance6->set_buildid(1771756242);

	auto* pCharGemInstance7 = pCharGemInstances->Add();
	pCharGemInstance7->set_id(2001);
	pCharGemInstance7->set_first(true);
	pCharGemInstance7->set_threestar(true);
	pCharGemInstance7->set_star(3);
	pCharGemInstance7->set_buildid(1771588091);

	auto* pCharGemInstance8 = pCharGemInstances->Add();
	pCharGemInstance8->set_id(1002);
	pCharGemInstance8->set_first(true);
	pCharGemInstance8->set_threestar(true);
	pCharGemInstance8->set_star(3);
	pCharGemInstance8->set_buildid(1771588091);

	auto* pCharGemInstance9 = pCharGemInstances->Add();
	pCharGemInstance9->set_id(2002);
	pCharGemInstance9->set_first(true);
	pCharGemInstance9->set_threestar(true);
	pCharGemInstance9->set_star(3);
	pCharGemInstance9->set_buildid(1771588091);

	// === Chars ===
	auto* pChars = info.mutable_chars();

	// Char 1 (Tid: 123)
	auto* pChar1 = pChars->Add();
	pChar1->set_tid(123);
	pChar1->set_level(1);
	pChar1->set_advance(0);
	pChar1->set_createtime(1760972520);
	pChar1->set_skin(12301);
	pChar1->set_affinitylevel(1);
	pChar1->set_affinityexp(0);
	pChar1->set_talentbackground(0);
	pChar1->set_talentnodes(Base64Decode("AAAAAAAAAB8="));
	// SkillLvs
	pChar1->add_skilllvs(1);
	pChar1->add_skilllvs(1);
	pChar1->add_skilllvs(1);
	pChar1->add_skilllvs(1);
	pChar1->add_skilllvs(1);
	// Plots
	pChar1->add_plots(12301);
	// CharGemSlots
	auto* pCharGemSlots1 = pChar1->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots1->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests1 = pChar1->mutable_affinityquests();
	// Quest 12301
	auto* pQuest1_1 = pAffinityQuests1->add_list();
	pQuest1_1->set_id(12301);
	pQuest1_1->set_type(Affinity);
	pQuest1_1->set_status(2);
	auto* pProgress1_1 = pQuest1_1->add_progress();
	pProgress1_1->set_cur(1);
	pProgress1_1->set_max(1);
	// Quest 12302
	auto* pQuest1_2 = pAffinityQuests1->add_list();
	pQuest1_2->set_id(12302);
	pQuest1_2->set_type(Affinity);
	auto* pProgress1_2 = pQuest1_2->add_progress();
	pProgress1_2->set_max(1);
	// Quest 12303
	auto* pQuest1_3 = pAffinityQuests1->add_list();
	pQuest1_3->set_id(12303);
	pQuest1_3->set_type(Affinity);
	auto* pProgress1_3 = pQuest1_3->add_progress();
	pProgress1_3->set_max(1);
	// Quest 12304
	auto* pQuest1_4 = pAffinityQuests1->add_list();
	pQuest1_4->set_id(12304);
	pQuest1_4->set_type(Affinity);
	auto* pProgress1_4 = pQuest1_4->add_progress();
	pProgress1_4->set_max(3);
	// Quest 12305
	auto* pQuest1_5 = pAffinityQuests1->add_list();
	pQuest1_5->set_id(12305);
	pQuest1_5->set_type(Affinity);
	auto* pProgress1_5 = pQuest1_5->add_progress();
	pProgress1_5->set_max(1);
	// Quest 12306
	auto* pQuest1_6 = pAffinityQuests1->add_list();
	pQuest1_6->set_id(12306);
	pQuest1_6->set_type(Affinity);
	auto* pProgress1_6 = pQuest1_6->add_progress();
	pProgress1_6->set_max(1);
	// Quest 12307
	auto* pQuest1_7 = pAffinityQuests1->add_list();
	pQuest1_7->set_id(12307);
	pQuest1_7->set_type(Affinity);
	auto* pProgress1_7 = pQuest1_7->add_progress();
	pProgress1_7->set_max(1);
	// Quest 12308
	auto* pQuest1_8 = pAffinityQuests1->add_list();
	pQuest1_8->set_id(12308);
	pQuest1_8->set_type(Affinity);
	auto* pProgress1_8 = pQuest1_8->add_progress();
	pProgress1_8->set_max(1);
	// Quest 12309
	auto* pQuest1_9 = pAffinityQuests1->add_list();
	pQuest1_9->set_id(12309);
	pQuest1_9->set_type(Affinity);
	auto* pProgress1_9 = pQuest1_9->add_progress();
	pProgress1_9->set_cur(8);
	pProgress1_9->set_max(16);

	// Char 2 (Tid: 111)
	auto* pChar2 = pChars->Add();
	pChar2->set_tid(111);
	pChar2->set_level(1);
	pChar2->set_advance(0);
	pChar2->set_createtime(1760971340);
	pChar2->set_skin(11101);
	pChar2->set_affinitylevel(1);
	pChar2->set_affinityexp(0);
	pChar2->set_talentbackground(0);
	pChar2->set_talentnodes(Base64Decode("AAAAAAAAAAA="));
	// SkillLvs
	pChar2->add_skilllvs(1);
	pChar2->add_skilllvs(1);
	pChar2->add_skilllvs(1);
	pChar2->add_skilllvs(1);
	pChar2->add_skilllvs(1);
	// Plots
	pChar2->add_plots(11101);
	// CharGemSlots
	auto* pCharGemSlots2 = pChar2->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots2->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests2 = pChar2->mutable_affinityquests();
	// Quest 11101
	auto* pQuest2_1 = pAffinityQuests2->add_list();
	pQuest2_1->set_id(11101);
	pQuest2_1->set_type(Affinity);
	pQuest2_1->set_status(2);
	auto* pProgress2_1 = pQuest2_1->add_progress();
	pProgress2_1->set_cur(1);
	pProgress2_1->set_max(1);
	// Quest 11102
	auto* pQuest2_2 = pAffinityQuests2->add_list();
	pQuest2_2->set_id(11102);
	pQuest2_2->set_type(Affinity);
	auto* pProgress2_2 = pQuest2_2->add_progress();
	pProgress2_2->set_max(1);
	// Quest 11103
	auto* pQuest2_3 = pAffinityQuests2->add_list();
	pQuest2_3->set_id(11103);
	pQuest2_3->set_type(Affinity);
	auto* pProgress2_3 = pQuest2_3->add_progress();
	pProgress2_3->set_max(1);
	// Quest 11104
	auto* pQuest2_4 = pAffinityQuests2->add_list();
	pQuest2_4->set_id(11104);
	pQuest2_4->set_type(Affinity);
	auto* pProgress2_4 = pQuest2_4->add_progress();
	pProgress2_4->set_max(3);
	// Quest 11105
	auto* pQuest2_5 = pAffinityQuests2->add_list();
	pQuest2_5->set_id(11105);
	pQuest2_5->set_type(Affinity);
	auto* pProgress2_5 = pQuest2_5->add_progress();
	pProgress2_5->set_max(1);
	// Quest 11106
	auto* pQuest2_6 = pAffinityQuests2->add_list();
	pQuest2_6->set_id(11106);
	pQuest2_6->set_type(Affinity);
	auto* pProgress2_6 = pQuest2_6->add_progress();
	pProgress2_6->set_max(1);
	// Quest 11107
	auto* pQuest2_7 = pAffinityQuests2->add_list();
	pQuest2_7->set_id(11107);
	pQuest2_7->set_type(Affinity);
	auto* pProgress2_7 = pQuest2_7->add_progress();
	pProgress2_7->set_max(1);
	// Quest 11108
	auto* pQuest2_8 = pAffinityQuests2->add_list();
	pQuest2_8->set_id(11108);
	pQuest2_8->set_type(Affinity);
	auto* pProgress2_8 = pQuest2_8->add_progress();
	pProgress2_8->set_max(1);
	// Quest 11109
	auto* pQuest2_9 = pAffinityQuests2->add_list();
	pQuest2_9->set_id(11109);
	pQuest2_9->set_type(Affinity);
	auto* pProgress2_9 = pQuest2_9->add_progress();
	pProgress2_9->set_cur(5);
	pProgress2_9->set_max(16);

	// Char 3 (Tid: 120)
	auto* pChar3 = pChars->Add();
	pChar3->set_tid(120);
	pChar3->set_level(1);
	pChar3->set_advance(0);
	pChar3->set_createtime(1761139177);
	pChar3->set_skin(12001);
	pChar3->set_affinitylevel(0);
	pChar3->set_affinityexp(0);
	pChar3->set_talentbackground(1);
	pChar3->set_talentnodes(Base64Decode("AAAAAAAAg/8="));
	// SkillLvs
	pChar3->add_skilllvs(1);
	pChar3->add_skilllvs(1);
	pChar3->add_skilllvs(1);
	pChar3->add_skilllvs(1);
	pChar3->add_skilllvs(1);
	// CharGemSlots
	auto* pCharGemSlots3 = pChar3->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots3->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests3 = pChar3->mutable_affinityquests();
	// Quest 12001
	auto* pQuest3_1 = pAffinityQuests3->add_list();
	pQuest3_1->set_id(12001);
	pQuest3_1->set_type(Affinity);
	auto* pProgress3_1 = pQuest3_1->add_progress();
	pProgress3_1->set_max(1);
	// Quest 12002
	auto* pQuest3_2 = pAffinityQuests3->add_list();
	pQuest3_2->set_id(12002);
	pQuest3_2->set_type(Affinity);
	auto* pProgress3_2 = pQuest3_2->add_progress();
	pProgress3_2->set_max(1);
	// Quest 12003
	auto* pQuest3_3 = pAffinityQuests3->add_list();
	pQuest3_3->set_id(12003);
	pQuest3_3->set_type(Affinity);
	auto* pProgress3_3 = pQuest3_3->add_progress();
	pProgress3_3->set_max(1);
	// Quest 12004
	auto* pQuest3_4 = pAffinityQuests3->add_list();
	pQuest3_4->set_id(12004);
	pQuest3_4->set_type(Affinity);
	auto* pProgress3_4 = pQuest3_4->add_progress();
	pProgress3_4->set_max(3);
	// Quest 12005
	auto* pQuest3_5 = pAffinityQuests3->add_list();
	pQuest3_5->set_id(12005);
	pQuest3_5->set_type(Affinity);
	auto* pProgress3_5 = pQuest3_5->add_progress();
	pProgress3_5->set_max(1);
	// Quest 12006
	auto* pQuest3_6 = pAffinityQuests3->add_list();
	pQuest3_6->set_id(12006);
	pQuest3_6->set_type(Affinity);
	auto* pProgress3_6 = pQuest3_6->add_progress();
	pProgress3_6->set_max(1);
	// Quest 12007
	auto* pQuest3_7 = pAffinityQuests3->add_list();
	pQuest3_7->set_id(12007);
	pQuest3_7->set_type(Affinity);
	auto* pProgress3_7 = pQuest3_7->add_progress();
	pProgress3_7->set_max(1);
	// Quest 12008
	auto* pQuest3_8 = pAffinityQuests3->add_list();
	pQuest3_8->set_id(12008);
	pQuest3_8->set_type(Affinity);
	auto* pProgress3_8 = pQuest3_8->add_progress();
	pProgress3_8->set_max(1);
	// Quest 12009
	auto* pQuest3_9 = pAffinityQuests3->add_list();
	pQuest3_9->set_id(12009);
	pQuest3_9->set_type(Affinity);
	auto* pProgress3_9 = pQuest3_9->add_progress();
	pProgress3_9->set_max(16);

	// Char 4 (Tid: 135)
	auto* pChar4 = pChars->Add();
	pChar4->set_tid(135);
	pChar4->set_level(1);
	pChar4->set_advance(0);
	pChar4->set_createtime(1778843414);
	pChar4->set_skin(13501);
	pChar4->set_affinitylevel(0);
	pChar4->set_affinityexp(0);
	pChar4->set_talentbackground(0);
	pChar4->set_talentnodes(Base64Decode("AAAAAAAAAAA="));
	// SkillLvs
	pChar4->add_skilllvs(1);
	pChar4->add_skilllvs(1);
	pChar4->add_skilllvs(1);
	pChar4->add_skilllvs(1);
	pChar4->add_skilllvs(1);
	// CharGemSlots
	auto* pCharGemSlots4 = pChar4->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots4->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests4 = pChar4->mutable_affinityquests();
	// Quest 13501
	auto* pQuest4_1 = pAffinityQuests4->add_list();
	pQuest4_1->set_id(13501);
	pQuest4_1->set_type(Affinity);
	auto* pProgress4_1 = pQuest4_1->add_progress();
	pProgress4_1->set_max(1);
	// Quest 13502
	auto* pQuest4_2 = pAffinityQuests4->add_list();
	pQuest4_2->set_id(13502);
	pQuest4_2->set_type(Affinity);
	auto* pProgress4_2 = pQuest4_2->add_progress();
	pProgress4_2->set_max(1);
	// Quest 13503
	auto* pQuest4_3 = pAffinityQuests4->add_list();
	pQuest4_3->set_id(13503);
	pQuest4_3->set_type(Affinity);
	auto* pProgress4_3 = pQuest4_3->add_progress();
	pProgress4_3->set_max(1);
	// Quest 13504
	auto* pQuest4_4 = pAffinityQuests4->add_list();
	pQuest4_4->set_id(13504);
	pQuest4_4->set_type(Affinity);
	auto* pProgress4_4 = pQuest4_4->add_progress();
	pProgress4_4->set_max(3);
	// Quest 13505
	auto* pQuest4_5 = pAffinityQuests4->add_list();
	pQuest4_5->set_id(13505);
	pQuest4_5->set_type(Affinity);
	auto* pProgress4_5 = pQuest4_5->add_progress();
	pProgress4_5->set_max(1);
	// Quest 13506
	auto* pQuest4_6 = pAffinityQuests4->add_list();
	pQuest4_6->set_id(13506);
	pQuest4_6->set_type(Affinity);
	auto* pProgress4_6 = pQuest4_6->add_progress();
	pProgress4_6->set_max(1);
	// Quest 13507
	auto* pQuest4_7 = pAffinityQuests4->add_list();
	pQuest4_7->set_id(13507);
	pQuest4_7->set_type(Affinity);
	auto* pProgress4_7 = pQuest4_7->add_progress();
	pProgress4_7->set_max(1);
	// Quest 13508
	auto* pQuest4_8 = pAffinityQuests4->add_list();
	pQuest4_8->set_id(13508);
	pQuest4_8->set_type(Affinity);
	auto* pProgress4_8 = pQuest4_8->add_progress();
	pProgress4_8->set_max(1);
	// Quest 13509
	auto* pQuest4_9 = pAffinityQuests4->add_list();
	pQuest4_9->set_id(13509);
	pQuest4_9->set_type(Affinity);
	auto* pProgress4_9 = pQuest4_9->add_progress();
	pProgress4_9->set_max(16);

	// Char 5 (Tid: 142)
	auto* pChar5 = pChars->Add();
	pChar5->set_tid(142);
	pChar5->set_level(10);
	pChar5->set_advance(1);
	pChar5->set_createtime(1761316191);
	pChar5->set_skin(14201);
	pChar5->set_affinitylevel(0);
	pChar5->set_affinityexp(0);
	pChar5->set_talentbackground(0);
	pChar5->set_talentnodes(Base64Decode("AAAAAAAAAAA="));
	// SkillLvs
	pChar5->add_skilllvs(1);
	pChar5->add_skilllvs(1);
	pChar5->add_skilllvs(1);
	pChar5->add_skilllvs(1);
	pChar5->add_skilllvs(1);
	// CharGemSlots
	auto* pCharGemSlots5 = pChar5->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots5->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests5 = pChar5->mutable_affinityquests();
	// Quest 14201
	auto* pQuest5_1 = pAffinityQuests5->add_list();
	pQuest5_1->set_id(14201);
	pQuest5_1->set_type(Affinity);
	auto* pProgress5_1 = pQuest5_1->add_progress();
	pProgress5_1->set_max(1);
	// Quest 14202
	auto* pQuest5_2 = pAffinityQuests5->add_list();
	pQuest5_2->set_id(14202);
	pQuest5_2->set_type(Affinity);
	auto* pProgress5_2 = pQuest5_2->add_progress();
	pProgress5_2->set_max(1);
	// Quest 14203
	auto* pQuest5_3 = pAffinityQuests5->add_list();
	pQuest5_3->set_id(14203);
	pQuest5_3->set_type(Affinity);
	auto* pProgress5_3 = pQuest5_3->add_progress();
	pProgress5_3->set_max(1);
	// Quest 14204
	auto* pQuest5_4 = pAffinityQuests5->add_list();
	pQuest5_4->set_id(14204);
	pQuest5_4->set_type(Affinity);
	auto* pProgress5_4 = pQuest5_4->add_progress();
	pProgress5_4->set_max(3);
	// Quest 14205
	auto* pQuest5_5 = pAffinityQuests5->add_list();
	pQuest5_5->set_id(14205);
	pQuest5_5->set_type(Affinity);
	auto* pProgress5_5 = pQuest5_5->add_progress();
	pProgress5_5->set_max(1);
	// Quest 14206
	auto* pQuest5_6 = pAffinityQuests5->add_list();
	pQuest5_6->set_id(14206);
	pQuest5_6->set_type(Affinity);
	auto* pProgress5_6 = pQuest5_6->add_progress();
	pProgress5_6->set_max(1);
	// Quest 14207
	auto* pQuest5_7 = pAffinityQuests5->add_list();
	pQuest5_7->set_id(14207);
	pQuest5_7->set_type(Affinity);
	auto* pProgress5_7 = pQuest5_7->add_progress();
	pProgress5_7->set_max(1);
	// Quest 14208
	auto* pQuest5_8 = pAffinityQuests5->add_list();
	pQuest5_8->set_id(14208);
	pQuest5_8->set_type(Affinity);
	auto* pProgress5_8 = pQuest5_8->add_progress();
	pProgress5_8->set_max(1);
	// Quest 14209
	auto* pQuest5_9 = pAffinityQuests5->add_list();
	pQuest5_9->set_id(14209);
	pQuest5_9->set_type(Affinity);
	auto* pProgress5_9 = pQuest5_9->add_progress();
	pProgress5_9->set_max(16);

	// Char 6 (Tid: 107)
	auto* pChar6 = pChars->Add();
	pChar6->set_tid(107);
	pChar6->set_level(60);
	pChar6->set_advance(5);
	pChar6->set_createtime(1760972543);
	pChar6->set_skin(10702);
	pChar6->set_affinitylevel(2);
	pChar6->set_affinityexp(100);
	pChar6->set_talentbackground(2);
	pChar6->set_talentnodes(Base64Decode("AAAAAIP/g/8="));
	// SkillLvs
	pChar6->add_skilllvs(2);
	pChar6->add_skilllvs(2);
	pChar6->add_skilllvs(3);
	pChar6->add_skilllvs(1);
	pChar6->add_skilllvs(1);
	// Plots
	pChar6->add_plots(10701);
	// CharGemSlots
	auto* pCharGemSlots6 = pChar6->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots6->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests6 = pChar6->mutable_affinityquests();
	// Quest 10701
	auto* pQuest6_1 = pAffinityQuests6->add_list();
	pQuest6_1->set_id(10701);
	pQuest6_1->set_type(Affinity);
	pQuest6_1->set_status(2);
	auto* pProgress6_1 = pQuest6_1->add_progress();
	pProgress6_1->set_cur(1);
	pProgress6_1->set_max(1);
	// Quest 10705
	auto* pQuest6_2 = pAffinityQuests6->add_list();
	pQuest6_2->set_id(10705);
	pQuest6_2->set_type(Affinity);
	pQuest6_2->set_status(2);
	auto* pProgress6_2 = pQuest6_2->add_progress();
	pProgress6_2->set_cur(1);
	pProgress6_2->set_max(1);
	// Quest 10703
	auto* pQuest6_3 = pAffinityQuests6->add_list();
	pQuest6_3->set_id(10703);
	pQuest6_3->set_type(Affinity);
	pQuest6_3->set_status(2);
	auto* pProgress6_3 = pQuest6_3->add_progress();
	pProgress6_3->set_cur(1);
	pProgress6_3->set_max(1);
	// Quest 10706
	auto* pQuest6_4 = pAffinityQuests6->add_list();
	pQuest6_4->set_id(10706);
	pQuest6_4->set_type(Affinity);
	pQuest6_4->set_status(2);
	auto* pProgress6_4 = pQuest6_4->add_progress();
	pProgress6_4->set_cur(1);
	pProgress6_4->set_max(1);
	// Quest 10702
	auto* pQuest6_5 = pAffinityQuests6->add_list();
	pQuest6_5->set_id(10702);
	pQuest6_5->set_type(Affinity);
	auto* pProgress6_5 = pQuest6_5->add_progress();
	pProgress6_5->set_max(1);
	// Quest 10704
	auto* pQuest6_6 = pAffinityQuests6->add_list();
	pQuest6_6->set_id(10704);
	pQuest6_6->set_type(Affinity);
	pQuest6_6->set_status(1);
	auto* pProgress6_6 = pQuest6_6->add_progress();
	pProgress6_6->set_cur(3);
	pProgress6_6->set_max(3);
	// Quest 10707
	auto* pQuest6_7 = pAffinityQuests6->add_list();
	pQuest6_7->set_id(10707);
	pQuest6_7->set_type(Affinity);
	auto* pProgress6_7 = pQuest6_7->add_progress();
	pProgress6_7->set_max(1);
	// Quest 10708
	auto* pQuest6_8 = pAffinityQuests6->add_list();
	pQuest6_8->set_id(10708);
	pQuest6_8->set_type(Affinity);
	auto* pProgress6_8 = pQuest6_8->add_progress();
	pProgress6_8->set_max(1);
	// Quest 10709
	auto* pQuest6_9 = pAffinityQuests6->add_list();
	pQuest6_9->set_id(10709);
	pQuest6_9->set_type(Affinity);
	auto* pProgress6_9 = pQuest6_9->add_progress();
	pProgress6_9->set_cur(13);
	pProgress6_9->set_max(16);

	// Char 7 (Tid: 134)
	auto* pChar7 = pChars->Add();
	pChar7->set_tid(134);
	pChar7->set_level(90);
	pChar7->set_advance(8);
	pChar7->set_createtime(1764870690);
	pChar7->set_skin(13402);
	pChar7->set_affinitylevel(16);
	pChar7->set_affinityexp(400);
	pChar7->set_talentbackground(0);
	pChar7->set_talentnodes(Base64Decode("AAAAAAAAAAA="));
	// SkillLvs
	pChar7->add_skilllvs(6);
	pChar7->add_skilllvs(9);
	pChar7->add_skilllvs(6);
	pChar7->add_skilllvs(7);
	pChar7->add_skilllvs(1);
	// CharGemSlots
	auto* pCharGemSlots7 = pChar7->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots7->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests7 = pChar7->mutable_affinityquests();
	// Quest 13403
	auto* pQuest7_1 = pAffinityQuests7->add_list();
	pQuest7_1->set_id(13403);
	pQuest7_1->set_type(Affinity);
	pQuest7_1->set_status(2);
	auto* pProgress7_1 = pQuest7_1->add_progress();
	pProgress7_1->set_cur(1);
	pProgress7_1->set_max(1);
	// Quest 13406
	auto* pQuest7_2 = pAffinityQuests7->add_list();
	pQuest7_2->set_id(13406);
	pQuest7_2->set_type(Affinity);
	pQuest7_2->set_status(2);
	auto* pProgress7_2 = pQuest7_2->add_progress();
	pProgress7_2->set_cur(1);
	pProgress7_2->set_max(1);
	// Quest 13404
	auto* pQuest7_3 = pAffinityQuests7->add_list();
	pQuest7_3->set_id(13404);
	pQuest7_3->set_type(Affinity);
	pQuest7_3->set_status(2);
	auto* pProgress7_3 = pQuest7_3->add_progress();
	pProgress7_3->set_cur(1);
	pProgress7_3->set_max(1);
	// Quest 13401
	auto* pQuest7_4 = pAffinityQuests7->add_list();
	pQuest7_4->set_id(13401);
	pQuest7_4->set_type(Affinity);
	pQuest7_4->set_status(2);
	auto* pProgress7_4 = pQuest7_4->add_progress();
	pProgress7_4->set_cur(1);
	pProgress7_4->set_max(1);
	// Quest 13407
	auto* pQuest7_5 = pAffinityQuests7->add_list();
	pQuest7_5->set_id(13407);
	pQuest7_5->set_type(Affinity);
	pQuest7_5->set_status(2);
	auto* pProgress7_5 = pQuest7_5->add_progress();
	pProgress7_5->set_cur(1);
	pProgress7_5->set_max(1);
	// Quest 13405
	auto* pQuest7_6 = pAffinityQuests7->add_list();
	pQuest7_6->set_id(13405);
	pQuest7_6->set_type(Affinity);
	pQuest7_6->set_status(2);
	auto* pProgress7_6 = pQuest7_6->add_progress();
	pProgress7_6->set_cur(1);
	pProgress7_6->set_max(1);
	// Quest 13409
	auto* pQuest7_7 = pAffinityQuests7->add_list();
	pQuest7_7->set_id(13409);
	pQuest7_7->set_type(Affinity);
	pQuest7_7->set_status(2);
	auto* pProgress7_7 = pQuest7_7->add_progress();
	pProgress7_7->set_cur(1);
	pProgress7_7->set_max(1);
	// Quest 13402
	auto* pQuest7_8 = pAffinityQuests7->add_list();
	pQuest7_8->set_id(13402);
	pQuest7_8->set_type(Affinity);
	auto* pProgress7_8 = pQuest7_8->add_progress();
	pProgress7_8->set_max(1);
	// Quest 13408
	auto* pQuest7_9 = pAffinityQuests7->add_list();
	pQuest7_9->set_id(13408);
	pQuest7_9->set_type(Affinity);
	auto* pProgress7_9 = pQuest7_9->add_progress();
	pProgress7_9->set_max(1);

	// Char 8 (Tid: 147)
	auto* pChar8 = pChars->Add();
	pChar8->set_tid(147);
	pChar8->set_level(1);
	pChar8->set_advance(0);
	pChar8->set_createtime(1761316191);
	pChar8->set_skin(14701);
	pChar8->set_affinitylevel(0);
	pChar8->set_affinityexp(0);
	pChar8->set_talentbackground(0);
	pChar8->set_talentnodes(Base64Decode("AAAAAAAAAAA="));
	// SkillLvs
	pChar8->add_skilllvs(1);
	pChar8->add_skilllvs(1);
	pChar8->add_skilllvs(1);
	pChar8->add_skilllvs(1);
	pChar8->add_skilllvs(1);
	// CharGemSlots
	auto* pCharGemSlots8 = pChar8->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots8->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests8 = pChar8->mutable_affinityquests();
	// Quest 14701
	auto* pQuest8_1 = pAffinityQuests8->add_list();
	pQuest8_1->set_id(14701);
	pQuest8_1->set_type(Affinity);
	auto* pProgress8_1 = pQuest8_1->add_progress();
	pProgress8_1->set_max(1);
	// Quest 14702
	auto* pQuest8_2 = pAffinityQuests8->add_list();
	pQuest8_2->set_id(14702);
	pQuest8_2->set_type(Affinity);
	auto* pProgress8_2 = pQuest8_2->add_progress();
	pProgress8_2->set_max(1);
	// Quest 14703
	auto* pQuest8_3 = pAffinityQuests8->add_list();
	pQuest8_3->set_id(14703);
	pQuest8_3->set_type(Affinity);
	auto* pProgress8_3 = pQuest8_3->add_progress();
	pProgress8_3->set_max(1);
	// Quest 14704
	auto* pQuest8_4 = pAffinityQuests8->add_list();
	pQuest8_4->set_id(14704);
	pQuest8_4->set_type(Affinity);
	auto* pProgress8_4 = pQuest8_4->add_progress();
	pProgress8_4->set_max(3);
	// Quest 14705
	auto* pQuest8_5 = pAffinityQuests8->add_list();
	pQuest8_5->set_id(14705);
	pQuest8_5->set_type(Affinity);
	auto* pProgress8_5 = pQuest8_5->add_progress();
	pProgress8_5->set_max(1);
	// Quest 14706
	auto* pQuest8_6 = pAffinityQuests8->add_list();
	pQuest8_6->set_id(14706);
	pQuest8_6->set_type(Affinity);
	auto* pProgress8_6 = pQuest8_6->add_progress();
	pProgress8_6->set_max(1);
	// Quest 14707
	auto* pQuest8_7 = pAffinityQuests8->add_list();
	pQuest8_7->set_id(14707);
	pQuest8_7->set_type(Affinity);
	auto* pProgress8_7 = pQuest8_7->add_progress();
	pProgress8_7->set_max(1);
	// Quest 14708
	auto* pQuest8_8 = pAffinityQuests8->add_list();
	pQuest8_8->set_id(14708);
	pQuest8_8->set_type(Affinity);
	auto* pProgress8_8 = pQuest8_8->add_progress();
	pProgress8_8->set_max(1);
	// Quest 14709
	auto* pQuest8_9 = pAffinityQuests8->add_list();
	pQuest8_9->set_id(14709);
	pQuest8_9->set_type(Affinity);
	auto* pProgress8_9 = pQuest8_9->add_progress();
	pProgress8_9->set_max(16);

	// Char 9 (Tid: 158)
	auto* pChar9 = pChars->Add();
	pChar9->set_tid(158);
	pChar9->set_level(90);
	pChar9->set_advance(8);
	pChar9->set_createtime(1766589587);
	pChar9->set_skin(15802);
	pChar9->set_affinitylevel(3);
	pChar9->set_affinityexp(50);
	pChar9->set_talentbackground(1);
	pChar9->set_talentnodes(Base64Decode("AAAAAAAAg/8="));
	// SkillLvs
	pChar9->add_skilllvs(8);
	pChar9->add_skilllvs(6);
	pChar9->add_skilllvs(6);
	pChar9->add_skilllvs(4);
	pChar9->add_skilllvs(1);
	// CharGemSlots
	auto* pCharGemSlots9 = pChar9->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots9->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests9 = pChar9->mutable_affinityquests();
	// Quest 15803
	auto* pQuest9_1 = pAffinityQuests9->add_list();
	pQuest9_1->set_id(15803);
	pQuest9_1->set_type(Affinity);
	pQuest9_1->set_status(2);
	auto* pProgress9_1 = pQuest9_1->add_progress();
	pProgress9_1->set_cur(1);
	pProgress9_1->set_max(1);
	// Quest 15806
	auto* pQuest9_2 = pAffinityQuests9->add_list();
	pQuest9_2->set_id(15806);
	pQuest9_2->set_type(Affinity);
	pQuest9_2->set_status(2);
	auto* pProgress9_2 = pQuest9_2->add_progress();
	pProgress9_2->set_cur(1);
	pProgress9_2->set_max(1);
	// Quest 15805
	auto* pQuest9_3 = pAffinityQuests9->add_list();
	pQuest9_3->set_id(15805);
	pQuest9_3->set_type(Affinity);
	pQuest9_3->set_status(2);
	auto* pProgress9_3 = pQuest9_3->add_progress();
	pProgress9_3->set_cur(1);
	pProgress9_3->set_max(1);
	// Quest 15801
	auto* pQuest9_4 = pAffinityQuests9->add_list();
	pQuest9_4->set_id(15801);
	pQuest9_4->set_type(Affinity);
	pQuest9_4->set_status(2);
	auto* pProgress9_4 = pQuest9_4->add_progress();
	pProgress9_4->set_cur(1);
	pProgress9_4->set_max(1);
	// Quest 15804
	auto* pQuest9_5 = pAffinityQuests9->add_list();
	pQuest9_5->set_id(15804);
	pQuest9_5->set_type(Affinity);
	pQuest9_5->set_status(2);
	auto* pProgress9_5 = pQuest9_5->add_progress();
	pProgress9_5->set_cur(1);
	pProgress9_5->set_max(1);
	// Quest 15807
	auto* pQuest9_6 = pAffinityQuests9->add_list();
	pQuest9_6->set_id(15807);
	pQuest9_6->set_type(Affinity);
	pQuest9_6->set_status(2);
	auto* pProgress9_6 = pQuest9_6->add_progress();
	pProgress9_6->set_cur(1);
	pProgress9_6->set_max(1);
	// Quest 15802
	auto* pQuest9_7 = pAffinityQuests9->add_list();
	pQuest9_7->set_id(15802);
	pQuest9_7->set_type(Affinity);
	auto* pProgress9_7 = pQuest9_7->add_progress();
	pProgress9_7->set_max(1);
	// Quest 15808
	auto* pQuest9_8 = pAffinityQuests9->add_list();
	pQuest9_8->set_id(15808);
	pQuest9_8->set_type(Affinity);
	auto* pProgress9_8 = pQuest9_8->add_progress();
	pProgress9_8->set_max(1);
	// Quest 15809
	auto* pQuest9_9 = pAffinityQuests9->add_list();
	pQuest9_9->set_id(15809);
	pQuest9_9->set_type(Affinity);
	auto* pProgress9_9 = pQuest9_9->add_progress();
	pProgress9_9->set_cur(14);
	pProgress9_9->set_max(16);

	// Char 10 (Tid: 117)
	auto* pChar10 = pChars->Add();
	pChar10->set_tid(117);
	pChar10->set_level(60);
	pChar10->set_advance(6);
	pChar10->set_createtime(1760972186);
	pChar10->set_skin(11702);
	pChar10->set_affinitylevel(3);
	pChar10->set_affinityexp(300);
	pChar10->set_talentbackground(2);
	pChar10->set_talentnodes(Base64Decode("AAAAAIP/g/8="));
	// SkillLvs
	pChar10->add_skilllvs(2);
	pChar10->add_skilllvs(2);
	pChar10->add_skilllvs(1);
	pChar10->add_skilllvs(1);
	pChar10->add_skilllvs(1);
	// Plots
	pChar10->add_plots(11701);
	// CharGemSlots
	auto* pCharGemSlots10 = pChar10->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots10->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests10 = pChar10->mutable_affinityquests();
	// Quest 11705
	auto* pQuest10_1 = pAffinityQuests10->add_list();
	pQuest10_1->set_id(11705);
	pQuest10_1->set_type(Affinity);
	pQuest10_1->set_status(2);
	auto* pProgress10_1 = pQuest10_1->add_progress();
	pProgress10_1->set_cur(1);
	pProgress10_1->set_max(1);
	// Quest 11701
	auto* pQuest10_2 = pAffinityQuests10->add_list();
	pQuest10_2->set_id(11701);
	pQuest10_2->set_type(Affinity);
	pQuest10_2->set_status(2);
	auto* pProgress10_2 = pQuest10_2->add_progress();
	pProgress10_2->set_cur(1);
	pProgress10_2->set_max(1);
	// Quest 11703
	auto* pQuest10_3 = pAffinityQuests10->add_list();
	pQuest10_3->set_id(11703);
	pQuest10_3->set_type(Affinity);
	pQuest10_3->set_status(2);
	auto* pProgress10_3 = pQuest10_3->add_progress();
	pProgress10_3->set_cur(1);
	pProgress10_3->set_max(1);
	// Quest 11709
	auto* pQuest10_4 = pAffinityQuests10->add_list();
	pQuest10_4->set_id(11709);
	pQuest10_4->set_type(Affinity);
	pQuest10_4->set_status(2);
	auto* pProgress10_4 = pQuest10_4->add_progress();
	pProgress10_4->set_cur(1);
	pProgress10_4->set_max(1);
	// Quest 11706
	auto* pQuest10_5 = pAffinityQuests10->add_list();
	pQuest10_5->set_id(11706);
	pQuest10_5->set_type(Affinity);
	pQuest10_5->set_status(2);
	auto* pProgress10_5 = pQuest10_5->add_progress();
	pProgress10_5->set_cur(1);
	pProgress10_5->set_max(1);
	// Quest 11702
	auto* pQuest10_6 = pAffinityQuests10->add_list();
	pQuest10_6->set_id(11702);
	pQuest10_6->set_type(Affinity);
	auto* pProgress10_6 = pQuest10_6->add_progress();
	pProgress10_6->set_max(1);
	// Quest 11704
	auto* pQuest10_7 = pAffinityQuests10->add_list();
	pQuest10_7->set_id(11704);
	pQuest10_7->set_type(Affinity);
	auto* pProgress10_7 = pQuest10_7->add_progress();
	pProgress10_7->set_cur(2);
	pProgress10_7->set_max(3);
	// Quest 11707
	auto* pQuest10_8 = pAffinityQuests10->add_list();
	pQuest10_8->set_id(11707);
	pQuest10_8->set_type(Affinity);
	auto* pProgress10_8 = pQuest10_8->add_progress();
	pProgress10_8->set_max(1);
	// Quest 11708
	auto* pQuest10_9 = pAffinityQuests10->add_list();
	pQuest10_9->set_id(11708);
	pQuest10_9->set_type(Affinity);
	auto* pProgress10_9 = pQuest10_9->add_progress();
	pProgress10_9->set_max(1);

	// Char 11 (Tid: 116)
	auto* pChar11 = pChars->Add();
	pChar11->set_tid(116);
	pChar11->set_level(1);
	pChar11->set_advance(0);
	pChar11->set_createtime(1761316259);
	pChar11->set_skin(11601);
	pChar11->set_affinitylevel(0);
	pChar11->set_affinityexp(0);
	pChar11->set_talentbackground(0);
	pChar11->set_talentnodes(Base64Decode("AAAAAAAAAAA="));
	// SkillLvs
	pChar11->add_skilllvs(1);
	pChar11->add_skilllvs(1);
	pChar11->add_skilllvs(1);
	pChar11->add_skilllvs(1);
	pChar11->add_skilllvs(1);
	// CharGemSlots
	auto* pCharGemSlots11 = pChar11->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots11->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests11 = pChar11->mutable_affinityquests();
	// Quest 11601
	auto* pQuest11_1 = pAffinityQuests11->add_list();
	pQuest11_1->set_id(11601);
	pQuest11_1->set_type(Affinity);
	auto* pProgress11_1 = pQuest11_1->add_progress();
	pProgress11_1->set_max(1);
	// Quest 11602
	auto* pQuest11_2 = pAffinityQuests11->add_list();
	pQuest11_2->set_id(11602);
	pQuest11_2->set_type(Affinity);
	auto* pProgress11_2 = pQuest11_2->add_progress();
	pProgress11_2->set_max(1);
	// Quest 11603
	auto* pQuest11_3 = pAffinityQuests11->add_list();
	pQuest11_3->set_id(11603);
	pQuest11_3->set_type(Affinity);
	auto* pProgress11_3 = pQuest11_3->add_progress();
	pProgress11_3->set_max(1);
	// Quest 11604
	auto* pQuest11_4 = pAffinityQuests11->add_list();
	pQuest11_4->set_id(11604);
	pQuest11_4->set_type(Affinity);
	auto* pProgress11_4 = pQuest11_4->add_progress();
	pProgress11_4->set_max(3);
	// Quest 11605
	auto* pQuest11_5 = pAffinityQuests11->add_list();
	pQuest11_5->set_id(11605);
	pQuest11_5->set_type(Affinity);
	auto* pProgress11_5 = pQuest11_5->add_progress();
	pProgress11_5->set_max(1);
	// Quest 11606
	auto* pQuest11_6 = pAffinityQuests11->add_list();
	pQuest11_6->set_id(11606);
	pQuest11_6->set_type(Affinity);
	auto* pProgress11_6 = pQuest11_6->add_progress();
	pProgress11_6->set_max(1);
	// Quest 11607
	auto* pQuest11_7 = pAffinityQuests11->add_list();
	pQuest11_7->set_id(11607);
	pQuest11_7->set_type(Affinity);
	auto* pProgress11_7 = pQuest11_7->add_progress();
	pProgress11_7->set_max(1);
	// Quest 11608
	auto* pQuest11_8 = pAffinityQuests11->add_list();
	pQuest11_8->set_id(11608);
	pQuest11_8->set_type(Affinity);
	auto* pProgress11_8 = pQuest11_8->add_progress();
	pProgress11_8->set_max(1);
	// Quest 11609
	auto* pQuest11_9 = pAffinityQuests11->add_list();
	pQuest11_9->set_id(11609);
	pQuest11_9->set_type(Affinity);
	auto* pProgress11_9 = pQuest11_9->add_progress();
	pProgress11_9->set_max(16);

	// Char 12 (Tid: 103)
	auto* pChar12 = pChars->Add();
	pChar12->set_tid(103);
	pChar12->set_level(70);
	pChar12->set_advance(7);
	pChar12->set_createtime(1760971340);
	pChar12->set_exp(80000);
	pChar12->set_skin(10302);
	pChar12->set_affinitylevel(15);
	pChar12->set_affinityexp(3150);
	pChar12->set_talentbackground(0);
	pChar12->set_talentnodes(Base64Decode("AAAAAAAAAB8="));
	// SkillLvs
	pChar12->add_skilllvs(6);
	pChar12->add_skilllvs(6);
	pChar12->add_skilllvs(4);
	pChar12->add_skilllvs(3);
	pChar12->add_skilllvs(1);
	// Plots
	pChar12->add_plots(10301);
	pChar12->add_plots(10302);
	pChar12->add_plots(10303);
	// CharGemSlots
	auto* pCharGemSlots12 = pChar12->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots12->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests12 = pChar12->mutable_affinityquests();
	// Quest 10301
	auto* pQuest12_1 = pAffinityQuests12->add_list();
	pQuest12_1->set_id(10301);
	pQuest12_1->set_type(Affinity);
	pQuest12_1->set_status(2);
	auto* pProgress12_1 = pQuest12_1->add_progress();
	pProgress12_1->set_cur(1);
	pProgress12_1->set_max(1);
	// Quest 10305
	auto* pQuest12_2 = pAffinityQuests12->add_list();
	pQuest12_2->set_id(10305);
	pQuest12_2->set_type(Affinity);
	pQuest12_2->set_status(2);
	auto* pProgress12_2 = pQuest12_2->add_progress();
	pProgress12_2->set_cur(1);
	pProgress12_2->set_max(1);
	// Quest 10303
	auto* pQuest12_3 = pAffinityQuests12->add_list();
	pQuest12_3->set_id(10303);
	pQuest12_3->set_type(Affinity);
	pQuest12_3->set_status(2);
	auto* pProgress12_3 = pQuest12_3->add_progress();
	pProgress12_3->set_cur(1);
	pProgress12_3->set_max(1);
	// Quest 10306
	auto* pQuest12_4 = pAffinityQuests12->add_list();
	pQuest12_4->set_id(10306);
	pQuest12_4->set_type(Affinity);
	pQuest12_4->set_status(2);
	auto* pProgress12_4 = pQuest12_4->add_progress();
	pProgress12_4->set_cur(1);
	pProgress12_4->set_max(1);
	// Quest 10304
	auto* pQuest12_5 = pAffinityQuests12->add_list();
	pQuest12_5->set_id(10304);
	pQuest12_5->set_type(Affinity);
	pQuest12_5->set_status(2);
	auto* pProgress12_5 = pQuest12_5->add_progress();
	pProgress12_5->set_cur(1);
	pProgress12_5->set_max(1);
	// Quest 10302
	auto* pQuest12_6 = pAffinityQuests12->add_list();
	pQuest12_6->set_id(10302);
	pQuest12_6->set_type(Affinity);
	auto* pProgress12_6 = pQuest12_6->add_progress();
	pProgress12_6->set_max(1);
	// Quest 10307
	auto* pQuest12_7 = pAffinityQuests12->add_list();
	pQuest12_7->set_id(10307);
	pQuest12_7->set_type(Affinity);
	auto* pProgress12_7 = pQuest12_7->add_progress();
	pProgress12_7->set_max(1);
	// Quest 10308
	auto* pQuest12_8 = pAffinityQuests12->add_list();
	pQuest12_8->set_id(10308);
	pQuest12_8->set_type(Affinity);
	auto* pProgress12_8 = pQuest12_8->add_progress();
	pProgress12_8->set_max(1);
	// Quest 10309
	auto* pQuest12_9 = pAffinityQuests12->add_list();
	pQuest12_9->set_id(10309);
	pQuest12_9->set_type(Affinity);
	auto* pProgress12_9 = pQuest12_9->add_progress();
	pProgress12_9->set_cur(15);
	pProgress12_9->set_max(16);

	// Char 13 (Tid: 127)
	auto* pChar13 = pChars->Add();
	pChar13->set_tid(127);
	pChar13->set_level(1);
	pChar13->set_advance(0);
	pChar13->set_createtime(1761575657);
	pChar13->set_skin(12701);
	pChar13->set_affinitylevel(2);
	pChar13->set_affinityexp(200);
	pChar13->set_talentbackground(0);
	pChar13->set_talentnodes(Base64Decode("AAAAAAAAAAA="));
	// SkillLvs
	pChar13->add_skilllvs(1);
	pChar13->add_skilllvs(1);
	pChar13->add_skilllvs(1);
	pChar13->add_skilllvs(1);
	pChar13->add_skilllvs(1);
	// CharGemSlots
	auto* pCharGemSlots13 = pChar13->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots13->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests13 = pChar13->mutable_affinityquests();
	// Quest 12701
	auto* pQuest13_1 = pAffinityQuests13->add_list();
	pQuest13_1->set_id(12701);
	pQuest13_1->set_type(Affinity);
	auto* pProgress13_1 = pQuest13_1->add_progress();
	pProgress13_1->set_max(1);
	// Quest 12702
	auto* pQuest13_2 = pAffinityQuests13->add_list();
	pQuest13_2->set_id(12702);
	pQuest13_2->set_type(Affinity);
	auto* pProgress13_2 = pQuest13_2->add_progress();
	pProgress13_2->set_max(1);
	// Quest 12703
	auto* pQuest13_3 = pAffinityQuests13->add_list();
	pQuest13_3->set_id(12703);
	pQuest13_3->set_type(Affinity);
	auto* pProgress13_3 = pQuest13_3->add_progress();
	pProgress13_3->set_max(1);
	// Quest 12704
	auto* pQuest13_4 = pAffinityQuests13->add_list();
	pQuest13_4->set_id(12704);
	pQuest13_4->set_type(Affinity);
	auto* pProgress13_4 = pQuest13_4->add_progress();
	pProgress13_4->set_max(3);
	// Quest 12705
	auto* pQuest13_5 = pAffinityQuests13->add_list();
	pQuest13_5->set_id(12705);
	pQuest13_5->set_type(Affinity);
	auto* pProgress13_5 = pQuest13_5->add_progress();
	pProgress13_5->set_max(1);
	// Quest 12706
	auto* pQuest13_6 = pAffinityQuests13->add_list();
	pQuest13_6->set_id(12706);
	pQuest13_6->set_type(Affinity);
	auto* pProgress13_6 = pQuest13_6->add_progress();
	pProgress13_6->set_max(1);
	// Quest 12707
	auto* pQuest13_7 = pAffinityQuests13->add_list();
	pQuest13_7->set_id(12707);
	pQuest13_7->set_type(Affinity);
	auto* pProgress13_7 = pQuest13_7->add_progress();
	pProgress13_7->set_max(1);
	// Quest 12708
	auto* pQuest13_8 = pAffinityQuests13->add_list();
	pQuest13_8->set_id(12708);
	pQuest13_8->set_type(Affinity);
	auto* pProgress13_8 = pQuest13_8->add_progress();
	pProgress13_8->set_max(1);
	// Quest 12709
	auto* pQuest13_9 = pAffinityQuests13->add_list();
	pQuest13_9->set_id(12709);
	pQuest13_9->set_type(Affinity);
	auto* pProgress13_9 = pQuest13_9->add_progress();
	pProgress13_9->set_max(16);

	// Char 14 (Tid: 155)
	auto* pChar14 = pChars->Add();
	pChar14->set_tid(155);
	pChar14->set_level(80);
	pChar14->set_advance(7);
	pChar14->set_createtime(1762954717);
	pChar14->set_skin(15502);
	pChar14->set_affinitylevel(6);
	pChar14->set_affinityexp(650);
	pChar14->set_talentbackground(0);
	pChar14->set_talentnodes(Base64Decode("AAAAAAAAAAA="));
	// SkillLvs
	pChar14->add_skilllvs(7);
	pChar14->add_skilllvs(7);
	pChar14->add_skilllvs(7);
	pChar14->add_skilllvs(7);
	pChar14->add_skilllvs(1);
	// CharGemSlots with AlterGems
	auto* pCharGemSlots14 = pChar14->mutable_chargemslots();
	auto* pSlot14_1 = pCharGemSlots14->Add();
	pSlot14_1->set_id(1);
	auto* pAlterGems = pSlot14_1->mutable_altergems();
	auto* pAlterGem = pAlterGems->Add();
	pAlterGem->add_attributes(2102);
	pAlterGem->add_attributes(2602);
	pAlterGem->add_attributes(703);
	pAlterGem->add_attributes(3302);
	pAlterGem->add_alterattributes(0);
	pAlterGem->add_alterattributes(0);
	pAlterGem->add_alterattributes(0);
	pAlterGem->add_alterattributes(0);
	auto* pSlot14_2 = pCharGemSlots14->Add();
	pSlot14_2->set_id(2);
	auto* pSlot14_3 = pCharGemSlots14->Add();
	pSlot14_3->set_id(3);
	// CharGemPresets with SlotGem
	auto* pCharGemPresets14 = pChar14->mutable_chargempresets();
	for (int i = 0; i < 3; i++) {
		auto* pPreset = pCharGemPresets14->add_chargempresets();
		if (i == 0) {
			pPreset->add_slotgem(0);
			pPreset->add_slotgem(-1);
			pPreset->add_slotgem(-1);
		}
		else {
			pPreset->add_slotgem(-1);
			pPreset->add_slotgem(-1);
			pPreset->add_slotgem(-1);
		}
	}
	// AffinityQuests
	auto* pAffinityQuests14 = pChar14->mutable_affinityquests();
	// Quest 15503
	auto* pQuest14_1 = pAffinityQuests14->add_list();
	pQuest14_1->set_id(15503);
	pQuest14_1->set_type(Affinity);
	pQuest14_1->set_status(2);
	auto* pProgress14_1 = pQuest14_1->add_progress();
	pProgress14_1->set_cur(1);
	pProgress14_1->set_max(1);
	// Quest 15506
	auto* pQuest14_2 = pAffinityQuests14->add_list();
	pQuest14_2->set_id(15506);
	pQuest14_2->set_type(Affinity);
	pQuest14_2->set_status(2);
	auto* pProgress14_2 = pQuest14_2->add_progress();
	pProgress14_2->set_cur(1);
	pProgress14_2->set_max(1);
	// Quest 15504
	auto* pQuest14_3 = pAffinityQuests14->add_list();
	pQuest14_3->set_id(15504);
	pQuest14_3->set_type(Affinity);
	pQuest14_3->set_status(2);
	auto* pProgress14_3 = pQuest14_3->add_progress();
	pProgress14_3->set_cur(1);
	pProgress14_3->set_max(1);
	// Quest 15507
	auto* pQuest14_4 = pAffinityQuests14->add_list();
	pQuest14_4->set_id(15507);
	pQuest14_4->set_type(Affinity);
	pQuest14_4->set_status(2);
	auto* pProgress14_4 = pQuest14_4->add_progress();
	pProgress14_4->set_cur(1);
	pProgress14_4->set_max(1);
	// Quest 15505
	auto* pQuest14_5 = pAffinityQuests14->add_list();
	pQuest14_5->set_id(15505);
	pQuest14_5->set_type(Affinity);
	pQuest14_5->set_status(2);
	auto* pProgress14_5 = pQuest14_5->add_progress();
	pProgress14_5->set_cur(1);
	pProgress14_5->set_max(1);
	// Quest 15501
	auto* pQuest14_6 = pAffinityQuests14->add_list();
	pQuest14_6->set_id(15501);
	pQuest14_6->set_type(Affinity);
	pQuest14_6->set_status(2);
	auto* pProgress14_6 = pQuest14_6->add_progress();
	pProgress14_6->set_cur(1);
	pProgress14_6->set_max(1);
	// Quest 15502
	auto* pQuest14_7 = pAffinityQuests14->add_list();
	pQuest14_7->set_id(15502);
	pQuest14_7->set_type(Affinity);
	auto* pProgress14_7 = pQuest14_7->add_progress();
	pProgress14_7->set_max(1);
	// Quest 15508
	auto* pQuest14_8 = pAffinityQuests14->add_list();
	pQuest14_8->set_id(15508);
	pQuest14_8->set_type(Affinity);
	auto* pProgress14_8 = pQuest14_8->add_progress();
	pProgress14_8->set_max(1);
	// Quest 15509
	auto* pQuest14_9 = pAffinityQuests14->add_list();
	pQuest14_9->set_id(15509);
	pQuest14_9->set_type(Affinity);
	auto* pProgress14_9 = pQuest14_9->add_progress();
	pProgress14_9->set_cur(10);
	pProgress14_9->set_max(16);

	// Char 15 (Tid: 115)
	auto* pChar15 = pChars->Add();
	pChar15->set_tid(115);
	pChar15->set_level(60);
	pChar15->set_advance(5);
	pChar15->set_createtime(1778844193);
	pChar15->set_skin(11502);
	pChar15->set_affinitylevel(0);
	pChar15->set_affinityexp(0);
	pChar15->set_talentbackground(0);
	pChar15->set_talentnodes(Base64Decode("AAAAAAAAAAA="));
	// SkillLvs
	pChar15->add_skilllvs(2);
	pChar15->add_skilllvs(6);
	pChar15->add_skilllvs(4);
	pChar15->add_skilllvs(4);
	pChar15->add_skilllvs(1);
	// CharGemSlots
	auto* pCharGemSlots15 = pChar15->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots15->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests15 = pChar15->mutable_affinityquests();
	// Quest 11501
	auto* pQuest15_1 = pAffinityQuests15->add_list();
	pQuest15_1->set_id(11501);
	pQuest15_1->set_type(Affinity);
	auto* pProgress15_1 = pQuest15_1->add_progress();
	pProgress15_1->set_max(1);
	// Quest 11502
	auto* pQuest15_2 = pAffinityQuests15->add_list();
	pQuest15_2->set_id(11502);
	pQuest15_2->set_type(Affinity);
	auto* pProgress15_2 = pQuest15_2->add_progress();
	pProgress15_2->set_max(1);
	// Quest 11503
	auto* pQuest15_3 = pAffinityQuests15->add_list();
	pQuest15_3->set_id(11503);
	pQuest15_3->set_type(Affinity);
	pQuest15_3->set_status(1);
	auto* pProgress15_3 = pQuest15_3->add_progress();
	pProgress15_3->set_cur(1);
	pProgress15_3->set_max(1);
	// Quest 11504
	auto* pQuest15_4 = pAffinityQuests15->add_list();
	pQuest15_4->set_id(11504);
	pQuest15_4->set_type(Affinity);
	pQuest15_4->set_status(1);
	auto* pProgress15_4 = pQuest15_4->add_progress();
	pProgress15_4->set_cur(3);
	pProgress15_4->set_max(3);
	// Quest 11505
	auto* pQuest15_5 = pAffinityQuests15->add_list();
	pQuest15_5->set_id(11505);
	pQuest15_5->set_type(Affinity);
	auto* pProgress15_5 = pQuest15_5->add_progress();
	pProgress15_5->set_max(1);
	// Quest 11506
	auto* pQuest15_6 = pAffinityQuests15->add_list();
	pQuest15_6->set_id(11506);
	pQuest15_6->set_type(Affinity);
	pQuest15_6->set_status(1);
	auto* pProgress15_6 = pQuest15_6->add_progress();
	pProgress15_6->set_cur(1);
	pProgress15_6->set_max(1);
	// Quest 11507
	auto* pQuest15_7 = pAffinityQuests15->add_list();
	pQuest15_7->set_id(11507);
	pQuest15_7->set_type(Affinity);
	auto* pProgress15_7 = pQuest15_7->add_progress();
	pProgress15_7->set_max(1);
	// Quest 11508
	auto* pQuest15_8 = pAffinityQuests15->add_list();
	pQuest15_8->set_id(11508);
	pQuest15_8->set_type(Affinity);
	auto* pProgress15_8 = pQuest15_8->add_progress();
	pProgress15_8->set_max(1);
	// Quest 11509
	auto* pQuest15_9 = pAffinityQuests15->add_list();
	pQuest15_9->set_id(11509);
	pQuest15_9->set_type(Affinity);
	auto* pProgress15_9 = pQuest15_9->add_progress();
	pProgress15_9->set_max(16);

	// Char 16 (Tid: 108)
	auto* pChar16 = pChars->Add();
	pChar16->set_tid(108);
	pChar16->set_level(1);
	pChar16->set_advance(0);
	pChar16->set_createtime(1761655367);
	pChar16->set_skin(10801);
	pChar16->set_affinitylevel(0);
	pChar16->set_affinityexp(0);
	pChar16->set_talentbackground(0);
	pChar16->set_talentnodes(Base64Decode("AAAAAAAAAB8="));
	// SkillLvs
	pChar16->add_skilllvs(1);
	pChar16->add_skilllvs(1);
	pChar16->add_skilllvs(1);
	pChar16->add_skilllvs(1);
	pChar16->add_skilllvs(1);
	// CharGemSlots
	auto* pCharGemSlots16 = pChar16->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots16->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests16 = pChar16->mutable_affinityquests();
	// Quest 10801
	auto* pQuest16_1 = pAffinityQuests16->add_list();
	pQuest16_1->set_id(10801);
	pQuest16_1->set_type(Affinity);
	auto* pProgress16_1 = pQuest16_1->add_progress();
	pProgress16_1->set_max(1);
	// Quest 10802
	auto* pQuest16_2 = pAffinityQuests16->add_list();
	pQuest16_2->set_id(10802);
	pQuest16_2->set_type(Affinity);
	auto* pProgress16_2 = pQuest16_2->add_progress();
	pProgress16_2->set_max(1);
	// Quest 10803
	auto* pQuest16_3 = pAffinityQuests16->add_list();
	pQuest16_3->set_id(10803);
	pQuest16_3->set_type(Affinity);
	auto* pProgress16_3 = pQuest16_3->add_progress();
	pProgress16_3->set_max(1);
	// Quest 10804
	auto* pQuest16_4 = pAffinityQuests16->add_list();
	pQuest16_4->set_id(10804);
	pQuest16_4->set_type(Affinity);
	auto* pProgress16_4 = pQuest16_4->add_progress();
	pProgress16_4->set_max(3);
	// Quest 10805
	auto* pQuest16_5 = pAffinityQuests16->add_list();
	pQuest16_5->set_id(10805);
	pQuest16_5->set_type(Affinity);
	auto* pProgress16_5 = pQuest16_5->add_progress();
	pProgress16_5->set_max(1);
	// Quest 10806
	auto* pQuest16_6 = pAffinityQuests16->add_list();
	pQuest16_6->set_id(10806);
	pQuest16_6->set_type(Affinity);
	auto* pProgress16_6 = pQuest16_6->add_progress();
	pProgress16_6->set_max(1);
	// Quest 10807
	auto* pQuest16_7 = pAffinityQuests16->add_list();
	pQuest16_7->set_id(10807);
	pQuest16_7->set_type(Affinity);
	auto* pProgress16_7 = pQuest16_7->add_progress();
	pProgress16_7->set_max(1);
	// Quest 10808
	auto* pQuest16_8 = pAffinityQuests16->add_list();
	pQuest16_8->set_id(10808);
	pQuest16_8->set_type(Affinity);
	auto* pProgress16_8 = pQuest16_8->add_progress();
	pProgress16_8->set_max(1);
	// Quest 10809
	auto* pQuest16_9 = pAffinityQuests16->add_list();
	pQuest16_9->set_id(10809);
	pQuest16_9->set_type(Affinity);
	auto* pProgress16_9 = pQuest16_9->add_progress();
	pProgress16_9->set_max(16);

	// Char 17 (Tid: 112)
	auto* pChar17 = pChars->Add();
	pChar17->set_tid(112);
	pChar17->set_level(50);
	pChar17->set_advance(4);
	pChar17->set_createtime(1760971340);
	pChar17->set_skin(11202);
	pChar17->set_affinitylevel(4);
	pChar17->set_affinityexp(500);
	pChar17->set_talentbackground(0);
	pChar17->set_talentnodes(Base64Decode("AAAAAAAAAAA="));
	// SkillLvs
	pChar17->add_skilllvs(3);
	pChar17->add_skilllvs(2);
	pChar17->add_skilllvs(2);
	pChar17->add_skilllvs(3);
	pChar17->add_skilllvs(1);
	// Plots
	pChar17->add_plots(11201);
	// CharGemSlots
	auto* pCharGemSlots17 = pChar17->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots17->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests17 = pChar17->mutable_affinityquests();
	// Quest 11205
	auto* pQuest17_1 = pAffinityQuests17->add_list();
	pQuest17_1->set_id(11205);
	pQuest17_1->set_type(Affinity);
	pQuest17_1->set_status(2);
	auto* pProgress17_1 = pQuest17_1->add_progress();
	pProgress17_1->set_cur(1);
	pProgress17_1->set_max(1);
	// Quest 11201
	auto* pQuest17_2 = pAffinityQuests17->add_list();
	pQuest17_2->set_id(11201);
	pQuest17_2->set_type(Affinity);
	pQuest17_2->set_status(2);
	auto* pProgress17_2 = pQuest17_2->add_progress();
	pProgress17_2->set_cur(1);
	pProgress17_2->set_max(1);
	// Quest 11203
	auto* pQuest17_3 = pAffinityQuests17->add_list();
	pQuest17_3->set_id(11203);
	pQuest17_3->set_type(Affinity);
	pQuest17_3->set_status(2);
	auto* pProgress17_3 = pQuest17_3->add_progress();
	pProgress17_3->set_cur(1);
	pProgress17_3->set_max(1);
	// Quest 11204
	auto* pQuest17_4 = pAffinityQuests17->add_list();
	pQuest17_4->set_id(11204);
	pQuest17_4->set_type(Affinity);
	pQuest17_4->set_status(2);
	auto* pProgress17_4 = pQuest17_4->add_progress();
	pProgress17_4->set_cur(1);
	pProgress17_4->set_max(1);
	// Quest 11202
	auto* pQuest17_5 = pAffinityQuests17->add_list();
	pQuest17_5->set_id(11202);
	pQuest17_5->set_type(Affinity);
	auto* pProgress17_5 = pQuest17_5->add_progress();
	pProgress17_5->set_max(1);
	// Quest 11206
	auto* pQuest17_6 = pAffinityQuests17->add_list();
	pQuest17_6->set_id(11206);
	pQuest17_6->set_type(Affinity);
	auto* pProgress17_6 = pQuest17_6->add_progress();
	pProgress17_6->set_max(1);
	// Quest 11207
	auto* pQuest17_7 = pAffinityQuests17->add_list();
	pQuest17_7->set_id(11207);
	pQuest17_7->set_type(Affinity);
	auto* pProgress17_7 = pQuest17_7->add_progress();
	pProgress17_7->set_max(1);
	// Quest 11208
	auto* pQuest17_8 = pAffinityQuests17->add_list();
	pQuest17_8->set_id(11208);
	pQuest17_8->set_type(Affinity);
	auto* pProgress17_8 = pQuest17_8->add_progress();
	pProgress17_8->set_max(1);
	// Quest 11209
	auto* pQuest17_9 = pAffinityQuests17->add_list();
	pQuest17_9->set_id(11209);
	pQuest17_9->set_type(Affinity);
	auto* pProgress17_9 = pQuest17_9->add_progress();
	pProgress17_9->set_cur(14);
	pProgress17_9->set_max(16);

	// Char 18 (Tid: 141)
	auto* pChar18 = pChars->Add();
	pChar18->set_tid(141);
	pChar18->set_level(10);
	pChar18->set_advance(1);
	pChar18->set_createtime(1761655441);
	pChar18->set_skin(14101);
	pChar18->set_affinitylevel(0);
	pChar18->set_affinityexp(0);
	pChar18->set_talentbackground(1);
	pChar18->set_talentnodes(Base64Decode("AAAAAAAAg/8="));
	// SkillLvs
	pChar18->add_skilllvs(1);
	pChar18->add_skilllvs(1);
	pChar18->add_skilllvs(1);
	pChar18->add_skilllvs(1);
	pChar18->add_skilllvs(1);
	// CharGemSlots
	auto* pCharGemSlots18 = pChar18->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots18->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests18 = pChar18->mutable_affinityquests();
	// Quest 14101
	auto* pQuest18_1 = pAffinityQuests18->add_list();
	pQuest18_1->set_id(14101);
	pQuest18_1->set_type(Affinity);
	auto* pProgress18_1 = pQuest18_1->add_progress();
	pProgress18_1->set_max(1);
	// Quest 14102
	auto* pQuest18_2 = pAffinityQuests18->add_list();
	pQuest18_2->set_id(14102);
	pQuest18_2->set_type(Affinity);
	auto* pProgress18_2 = pQuest18_2->add_progress();
	pProgress18_2->set_max(1);
	// Quest 14103
	auto* pQuest18_3 = pAffinityQuests18->add_list();
	pQuest18_3->set_id(14103);
	pQuest18_3->set_type(Affinity);
	auto* pProgress18_3 = pQuest18_3->add_progress();
	pProgress18_3->set_max(1);
	// Quest 14104
	auto* pQuest18_4 = pAffinityQuests18->add_list();
	pQuest18_4->set_id(14104);
	pQuest18_4->set_type(Affinity);
	auto* pProgress18_4 = pQuest18_4->add_progress();
	pProgress18_4->set_max(3);
	// Quest 14105
	auto* pQuest18_5 = pAffinityQuests18->add_list();
	pQuest18_5->set_id(14105);
	pQuest18_5->set_type(Affinity);
	auto* pProgress18_5 = pQuest18_5->add_progress();
	pProgress18_5->set_max(1);
	// Quest 14106
	auto* pQuest18_6 = pAffinityQuests18->add_list();
	pQuest18_6->set_id(14106);
	pQuest18_6->set_type(Affinity);
	auto* pProgress18_6 = pQuest18_6->add_progress();
	pProgress18_6->set_max(1);
	// Quest 14107
	auto* pQuest18_7 = pAffinityQuests18->add_list();
	pQuest18_7->set_id(14107);
	pQuest18_7->set_type(Affinity);
	auto* pProgress18_7 = pQuest18_7->add_progress();
	pProgress18_7->set_max(1);
	// Quest 14108
	auto* pQuest18_8 = pAffinityQuests18->add_list();
	pQuest18_8->set_id(14108);
	pQuest18_8->set_type(Affinity);
	auto* pProgress18_8 = pQuest18_8->add_progress();
	pProgress18_8->set_max(1);
	// Quest 14109
	auto* pQuest18_9 = pAffinityQuests18->add_list();
	pQuest18_9->set_id(14109);
	pQuest18_9->set_type(Affinity);
	auto* pProgress18_9 = pQuest18_9->add_progress();
	pProgress18_9->set_max(16);

	// Char 19 (Tid: 132)
	auto* pChar19 = pChars->Add();
	pChar19->set_tid(132);
	pChar19->set_level(60);
	pChar19->set_advance(5);
	pChar19->set_createtime(1760972186);
	pChar19->set_skin(13202);
	pChar19->set_affinitylevel(10);
	pChar19->set_affinityexp(1200);
	pChar19->set_talentbackground(0);
	pChar19->set_talentnodes(Base64Decode("AAAAAAAAAAA="));
	// SkillLvs
	pChar19->add_skilllvs(6);
	pChar19->add_skilllvs(4);
	pChar19->add_skilllvs(3);
	pChar19->add_skilllvs(3);
	pChar19->add_skilllvs(1);
	// Plots
	pChar19->add_plots(13201);
	pChar19->add_plots(13202);
	// CharGemSlots
	auto* pCharGemSlots19 = pChar19->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots19->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests19 = pChar19->mutable_affinityquests();
	// Quest 13201
	auto* pQuest19_1 = pAffinityQuests19->add_list();
	pQuest19_1->set_id(13201);
	pQuest19_1->set_type(Affinity);
	pQuest19_1->set_status(2);
	auto* pProgress19_1 = pQuest19_1->add_progress();
	pProgress19_1->set_cur(1);
	pProgress19_1->set_max(1);
	// Quest 13204
	auto* pQuest19_2 = pAffinityQuests19->add_list();
	pQuest19_2->set_id(13204);
	pQuest19_2->set_type(Affinity);
	pQuest19_2->set_status(2);
	auto* pProgress19_2 = pQuest19_2->add_progress();
	pProgress19_2->set_cur(1);
	pProgress19_2->set_max(1);
	// Quest 13205
	auto* pQuest19_3 = pAffinityQuests19->add_list();
	pQuest19_3->set_id(13205);
	pQuest19_3->set_type(Affinity);
	pQuest19_3->set_status(2);
	auto* pProgress19_3 = pQuest19_3->add_progress();
	pProgress19_3->set_cur(1);
	pProgress19_3->set_max(1);
	// Quest 13209
	auto* pQuest19_4 = pAffinityQuests19->add_list();
	pQuest19_4->set_id(13209);
	pQuest19_4->set_type(Affinity);
	pQuest19_4->set_status(2);
	auto* pProgress19_4 = pQuest19_4->add_progress();
	pProgress19_4->set_cur(1);
	pProgress19_4->set_max(1);
	// Quest 13203
	auto* pQuest19_5 = pAffinityQuests19->add_list();
	pQuest19_5->set_id(13203);
	pQuest19_5->set_type(Affinity);
	pQuest19_5->set_status(2);
	auto* pProgress19_5 = pQuest19_5->add_progress();
	pProgress19_5->set_cur(1);
	pProgress19_5->set_max(1);
	// Quest 13206
	auto* pQuest19_6 = pAffinityQuests19->add_list();
	pQuest19_6->set_id(13206);
	pQuest19_6->set_type(Affinity);
	pQuest19_6->set_status(2);
	auto* pProgress19_6 = pQuest19_6->add_progress();
	pProgress19_6->set_cur(1);
	pProgress19_6->set_max(1);
	// Quest 13202
	auto* pQuest19_7 = pAffinityQuests19->add_list();
	pQuest19_7->set_id(13202);
	pQuest19_7->set_type(Affinity);
	auto* pProgress19_7 = pQuest19_7->add_progress();
	pProgress19_7->set_max(1);
	// Quest 13207
	auto* pQuest19_8 = pAffinityQuests19->add_list();
	pQuest19_8->set_id(13207);
	pQuest19_8->set_type(Affinity);
	auto* pProgress19_8 = pQuest19_8->add_progress();
	pProgress19_8->set_max(1);
	// Quest 13208
	auto* pQuest19_9 = pAffinityQuests19->add_list();
	pQuest19_9->set_id(13208);
	pQuest19_9->set_type(Affinity);
	auto* pProgress19_9 = pQuest19_9->add_progress();
	pProgress19_9->set_max(1);

	// Char 20 (Tid: 149)
	auto* pChar20 = pChars->Add();
	pChar20->set_tid(149);
	pChar20->set_level(1);
	pChar20->set_advance(0);
	pChar20->set_createtime(1764612997);
	pChar20->set_skin(14901);
	pChar20->set_affinitylevel(0);
	pChar20->set_affinityexp(0);
	pChar20->set_talentbackground(0);
	pChar20->set_talentnodes(Base64Decode("AAAAAAAAAAA="));
	// SkillLvs
	pChar20->add_skilllvs(1);
	pChar20->add_skilllvs(1);
	pChar20->add_skilllvs(1);
	pChar20->add_skilllvs(1);
	pChar20->add_skilllvs(1);
	// CharGemSlots
	auto* pCharGemSlots20 = pChar20->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots20->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests20 = pChar20->mutable_affinityquests();
	// Quest 14901
	auto* pQuest20_1 = pAffinityQuests20->add_list();
	pQuest20_1->set_id(14901);
	pQuest20_1->set_type(Affinity);
	auto* pProgress20_1 = pQuest20_1->add_progress();
	pProgress20_1->set_max(1);
	// Quest 14902
	auto* pQuest20_2 = pAffinityQuests20->add_list();
	pQuest20_2->set_id(14902);
	pQuest20_2->set_type(Affinity);
	auto* pProgress20_2 = pQuest20_2->add_progress();
	pProgress20_2->set_max(1);
	// Quest 14903
	auto* pQuest20_3 = pAffinityQuests20->add_list();
	pQuest20_3->set_id(14903);
	pQuest20_3->set_type(Affinity);
	auto* pProgress20_3 = pQuest20_3->add_progress();
	pProgress20_3->set_max(1);
	// Quest 14904
	auto* pQuest20_4 = pAffinityQuests20->add_list();
	pQuest20_4->set_id(14904);
	pQuest20_4->set_type(Affinity);
	auto* pProgress20_4 = pQuest20_4->add_progress();
	pProgress20_4->set_max(3);
	// Quest 14905
	auto* pQuest20_5 = pAffinityQuests20->add_list();
	pQuest20_5->set_id(14905);
	pQuest20_5->set_type(Affinity);
	auto* pProgress20_5 = pQuest20_5->add_progress();
	pProgress20_5->set_max(1);
	// Quest 14906
	auto* pQuest20_6 = pAffinityQuests20->add_list();
	pQuest20_6->set_id(14906);
	pQuest20_6->set_type(Affinity);
	auto* pProgress20_6 = pQuest20_6->add_progress();
	pProgress20_6->set_max(1);
	// Quest 14907
	auto* pQuest20_7 = pAffinityQuests20->add_list();
	pQuest20_7->set_id(14907);
	pQuest20_7->set_type(Affinity);
	auto* pProgress20_7 = pQuest20_7->add_progress();
	pProgress20_7->set_max(1);
	// Quest 14908
	auto* pQuest20_8 = pAffinityQuests20->add_list();
	pQuest20_8->set_id(14908);
	pQuest20_8->set_type(Affinity);
	auto* pProgress20_8 = pQuest20_8->add_progress();
	pProgress20_8->set_max(1);
	// Quest 14909
	auto* pQuest20_9 = pAffinityQuests20->add_list();
	pQuest20_9->set_id(14909);
	pQuest20_9->set_type(Affinity);
	auto* pProgress20_9 = pQuest20_9->add_progress();
	pProgress20_9->set_max(16);

	// Char 21 (Tid: 150)
	auto* pChar21 = pChars->Add();
	pChar21->set_tid(150);
	pChar21->set_level(1);
	pChar21->set_advance(0);
	pChar21->set_createtime(1761068138);
	pChar21->set_skin(15001);
	pChar21->set_affinitylevel(0);
	pChar21->set_affinityexp(0);
	pChar21->set_talentbackground(0);
	pChar21->set_talentnodes(Base64Decode("AAAAAAAAAB8="));
	// SkillLvs
	pChar21->add_skilllvs(1);
	pChar21->add_skilllvs(1);
	pChar21->add_skilllvs(1);
	pChar21->add_skilllvs(1);
	pChar21->add_skilllvs(1);
	// CharGemSlots
	auto* pCharGemSlots21 = pChar21->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots21->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests21 = pChar21->mutable_affinityquests();
	// Quest 15001
	auto* pQuest21_1 = pAffinityQuests21->add_list();
	pQuest21_1->set_id(15001);
	pQuest21_1->set_type(Affinity);
	auto* pProgress21_1 = pQuest21_1->add_progress();
	pProgress21_1->set_max(1);
	// Quest 15002
	auto* pQuest21_2 = pAffinityQuests21->add_list();
	pQuest21_2->set_id(15002);
	pQuest21_2->set_type(Affinity);
	auto* pProgress21_2 = pQuest21_2->add_progress();
	pProgress21_2->set_max(1);
	// Quest 15003
	auto* pQuest21_3 = pAffinityQuests21->add_list();
	pQuest21_3->set_id(15003);
	pQuest21_3->set_type(Affinity);
	auto* pProgress21_3 = pQuest21_3->add_progress();
	pProgress21_3->set_max(1);
	// Quest 15004
	auto* pQuest21_4 = pAffinityQuests21->add_list();
	pQuest21_4->set_id(15004);
	pQuest21_4->set_type(Affinity);
	auto* pProgress21_4 = pQuest21_4->add_progress();
	pProgress21_4->set_max(3);
	// Quest 15005
	auto* pQuest21_5 = pAffinityQuests21->add_list();
	pQuest21_5->set_id(15005);
	pQuest21_5->set_type(Affinity);
	auto* pProgress21_5 = pQuest21_5->add_progress();
	pProgress21_5->set_max(1);
	// Quest 15006
	auto* pQuest21_6 = pAffinityQuests21->add_list();
	pQuest21_6->set_id(15006);
	pQuest21_6->set_type(Affinity);
	auto* pProgress21_6 = pQuest21_6->add_progress();
	pProgress21_6->set_max(1);
	// Quest 15007
	auto* pQuest21_7 = pAffinityQuests21->add_list();
	pQuest21_7->set_id(15007);
	pQuest21_7->set_type(Affinity);
	auto* pProgress21_7 = pQuest21_7->add_progress();
	pProgress21_7->set_max(1);
	// Quest 15008
	auto* pQuest21_8 = pAffinityQuests21->add_list();
	pQuest21_8->set_id(15008);
	pQuest21_8->set_type(Affinity);
	auto* pProgress21_8 = pQuest21_8->add_progress();
	pProgress21_8->set_max(1);
	// Quest 15009
	auto* pQuest21_9 = pAffinityQuests21->add_list();
	pQuest21_9->set_id(15009);
	pQuest21_9->set_type(Affinity);
	auto* pProgress21_9 = pQuest21_9->add_progress();
	pProgress21_9->set_max(16);

	// Char 22 (Tid: 126)
	auto* pChar22 = pChars->Add();
	pChar22->set_tid(126);
	pChar22->set_level(90);
	pChar22->set_advance(8);
	pChar22->set_createtime(1761316226);
	pChar22->set_skin(12602);
	pChar22->set_affinitylevel(1);
	pChar22->set_affinityexp(100);
	pChar22->set_talentbackground(2);
	pChar22->set_talentnodes(Base64Decode("AAAAH4P/g/8="));
	// SkillLvs
	pChar22->add_skilllvs(2);
	pChar22->add_skilllvs(6);
	pChar22->add_skilllvs(3);
	pChar22->add_skilllvs(5);
	pChar22->add_skilllvs(1);
	// Plots
	pChar22->add_plots(12601);
	// CharGemSlots
	auto* pCharGemSlots22 = pChar22->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots22->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests22 = pChar22->mutable_affinityquests();
	// Quest 12605
	auto* pQuest22_1 = pAffinityQuests22->add_list();
	pQuest22_1->set_id(12605);
	pQuest22_1->set_type(Affinity);
	pQuest22_1->set_status(2);
	auto* pProgress22_1 = pQuest22_1->add_progress();
	pProgress22_1->set_cur(1);
	pProgress22_1->set_max(1);
	// Quest 12601
	auto* pQuest22_2 = pAffinityQuests22->add_list();
	pQuest22_2->set_id(12601);
	pQuest22_2->set_type(Affinity);
	pQuest22_2->set_status(2);
	auto* pProgress22_2 = pQuest22_2->add_progress();
	pProgress22_2->set_cur(1);
	pProgress22_2->set_max(1);
	// Quest 12602
	auto* pQuest22_3 = pAffinityQuests22->add_list();
	pQuest22_3->set_id(12602);
	pQuest22_3->set_type(Affinity);
	auto* pProgress22_3 = pQuest22_3->add_progress();
	pProgress22_3->set_max(1);
	// Quest 12603
	auto* pQuest22_4 = pAffinityQuests22->add_list();
	pQuest22_4->set_id(12603);
	pQuest22_4->set_type(Affinity);
	pQuest22_4->set_status(1);
	auto* pProgress22_4 = pQuest22_4->add_progress();
	pProgress22_4->set_cur(1);
	pProgress22_4->set_max(1);
	// Quest 12604
	auto* pQuest22_5 = pAffinityQuests22->add_list();
	pQuest22_5->set_id(12604);
	pQuest22_5->set_type(Affinity);
	pQuest22_5->set_status(1);
	auto* pProgress22_5 = pQuest22_5->add_progress();
	pProgress22_5->set_cur(3);
	pProgress22_5->set_max(3);
	// Quest 12606
	auto* pQuest22_6 = pAffinityQuests22->add_list();
	pQuest22_6->set_id(12606);
	pQuest22_6->set_type(Affinity);
	pQuest22_6->set_status(1);
	auto* pProgress22_6 = pQuest22_6->add_progress();
	pProgress22_6->set_cur(1);
	pProgress22_6->set_max(1);
	// Quest 12607
	auto* pQuest22_7 = pAffinityQuests22->add_list();
	pQuest22_7->set_id(12607);
	pQuest22_7->set_type(Affinity);
	auto* pProgress22_7 = pQuest22_7->add_progress();
	pProgress22_7->set_max(1);
	// Quest 12608
	auto* pQuest22_8 = pAffinityQuests22->add_list();
	pQuest22_8->set_id(12608);
	pQuest22_8->set_type(Affinity);
	auto* pProgress22_8 = pQuest22_8->add_progress();
	pProgress22_8->set_max(1);
	// Quest 12609
	auto* pQuest22_9 = pAffinityQuests22->add_list();
	pQuest22_9->set_id(12609);
	pQuest22_9->set_type(Affinity);
	auto* pProgress22_9 = pQuest22_9->add_progress();
	pProgress22_9->set_cur(15);
	pProgress22_9->set_max(16);

	// Char 23 (Tid: 118)
	auto* pChar23 = pChars->Add();
	pChar23->set_tid(118);
	pChar23->set_level(1);
	pChar23->set_advance(0);
	pChar23->set_createtime(1762681457);
	pChar23->set_skin(11801);
	pChar23->set_affinitylevel(0);
	pChar23->set_affinityexp(0);
	pChar23->set_talentbackground(0);
	pChar23->set_talentnodes(Base64Decode("AAAAAAAAAAA="));
	// SkillLvs
	pChar23->add_skilllvs(1);
	pChar23->add_skilllvs(1);
	pChar23->add_skilllvs(1);
	pChar23->add_skilllvs(1);
	pChar23->add_skilllvs(1);
	// CharGemSlots
	auto* pCharGemSlots23 = pChar23->mutable_chargemslots();
	for (int i = 1; i <= 3; i++) {
		auto* pSlot = pCharGemSlots23->Add();
		pSlot->set_id(i);
	}
	// AffinityQuests
	auto* pAffinityQuests23 = pChar23->mutable_affinityquests();
	// Quest 11801
	auto* pQuest23_1 = pAffinityQuests23->add_list();
	pQuest23_1->set_id(11801);
	pQuest23_1->set_type(Affinity);
	auto* pProgress23_1 = pQuest23_1->add_progress();
	pProgress23_1->set_max(1);
	// Quest 11802
	auto* pQuest23_2 = pAffinityQuests23->add_list();
	pQuest23_2->set_id(11802);
	pQuest23_2->set_type(Affinity);
	auto* pProgress23_2 = pQuest23_2->add_progress();
	pProgress23_2->set_max(1);
	// Quest 11803
	auto* pQuest23_3 = pAffinityQuests23->add_list();
	pQuest23_3->set_id(11803);
	pQuest23_3->set_type(Affinity);
	auto* pProgress23_3 = pQuest23_3->add_progress();
	pProgress23_3->set_max(1);
	// Quest 11804
	auto* pQuest23_4 = pAffinityQuests23->add_list();
	pQuest23_4->set_id(11804);
	pQuest23_4->set_type(Affinity);
	auto* pProgress23_4 = pQuest23_4->add_progress();
	pProgress23_4->set_max(3);
	// Quest 11805
	auto* pQuest23_5 = pAffinityQuests23->add_list();
	pQuest23_5->set_id(11805);
	pQuest23_5->set_type(Affinity);
	auto* pProgress23_5 = pQuest23_5->add_progress();
	pProgress23_5->set_max(1);
	// Quest 11806
	auto* pQuest23_6 = pAffinityQuests23->add_list();
	pQuest23_6->set_id(11806);
	pQuest23_6->set_type(Affinity);
	auto* pProgress23_6 = pQuest23_6->add_progress();
	pProgress23_6->set_max(1);
	// Quest 11807
	auto* pQuest23_7 = pAffinityQuests23->add_list();
	pQuest23_7->set_id(11807);
	pQuest23_7->set_type(Affinity);
	auto* pProgress23_7 = pQuest23_7->add_progress();
	pProgress23_7->set_max(1);
	// Quest 11808
	auto* pQuest23_8 = pAffinityQuests23->add_list();
	pQuest23_8->set_id(11808);
	pQuest23_8->set_type(Affinity);
	auto* pProgress23_8 = pQuest23_8->add_progress();
	pProgress23_8->set_max(1);
	// Quest 11809
	auto* pQuest23_9 = pAffinityQuests23->add_list();
	pQuest23_9->set_id(11809);
	pQuest23_9->set_type(Affinity);
	auto* pProgress23_9 = pQuest23_9->add_progress();
	pProgress23_9->set_max(16);


	// DailyInstances
	auto* pDailyInstances = info.mutable_dailyinstances();

	auto* pDailyInstance1 = pDailyInstances->Add();
	pDailyInstance1->set_id(1002);
	pDailyInstance1->set_star(7);
	pDailyInstance1->set_buildid(1761393282);

	auto* pDailyInstance2 = pDailyInstances->Add();
	pDailyInstance2->set_id(1003);
	pDailyInstance2->set_star(7);
	pDailyInstance2->set_buildid(1764059826);

	auto* pDailyInstance3 = pDailyInstances->Add();
	pDailyInstance3->set_id(1004);
	pDailyInstance3->set_star(7);
	pDailyInstance3->set_buildid(1764059826);

	auto* pDailyInstance4 = pDailyInstances->Add();
	pDailyInstance4->set_id(1005);
	pDailyInstance4->set_star(7);
	pDailyInstance4->set_buildid(1764786061);

	auto* pDailyInstance5 = pDailyInstances->Add();
	pDailyInstance5->set_id(1000);
	pDailyInstance5->set_star(7);
	pDailyInstance5->set_buildid(1760972241);

	auto* pDailyInstance6 = pDailyInstances->Add();
	pDailyInstance6->set_id(1001);
	pDailyInstance6->set_star(7);
	pDailyInstance6->set_buildid(1761320747);

	// DailyMallRewardStatus
	info.set_dailymallrewardstatus(true);

	// DailyShopRewardStatus
	info.set_dailyshoprewardstatus(true);

	// === Dictionaries ===
	auto* pDictionaries = info.mutable_dictionaries();

	// Dictionary 1 (TabId: 3)
	auto* pDictionary1 = pDictionaries->Add();
	pDictionary1->set_tabid(3);
	auto* pEntries1 = pDictionary1->mutable_entries();
	// Entry 1
	auto* pEntry1_1 = pEntries1->Add();
	pEntry1_1->set_index(1);
	pEntry1_1->set_status(2);
	// Entry 2
	auto* pEntry1_2 = pEntries1->Add();
	pEntry1_2->set_index(2);
	pEntry1_2->set_status(2);
	// Entry 3
	auto* pEntry1_3 = pEntries1->Add();
	pEntry1_3->set_index(3);
	pEntry1_3->set_status(1);
	// Entry 4
	auto* pEntry1_4 = pEntries1->Add();
	pEntry1_4->set_index(4);
	pEntry1_4->set_status(1);
	// Entry 5
	auto* pEntry1_5 = pEntries1->Add();
	pEntry1_5->set_index(5);
	pEntry1_5->set_status(1);
	// Entry 6
	auto* pEntry1_6 = pEntries1->Add();
	pEntry1_6->set_index(6);
	pEntry1_6->set_status(1);
	// Entry 7
	auto* pEntry1_7 = pEntries1->Add();
	pEntry1_7->set_index(7);
	pEntry1_7->set_status(1);

	// Dictionary 2 (TabId: 2)
	auto* pDictionary2 = pDictionaries->Add();
	pDictionary2->set_tabid(2);
	auto* pEntries2 = pDictionary2->mutable_entries();
	// Entry 1
	auto* pEntry2_1 = pEntries2->Add();
	pEntry2_1->set_index(1);
	pEntry2_1->set_status(2);
	// Entry 2
	auto* pEntry2_2 = pEntries2->Add();
	pEntry2_2->set_index(2);
	pEntry2_2->set_status(2);
	// Entry 3
	auto* pEntry2_3 = pEntries2->Add();
	pEntry2_3->set_index(3);
	pEntry2_3->set_status(2);
	// Entry 4
	auto* pEntry2_4 = pEntries2->Add();
	pEntry2_4->set_index(4);
	pEntry2_4->set_status(1);
	// Entry 5
	auto* pEntry2_5 = pEntries2->Add();
	pEntry2_5->set_index(5);
	pEntry2_5->set_status(1);
	// Entry 6
	auto* pEntry2_6 = pEntries2->Add();
	pEntry2_6->set_index(6);
	pEntry2_6->set_status(1);
	// Entry 7
	auto* pEntry2_7 = pEntries2->Add();
	pEntry2_7->set_index(7);
	pEntry2_7->set_status(1);
	// Entry 8
	auto* pEntry2_8 = pEntries2->Add();
	pEntry2_8->set_index(8);
	pEntry2_8->set_status(1);
	// Entry 9
	auto* pEntry2_9 = pEntries2->Add();
	pEntry2_9->set_index(9);
	pEntry2_9->set_status(1);
	// Entry 10
	auto* pEntry2_10 = pEntries2->Add();
	pEntry2_10->set_index(10);
	pEntry2_10->set_status(1);
	// Entry 11
	auto* pEntry2_11 = pEntries2->Add();
	pEntry2_11->set_index(11);
	pEntry2_11->set_status(1);

	// Dictionary 3 (TabId: 1)
	auto* pDictionary3 = pDictionaries->Add();
	pDictionary3->set_tabid(1);
	auto* pEntries3 = pDictionary3->mutable_entries();
	// Entry 1
	auto* pEntry3_1 = pEntries3->Add();
	pEntry3_1->set_index(1);
	pEntry3_1->set_status(2);
	// Entry 2
	auto* pEntry3_2 = pEntries3->Add();
	pEntry3_2->set_index(2);
	pEntry3_2->set_status(2);
	// Entry 3
	auto* pEntry3_3 = pEntries3->Add();
	pEntry3_3->set_index(3);
	pEntry3_3->set_status(2);
	// Entry 4
	auto* pEntry3_4 = pEntries3->Add();
	pEntry3_4->set_index(4);
	pEntry3_4->set_status(2);
	// Entry 5
	auto* pEntry3_5 = pEntries3->Add();
	pEntry3_5->set_index(5);
	pEntry3_5->set_status(2);
	// Entry 6
	auto* pEntry3_6 = pEntries3->Add();
	pEntry3_6->set_index(6);
	pEntry3_6->set_status(2);

	// === Discs ===
	auto* pDiscs = info.mutable_discs();

	auto* pDisc1 = pDiscs->Add();
	pDisc1->set_id(214041);
	pDisc1->set_level(90);
	pDisc1->set_phase(8);
	pDisc1->set_star(5);
	pDisc1->set_createtime(1767690211);

	auto* pDisc2 = pDiscs->Add();
	pDisc2->set_id(214042);
	pDisc2->set_level(30);
	pDisc2->set_phase(2);
	pDisc2->set_star(4);
	pDisc2->set_createtime(1770570752);

	auto* pDisc3 = pDiscs->Add();
	pDisc3->set_id(214050);
	pDisc3->set_level(1);
	pDisc3->set_createtime(1776869953);

	auto* pDisc4 = pDiscs->Add();
	pDisc4->set_id(212006);
	pDisc4->set_level(1);
	pDisc4->set_star(5);
	pDisc4->set_createtime(1760972186);

	auto* pDisc5 = pDiscs->Add();
	pDisc5->set_id(213008);
	pDisc5->set_level(70);
	pDisc5->set_phase(6);
	pDisc5->set_star(2);
	pDisc5->set_read(true);
	pDisc5->set_createtime(1760972520);

	auto* pDisc6 = pDiscs->Add();
	pDisc6->set_id(213010);
	pDisc6->set_level(3);
	pDisc6->set_exp(70);
	pDisc6->set_createtime(1762954761);

	auto* pDisc7 = pDiscs->Add();
	pDisc7->set_id(213016);
	pDisc7->set_level(1);
	pDisc7->set_createtime(1764870774);

	auto* pDisc8 = pDiscs->Add();
	pDisc8->set_id(213003);
	pDisc8->set_level(1);
	pDisc8->set_createtime(1777296997);

	auto* pDisc9 = pDiscs->Add();
	pDisc9->set_id(213011);
	pDisc9->set_level(70);
	pDisc9->set_phase(6);
	pDisc9->set_read(true);
	pDisc9->set_createtime(1760972186);

	auto* pDisc10 = pDiscs->Add();
	pDisc10->set_id(212001);
	pDisc10->set_level(1);
	pDisc10->set_star(5);
	pDisc10->set_createtime(1761068138);

	auto* pDisc11 = pDiscs->Add();
	pDisc11->set_id(213018);
	pDisc11->set_level(1);
	pDisc11->set_createtime(1764612948);

	auto* pDisc12 = pDiscs->Add();
	pDisc12->set_id(213014);
	pDisc12->set_level(1);
	pDisc12->set_createtime(1764870753);

	auto* pDisc13 = pDiscs->Add();
	pDisc13->set_id(211006);
	pDisc13->set_level(1);
	pDisc13->set_star(5);
	pDisc13->set_createtime(1760972186);

	auto* pDisc14 = pDiscs->Add();
	pDisc14->set_id(212005);
	pDisc14->set_level(1);
	pDisc14->set_star(5);
	pDisc14->set_createtime(1760972543);

	auto* pDisc15 = pDiscs->Add();
	pDisc15->set_id(213007);
	pDisc15->set_level(60);
	pDisc15->set_phase(5);
	pDisc15->set_star(3);
	pDisc15->set_read(true);
	pDisc15->set_createtime(1760979225);

	auto* pDisc16 = pDiscs->Add();
	pDisc16->set_id(214043);
	pDisc16->set_level(1);
	pDisc16->set_createtime(1768985542);

	auto* pDisc17 = pDiscs->Add();
	pDisc17->set_id(213012);
	pDisc17->set_level(1);
	pDisc17->set_createtime(1778843687);

	auto* pDisc18 = pDiscs->Add();
	pDisc18->set_id(214003);
	pDisc18->set_level(54);
	pDisc18->set_phase(5);
	pDisc18->set_star(1);
	pDisc18->set_exp(6700);
	pDisc18->set_createtime(1778987062);

	auto* pDisc19 = pDiscs->Add();
	pDisc19->set_id(212009);
	pDisc19->set_level(1);
	pDisc19->set_star(5);
	pDisc19->set_createtime(1760972543);

	auto* pDisc20 = pDiscs->Add();
	pDisc20->set_id(212003);
	pDisc20->set_level(1);
	pDisc20->set_star(5);
	pDisc20->set_createtime(1760972543);

	auto* pDisc21 = pDiscs->Add();
	pDisc21->set_id(212002);
	pDisc21->set_level(1);
	pDisc21->set_star(5);
	pDisc21->set_createtime(1761068138);

	auto* pDisc22 = pDiscs->Add();
	pDisc22->set_id(212007);
	pDisc22->set_level(1);
	pDisc22->set_star(5);
	pDisc22->set_createtime(1761068138);

	auto* pDisc23 = pDiscs->Add();
	pDisc23->set_id(213006);
	pDisc23->set_level(60);
	pDisc23->set_phase(5);
	pDisc23->set_read(true);
	pDisc23->set_createtime(1761655441);

	auto* pDisc24 = pDiscs->Add();
	pDisc24->set_id(214039);
	pDisc24->set_level(70);
	pDisc24->set_phase(7);
	pDisc24->set_star(5);
	pDisc24->set_read(true);
	pDisc24->set_createtime(1762784312);

	auto* pDisc25 = pDiscs->Add();
	pDisc25->set_id(211002);
	pDisc25->set_level(1);
	pDisc25->set_star(5);
	pDisc25->set_createtime(1760972535);

	auto* pDisc26 = pDiscs->Add();
	pDisc26->set_id(211003);
	pDisc26->set_level(1);
	pDisc26->set_star(5);
	pDisc26->set_createtime(1760972535);

	auto* pDisc27 = pDiscs->Add();
	pDisc27->set_id(212010);
	pDisc27->set_level(1);
	pDisc27->set_star(2);
	pDisc27->set_createtime(1761575657);

	auto* pDisc28 = pDiscs->Add();
	pDisc28->set_id(213005);
	pDisc28->set_level(1);
	pDisc28->set_createtime(1764612948);

	auto* pDisc29 = pDiscs->Add();
	pDisc29->set_id(214024);
	pDisc29->set_level(80);
	pDisc29->set_phase(7);
	pDisc29->set_star(5);
	pDisc29->set_read(true);
	pDisc29->set_createtime(1766291175);

	auto* pDisc30 = pDiscs->Add();
	pDisc30->set_id(214047);
	pDisc30->set_level(1);
	pDisc30->set_createtime(1774520126);

	auto* pDisc31 = pDiscs->Add();
	pDisc31->set_id(211007);
	pDisc31->set_level(1);
	pDisc31->set_star(5);
	pDisc31->set_createtime(1760971340);

	auto* pDisc32 = pDiscs->Add();
	pDisc32->set_id(212008);
	pDisc32->set_level(1);
	pDisc32->set_star(5);
	pDisc32->set_createtime(1760972535);

	auto* pDisc33 = pDiscs->Add();
	pDisc33->set_id(214046);
	pDisc33->set_level(6);
	pDisc33->set_star(5);
	pDisc33->set_exp(150);
	pDisc33->set_createtime(1772390659);

	auto* pDisc34 = pDiscs->Add();
	pDisc34->set_id(214049);
	pDisc34->set_level(1);
	pDisc34->set_createtime(1776429462);

	auto* pDisc35 = pDiscs->Add();
	pDisc35->set_id(211005);
	pDisc35->set_level(1);
	pDisc35->set_star(5);
	pDisc35->set_createtime(1760971340);

	auto* pDisc36 = pDiscs->Add();
	pDisc36->set_id(211001);
	pDisc36->set_level(1);
	pDisc36->set_star(5);
	pDisc36->set_createtime(1760971340);

	auto* pDisc37 = pDiscs->Add();
	pDisc37->set_id(211008);
	pDisc37->set_level(1);
	pDisc37->set_star(5);
	pDisc37->set_createtime(1760971340);

	auto* pDisc38 = pDiscs->Add();
	pDisc38->set_id(211004);
	pDisc38->set_level(1);
	pDisc38->set_star(5);
	pDisc38->set_createtime(1760972535);

	auto* pDisc39 = pDiscs->Add();
	pDisc39->set_id(214040);
	pDisc39->set_level(90);
	pDisc39->set_phase(8);
	pDisc39->set_star(5);
	pDisc39->set_createtime(1764612905);

	// === Energy ===
	auto* pEnergy = info.mutable_energy();
	pEnergy->mutable_energy()->set_isprimary(true);
	pEnergy->mutable_energy()->set_nextduration(320);
	pEnergy->mutable_energy()->set_primary(240);
	pEnergy->mutable_energy()->set_updatetime(1779090900);

	// === Formation ===
	auto* pFormation = info.mutable_formation();

	// Formation Info
	auto* pFormationInfo = pFormation->mutable_info();

	// Formation 1 (Number: 1)
	auto* pFormation1 = pFormationInfo->Add();
	pFormation1->set_number(1);
	// CharIds
	pFormation1->add_charids(134);
	pFormation1->add_charids(126);
	pFormation1->add_charids(158);
	// DiscIds
	pFormation1->add_discids(214040);
	pFormation1->add_discids(214041);
	pFormation1->add_discids(214024);
	pFormation1->add_discids(213008);
	pFormation1->add_discids(213011);
	pFormation1->add_discids(0);

	// Formation 2 (Number: 2)
	auto* pFormation2 = pFormationInfo->Add();
	pFormation2->set_number(2);
	// CharIds
	pFormation2->add_charids(103);
	pFormation2->add_charids(126);
	pFormation2->add_charids(141);
	// DiscIds (all 0)
	pFormation2->add_discids(0);
	pFormation2->add_discids(0);
	pFormation2->add_discids(0);

	// Formation Record
	auto* pFormationRecord = pFormation->mutable_record();

	// Record 1
	auto* pRecord1 = pFormationRecord->Add();
	pRecord1->set_groupid(4);
	pRecord1->set_number(1);

	// Record 2
	auto* pRecord2 = pFormationRecord->Add();
	pRecord2->set_groupid(2);
	pRecord2->set_number(1);

	// Record 3
	auto* pRecord3 = pFormationRecord->Add();
	pRecord3->set_groupid(1);
	pRecord3->set_number(1);

	// Record 4
	auto* pRecord4 = pFormationRecord->Add();
	pRecord4->set_groupid(3);
	pRecord4->set_number(1);


	// === Handbook ===
	auto* pHandbook = info.mutable_handbook();

	// Handbook 1 (Type: 1)
	auto* pHandbook1 = pHandbook->Add();
	pHandbook1->set_type(1);
	pHandbook1->set_data(Base64Decode("Ao4wcUdwaDsAAAAAAAAZqg=="));

	// Handbook 2 (Type: 3)
	auto* pHandbook2 = pHandbook->Add();
	pHandbook2->set_type(3);
	pHandbook2->set_data(Base64Decode("AAAAf/////8="));

	// Handbook 3 (Type: 2)
	auto* pHandbook3 = pHandbook->Add();
	pHandbook3->set_type(2);
	pHandbook3->set_data(Base64Decode("AANnwACAAAQ="));

	// Handbook 4 (Type: 5)
	auto* pHandbook4 = pHandbook->Add();
	pHandbook4->set_type(5);
	pHandbook4->set_data(Base64Decode("AAAAAAAAAAM="));

	// === HonorList ===
	auto* pHonorList = info.mutable_honorlist();
	pHonorList->Add(114003);
	pHonorList->Add(116001);
	pHonorList->Add(116002);
	pHonorList->Add(116003);
	pHonorList->Add(116006);
	pHonorList->Add(116007);
	pHonorList->Add(112134);
	pHonorList->Add(112103);
	pHonorList->Add(116009);
	pHonorList->Add(112132);

	// === Honors ===
	auto* pHonors = info.mutable_honors();

	auto* pHonor1 = pHonors->Add();
	pHonor1->set_id(119901);
	pHonor1->set_affinitylv(1);

	auto* pHonor2 = pHonors->Add();
	pHonor2->set_id(116003);

	auto* pHonor3 = pHonors->Add();
	pHonor3->set_id(116001);

	// === Items ===
	auto* pItems = info.mutable_items();

	// Item 1
	auto* pItem1 = pItems->Add();
	pItem1->set_tid(81009);
	pItem1->set_qty(1);
	pItem1->set_expire(1779652800);
	pItem1->set_id(660013937646901093);

	// Item 2
	auto* pItem2 = pItems->Add();
	pItem2->set_tid(1120);
	pItem2->set_qty(30);

	// Item 3
	auto* pItem3 = pItems->Add();
	pItem3->set_tid(1030);
	pItem3->set_qty(30);

	// Item 4
	auto* pItem4 = pItems->Add();
	pItem4->set_tid(81009);
	pItem4->set_qty(2);
	pItem4->set_expire(1779652800);
	pItem4->set_id(661496423707055054);

	// Item 5
	auto* pItem5 = pItems->Add();
	pItem5->set_tid(81010);
	pItem5->set_qty(1);
	pItem5->set_expire(1779652800);
	pItem5->set_id(661496423707055061);

	// Item 6
	auto* pItem6 = pItems->Add();
	pItem6->set_tid(21072);
	pItem6->set_qty(16);

	// Item 7
	auto* pItem7 = pItems->Add();
	pItem7->set_tid(32012);
	pItem7->set_qty(200);

	// Item 8
	auto* pItem8 = pItems->Add();
	pItem8->set_tid(30002);
	pItem8->set_qty(143);

	// Item 9
	auto* pItem9 = pItems->Add();
	pItem9->set_tid(81009);
	pItem9->set_qty(1);
	pItem9->set_expire(1779652800);
	pItem9->set_id(661496423707055070);

	// Item 10
	auto* pItem10 = pItems->Add();
	pItem10->set_tid(50002);
	pItem10->set_qty(1);

	// Item 11
	auto* pItem11 = pItems->Add();
	pItem11->set_tid(32002);
	pItem11->set_qty(20);

	// Item 12
	auto* pItem12 = pItems->Add();
	pItem12->set_tid(82005);
	pItem12->set_qty(50);

	// Item 13
	auto* pItem13 = pItems->Add();
	pItem13->set_tid(603);
	pItem13->set_qty(18600);

	// Item 14
	auto* pItem14 = pItems->Add();
	pItem14->set_tid(81009);
	pItem14->set_qty(4);
	pItem14->set_expire(1779652800);
	pItem14->set_id(660717635914174521);

	// Item 15
	auto* pItem15 = pItems->Add();
	pItem15->set_tid(503);
	pItem15->set_qty(70);

	// Item 16
	auto* pItem16 = pItems->Add();
	pItem16->set_tid(81003);
	pItem16->set_qty(2);

	// Item 17
	auto* pItem17 = pItems->Add();
	pItem17->set_tid(21023);
	pItem17->set_qty(4);

	// Item 18
	auto* pItem18 = pItems->Add();
	pItem18->set_tid(81009);
	pItem18->set_qty(1);
	pItem18->set_expire(1779652800);
	pItem18->set_id(661496423707055057);

	// Item 19
	auto* pItem19 = pItems->Add();
	pItem19->set_tid(20072);
	pItem19->set_qty(10);

	// Item 20
	auto* pItem20 = pItems->Add();
	pItem20->set_tid(224049);
	pItem20->set_qty(5);

	// Item 21
	auto* pItem21 = pItems->Add();
	pItem21->set_tid(51);
	pItem21->set_qty(50);

	// Item 22
	auto* pItem22 = pItems->Add();
	pItem22->set_tid(35000);
	pItem22->set_qty(26);

	// Item 23
	auto* pItem23 = pItems->Add();
	pItem23->set_tid(224043);
	pItem23->set_qty(2);

	// Item 24
	auto* pItem24 = pItems->Add();
	pItem24->set_tid(20071);
	pItem24->set_qty(1);

	// Item 25
	auto* pItem25 = pItems->Add();
	pItem25->set_tid(223014);
	pItem25->set_qty(1);

	// Item 26
	auto* pItem26 = pItems->Add();
	pItem26->set_tid(33072);
	pItem26->set_qty(1);

	// Item 27
	auto* pItem27 = pItems->Add();
	pItem27->set_tid(32023);
	pItem27->set_qty(11);

	// Item 28
	auto* pItem28 = pItems->Add();
	pItem28->set_tid(82004);
	pItem28->set_qty(1314);

	// Item 29
	auto* pItem29 = pItems->Add();
	pItem29->set_tid(20081);
	pItem29->set_qty(334);

	// Item 30
	auto* pItem30 = pItems->Add();
	pItem30->set_tid(81010);
	pItem30->set_qty(1);
	pItem30->set_expire(1779652800);
	pItem30->set_id(660717635914174515);

	// Item 31
	auto* pItem31 = pItems->Add();
	pItem31->set_tid(223018);
	pItem31->set_qty(2);

	// Item 32
	auto* pItem32 = pItems->Add();
	pItem32->set_tid(1470);
	pItem32->set_qty(90);

	// Item 33
	auto* pItem33 = pItems->Add();
	pItem33->set_tid(81009);
	pItem33->set_qty(3);
	pItem33->set_expire(1779652800);
	pItem33->set_id(661496423707055154);

	// Item 34
	auto* pItem34 = pItems->Add();
	pItem34->set_tid(32022);
	pItem34->set_qty(45);

	// Item 35
	auto* pItem35 = pItems->Add();
	pItem35->set_tid(35003);
	pItem35->set_qty(174);

	// Item 36
	auto* pItem36 = pItems->Add();
	pItem36->set_tid(86011);
	pItem36->set_qty(1);

	// Item 37
	auto* pItem37 = pItems->Add();
	pItem37->set_tid(1170);
	pItem37->set_qty(30);

	// Item 38
	auto* pItem38 = pItems->Add();
	pItem38->set_tid(20023);
	pItem38->set_qty(4);

	// Item 39
	auto* pItem39 = pItems->Add();
	pItem39->set_tid(32011);
	pItem39->set_qty(251);

	// Item 40
	auto* pItem40 = pItems->Add();
	pItem40->set_tid(33033);
	pItem40->set_qty(1);

	// Item 41
	auto* pItem41 = pItems->Add();
	pItem41->set_tid(223016);
	pItem41->set_qty(1);

	// Item 42
	auto* pItem42 = pItems->Add();
	pItem42->set_tid(30);
	pItem42->set_qty(112);

	// Item 43
	auto* pItem43 = pItems->Add();
	pItem43->set_tid(1420);
	pItem43->set_qty(60);

	// Item 44
	auto* pItem44 = pItems->Add();
	pItem44->set_tid(81009);
	pItem44->set_qty(2);
	pItem44->set_expire(1779652800);
	pItem44->set_id(660714711045642038);

	// Item 45
	auto* pItem45 = pItems->Add();
	pItem45->set_tid(81009);
	pItem45->set_qty(1);
	pItem45->set_expire(1779652800);
	pItem45->set_id(660717635914174514);

	// Item 46
	auto* pItem46 = pItems->Add();
	pItem46->set_tid(223007);
	pItem46->set_qty(1);

	// Item 47
	auto* pItem47 = pItems->Add();
	pItem47->set_tid(81010);
	pItem47->set_qty(10);
	pItem47->set_expire(1779652800);
	pItem47->set_id(661496423707055708);

	// Item 48
	auto* pItem48 = pItems->Add();
	pItem48->set_tid(35001);
	pItem48->set_qty(284);

	// Item 49
	auto* pItem49 = pItems->Add();
	pItem49->set_tid(33012);
	pItem49->set_qty(8);

	// Item 50
	auto* pItem50 = pItems->Add();
	pItem50->set_tid(1080);
	pItem50->set_qty(30);

	// Item 51
	auto* pItem51 = pItems->Add();
	pItem51->set_tid(33032);
	pItem51->set_qty(20);

	// Item 52
	auto* pItem52 = pItems->Add();
	pItem52->set_tid(86010);
	pItem52->set_qty(1);

	// Item 53
	auto* pItem53 = pItems->Add();
	pItem53->set_tid(20073);
	pItem53->set_qty(6);

	// Item 54
	auto* pItem54 = pItems->Add();
	pItem54->set_tid(224050);
	pItem54->set_qty(5);

	// Item 55
	auto* pItem55 = pItems->Add();
	pItem55->set_tid(82008);
	pItem55->set_qty(27);

	// Item 56
	auto* pItem56 = pItems->Add();
	pItem56->set_tid(82007);
	pItem56->set_qty(120);

	// Item 57
	auto* pItem57 = pItems->Add();
	pItem57->set_tid(33903);
	pItem57->set_qty(24);

	// Item 58
	auto* pItem58 = pItems->Add();
	pItem58->set_tid(1180);
	pItem58->set_qty(30);

	// Item 59
	auto* pItem59 = pItems->Add();
	pItem59->set_tid(33902);
	pItem59->set_qty(1);

	// Item 60
	auto* pItem60 = pItems->Add();
	pItem60->set_tid(224047);
	pItem60->set_qty(1);

	// Item 61
	auto* pItem61 = pItems->Add();
	pItem61->set_tid(30001);
	pItem61->set_qty(257);

	// Item 62
	auto* pItem62 = pItems->Add();
	pItem62->set_tid(73110);
	pItem62->set_qty(22400);

	// Item 63
	auto* pItem63 = pItems->Add();
	pItem63->set_tid(82002);
	pItem63->set_qty(32);

	// Item 64
	auto* pItem64 = pItems->Add();
	pItem64->set_tid(33001);
	pItem64->set_qty(3761);

	// Item 65
	auto* pItem65 = pItems->Add();
	pItem65->set_tid(1070);
	pItem65->set_qty(90);

	// Item 66
	auto* pItem66 = pItems->Add();
	pItem66->set_tid(32000);
	pItem66->set_qty(11781);

	// Item 67
	auto* pItem67 = pItems->Add();
	pItem67->set_tid(86009);
	pItem67->set_qty(1);

	// Item 68
	auto* pItem68 = pItems->Add();
	pItem68->set_tid(35002);
	pItem68->set_qty(284);

	// Item 69
	auto* pItem69 = pItems->Add();
	pItem69->set_tid(21081);
	pItem69->set_qty(67);

	// Item 70
	auto* pItem70 = pItems->Add();
	pItem70->set_tid(82001);
	pItem70->set_qty(670);

	// Item 71
	auto* pItem71 = pItems->Add();
	pItem71->set_tid(1200);
	pItem71->set_qty(90);

	// Item 72
	auto* pItem72 = pItems->Add();
	pItem72->set_tid(81009);
	pItem72->set_qty(1);
	pItem72->set_expire(1779652800);
	pItem72->set_id(659764076573890405);

	// Item 73
	auto* pItem73 = pItems->Add();
	pItem73->set_tid(81009);
	pItem73->set_qty(2);
	pItem73->set_expire(1779652800);
	pItem73->set_id(659764076573890416);

	// Item 74
	auto* pItem74 = pItems->Add();
	pItem74->set_tid(120001);
	pItem74->set_qty(38);

	// Item 75
	auto* pItem75 = pItems->Add();
	pItem75->set_tid(20013);
	pItem75->set_qty(13);

	// Item 76
	auto* pItem76 = pItems->Add();
	pItem76->set_tid(33901);
	pItem76->set_qty(4);

	// Item 77
	auto* pItem77 = pItems->Add();
	pItem77->set_tid(224042);
	pItem77->set_qty(1);

	// Item 78
	auto* pItem78 = pItems->Add();
	pItem78->set_tid(21071);
	pItem78->set_qty(334);

	// Item 79
	auto* pItem79 = pItems->Add();
	pItem79->set_tid(21083);
	pItem79->set_qty(8);

	// Item 80
	auto* pItem80 = pItems->Add();
	pItem80->set_tid(20082);
	pItem80->set_qty(29);

	// Item 81
	auto* pItem81 = pItems->Add();
	pItem81->set_tid(21013);
	pItem81->set_qty(1);

	// Item 82
	auto* pItem82 = pItems->Add();
	pItem82->set_tid(86002);
	pItem82->set_qty(1);

	// Item 83
	auto* pItem83 = pItems->Add();
	pItem83->set_tid(20053);
	pItem83->set_qty(7);

	// Item 84
	auto* pItem84 = pItems->Add();
	pItem84->set_tid(1270);
	pItem84->set_qty(30);

	// Item 85
	auto* pItem85 = pItems->Add();
	pItem85->set_tid(32013);
	pItem85->set_qty(70);

	// Item 86
	auto* pItem86 = pItems->Add();
	pItem86->set_tid(81009);
	pItem86->set_qty(4);
	pItem86->set_expire(1779652800);
	pItem86->set_id(659764076573890431);

	// Item 87
	auto* pItem87 = pItems->Add();
	pItem87->set_tid(21082);
	pItem87->set_qty(10);

	// Item 88
	auto* pItem88 = pItems->Add();
	pItem88->set_tid(33063);
	pItem88->set_qty(1);

	// Item 89
	auto* pItem89 = pItems->Add();
	pItem89->set_tid(602);
	pItem89->set_qty(16860);

	// Item 90
	auto* pItem90 = pItems->Add();
	pItem90->set_tid(33042);
	pItem90->set_qty(6);

	// Item 91
	auto* pItem91 = pItems->Add();
	pItem91->set_tid(32021);
	pItem91->set_qty(129);

	// Item 92
	auto* pItem92 = pItems->Add();
	pItem92->set_tid(21043);
	pItem92->set_qty(12);

	// Item 93
	auto* pItem93 = pItems->Add();
	pItem93->set_tid(20083);
	pItem93->set_qty(1);

	// Item 94
	auto* pItem94 = pItems->Add();
	pItem94->set_tid(1160);
	pItem94->set_qty(120);

	// Item 95
	auto* pItem95 = pItems->Add();
	pItem95->set_tid(81009);
	pItem95->set_qty(2);
	pItem95->set_expire(1779652800);
	pItem95->set_id(659764076573890411);

	// Item 96
	auto* pItem96 = pItems->Add();
	pItem96->set_tid(81010);
	pItem96->set_qty(1);
	pItem96->set_expire(1779652800);
	pItem96->set_id(659764076573890440);

	// Item 97
	auto* pItem97 = pItems->Add();
	pItem97->set_tid(222010);
	pItem97->set_qty(3);

	// Item 98
	auto* pItem98 = pItems->Add();
	pItem98->set_tid(21003);
	pItem98->set_qty(4);

	// Item 99
	auto* pItem99 = pItems->Add();
	pItem99->set_tid(35010);
	pItem99->set_qty(1);


	// === LastRead ===
	auto* pLastRead = info.mutable_lastread();
	pLastRead->set_type(2);

	auto* pActivityStory = pLastRead->mutable_activitystory();
	pActivityStory->set_chapterid(10106);
	pActivityStory->set_storyid(101060201);

	// === Phone ===
	auto* pPhone = info.mutable_phone();
	pPhone->set_newmessage(1);





	// === Quests ===
	auto* pQuests = info.mutable_quests();

	// Daily Quests
	// Quest 1003
	auto* pQuest1 = pQuests->add_list();
	pQuest1->set_id(1003);
	pQuest1->set_type(Daily);
	pQuest1->set_expire(1779134400);
	auto* pProgress1 = pQuest1->add_progress();
	pProgress1->set_max(5);

	// Quest 1004
	auto* pQuest2 = pQuests->add_list();
	pQuest2->set_id(1004);
	pQuest2->set_type(Daily);
	pQuest2->set_expire(1779134400);
	auto* pProgress2 = pQuest2->add_progress();
	pProgress2->set_max(100);

	// Quest 1005
	auto* pQuest3 = pQuests->add_list();
	pQuest3->set_id(1005);
	pQuest3->set_type(Daily);
	pQuest3->set_expire(1779134400);
	auto* pProgress3 = pQuest3->add_progress();
	pProgress3->set_max(1);

	// Quest 1006
	auto* pQuest4 = pQuests->add_list();
	pQuest4->set_id(1006);
	pQuest4->set_type(Daily);
	pQuest4->set_expire(1779134400);
	auto* pProgress4 = pQuest4->add_progress();
	pProgress4->set_max(1);

	// Quest 1009
	auto* pQuest5 = pQuests->add_list();
	pQuest5->set_id(1009);
	pQuest5->set_type(Daily);
	pQuest5->set_expire(1779134400);
	auto* pProgress5 = pQuest5->add_progress();
	pProgress5->set_max(1);

	// Quest 1010
	auto* pQuest6 = pQuests->add_list();
	pQuest6->set_id(1010);
	pQuest6->set_type(Daily);
	pQuest6->set_expire(1779134400);
	auto* pProgress6 = pQuest6->add_progress();
	pProgress6->set_max(1);

	// Quest 1007
	auto* pQuest7 = pQuests->add_list();
	pQuest7->set_id(1007);
	pQuest7->set_type(Daily);
	pQuest7->set_expire(1779134400);
	auto* pProgress7 = pQuest7->add_progress();
	pProgress7->set_max(1);

	// Quest 2002
	auto* pQuest8 = pQuests->add_list();
	pQuest8->set_id(2002);
	pQuest8->set_type(Daily);
	pQuest8->set_expire(1779134400);
	auto* pProgress8 = pQuest8->add_progress();
	pProgress8->set_max(1);

	// Quest 1001
	auto* pQuest9 = pQuests->add_list();
	pQuest9->set_id(1001);
	pQuest9->set_type(Daily);
	pQuest9->set_expire(1779134400);
	pQuest9->set_status(1);
	auto* pProgress9 = pQuest9->add_progress();
	pProgress9->set_cur(1);
	pProgress9->set_max(1);

	// Quest 1008
	auto* pQuest10 = pQuests->add_list();
	pQuest10->set_id(1008);
	pQuest10->set_type(Daily);
	pQuest10->set_expire(1779134400);
	auto* pProgress10 = pQuest10->add_progress();
	pProgress10->set_max(1);

	// Quest 1002
	auto* pQuest11 = pQuests->add_list();
	pQuest11->set_id(1002);
	pQuest11->set_type(Daily);
	pQuest11->set_expire(1779134400);
	auto* pProgress11 = pQuest11->add_progress();
	pProgress11->set_max(1);

	// Quest 2005
	auto* pQuest12 = pQuests->add_list();
	pQuest12->set_id(2005);
	pQuest12->set_type(Daily);
	pQuest12->set_expire(1779134400);
	auto* pProgress12 = pQuest12->add_progress();
	pProgress12->set_max(1);

	// Weekly Quests
	// Quest 1009
	auto* pQuest13 = pQuests->add_list();
	pQuest13->set_id(1009);
	pQuest13->set_type(Weekly);
	pQuest13->set_expire(1779652800);
	auto* pProgress13 = pQuest13->add_progress();
	pProgress13->set_max(1000);

	// Quest 1008
	auto* pQuest14 = pQuests->add_list();
	pQuest14->set_id(1008);
	pQuest14->set_type(Weekly);
	pQuest14->set_expire(1779652800);
	auto* pProgress14 = pQuest14->add_progress();
	pProgress14->set_max(500);

	// Quest 1011
	auto* pQuest15 = pQuests->add_list();
	pQuest15->set_id(1011);
	pQuest15->set_type(Weekly);
	pQuest15->set_expire(1779652800);
	auto* pProgress15 = pQuest15->add_progress();
	pProgress15->set_max(5);

	// Quest 1001
	auto* pQuest16 = pQuests->add_list();
	pQuest16->set_id(1001);
	pQuest16->set_type(Weekly);
	pQuest16->set_expire(1779652800);
	pQuest16->set_status(1);
	auto* pProgress16 = pQuest16->add_progress();
	pProgress16->set_cur(1);
	pProgress16->set_max(1);

	// Quest 1002
	auto* pQuest17 = pQuests->add_list();
	pQuest17->set_id(1002);
	pQuest17->set_type(Weekly);
	pQuest17->set_expire(1779652800);
	auto* pProgress17 = pQuest17->add_progress();
	pProgress17->set_cur(1);
	pProgress17->set_max(5);

	// Quest 1007
	auto* pQuest18 = pQuests->add_list();
	pQuest18->set_id(1007);
	pQuest18->set_type(Weekly);
	pQuest18->set_expire(1779652800);
	auto* pProgress18 = pQuest18->add_progress();
	pProgress18->set_max(40);

	// Quest 1012
	auto* pQuest19 = pQuests->add_list();
	pQuest19->set_id(1012);
	pQuest19->set_type(Weekly);
	pQuest19->set_expire(1779652800);
	auto* pProgress19 = pQuest19->add_progress();
	pProgress19->set_max(10);

	// Quest 1006
	auto* pQuest20 = pQuests->add_list();
	pQuest20->set_id(1006);
	pQuest20->set_type(Weekly);
	pQuest20->set_expire(1779652800);
	auto* pProgress20 = pQuest20->add_progress();
	pProgress20->set_max(20);

	// Quest 1013
	auto* pQuest21 = pQuests->add_list();
	pQuest21->set_id(1013);
	pQuest21->set_type(Weekly);
	pQuest21->set_expire(1779652800);
	auto* pProgress21 = pQuest21->add_progress();
	pProgress21->set_max(3);

	// Quest 1010
	auto* pQuest22 = pQuests->add_list();
	pQuest22->set_id(1010);
	pQuest22->set_type(Weekly);
	pQuest22->set_expire(1779652800);
	auto* pProgress22 = pQuest22->add_progress();
	pProgress22->set_max(5);

	// Quest 1005
	auto* pQuest23 = pQuests->add_list();
	pQuest23->set_id(1005);
	pQuest23->set_type(Weekly);
	pQuest23->set_expire(1779652800);
	auto* pProgress23 = pQuest23->add_progress();
	pProgress23->set_max(10);

	// Quest 1004
	auto* pQuest24 = pQuests->add_list();
	pQuest24->set_id(1004);
	pQuest24->set_type(Weekly);
	pQuest24->set_expire(1779652800);
	auto* pProgress24 = pQuest24->add_progress();
	pProgress24->set_max(5);

	// Quest 1003
	auto* pQuest25 = pQuests->add_list();
	pQuest25->set_id(1003);
	pQuest25->set_type(Weekly);
	pQuest25->set_expire(1779652800);
	auto* pProgress25 = pQuest25->add_progress();
	pProgress25->set_max(3);

	// TourGuide Quests
	// Quest 4001
	auto* pQuest26 = pQuests->add_list();
	pQuest26->set_id(4001);
	pQuest26->set_type(TourGuide);
	pQuest26->set_status(2);
	auto* pProgress26 = pQuest26->add_progress();
	pProgress26->set_cur(1);
	pProgress26->set_max(1);

	// Quest 4002
	auto* pQuest27 = pQuests->add_list();
	pQuest27->set_id(4002);
	pQuest27->set_type(TourGuide);
	pQuest27->set_status(2);
	auto* pProgress27 = pQuest27->add_progress();
	pProgress27->set_cur(4);
	pProgress27->set_max(4);

	// Quest 4003
	auto* pQuest28 = pQuests->add_list();
	pQuest28->set_id(4003);
	pQuest28->set_type(TourGuide);
	pQuest28->set_status(2);
	auto* pProgress28 = pQuest28->add_progress();
	pProgress28->set_cur(5);
	pProgress28->set_max(5);

	// Quest 4004
	auto* pQuest29 = pQuests->add_list();
	pQuest29->set_id(4004);
	pQuest29->set_type(TourGuide);
	pQuest29->set_status(2);
	auto* pProgress29 = pQuest29->add_progress();
	pProgress29->set_cur(4);
	pProgress29->set_max(4);

	// Quest 4005
	auto* pQuest30 = pQuests->add_list();
	pQuest30->set_id(4005);
	pQuest30->set_type(TourGuide);
	pQuest30->set_status(2);
	auto* pProgress30 = pQuest30->add_progress();
	pProgress30->set_cur(1);
	pProgress30->set_max(1);

	// Quest 4006
	auto* pQuest31 = pQuests->add_list();
	pQuest31->set_id(4006);
	pQuest31->set_type(TourGuide);
	pQuest31->set_status(2);
	auto* pProgress31 = pQuest31->add_progress();
	pProgress31->set_cur(15);
	pProgress31->set_max(15);

	// Quest 4007
	auto* pQuest32 = pQuests->add_list();
	pQuest32->set_id(4007);
	pQuest32->set_type(TourGuide);
	pQuest32->set_status(2);
	auto* pProgress32 = pQuest32->add_progress();
	pProgress32->set_cur(6);
	pProgress32->set_max(6);

	// Quest 4008
	auto* pQuest33 = pQuests->add_list();
	pQuest33->set_id(4008);
	pQuest33->set_type(TourGuide);
	pQuest33->set_status(2);
	auto* pProgress33 = pQuest33->add_progress();
	pProgress33->set_cur(50);
	pProgress33->set_max(50);

	// Quest 4009
	auto* pQuest34 = pQuests->add_list();
	pQuest34->set_id(4009);
	pQuest34->set_type(TourGuide);
	pQuest34->set_status(2);
	auto* pProgress34 = pQuest34->add_progress();
	pProgress34->set_cur(3);
	pProgress34->set_max(3);

	// Quest 4010
	auto* pQuest35 = pQuests->add_list();
	pQuest35->set_id(4010);
	pQuest35->set_type(TourGuide);
	pQuest35->set_status(2);
	auto* pProgress35 = pQuest35->add_progress();
	pProgress35->set_cur(1);
	pProgress35->set_max(1);

	// Quest 4011
	auto* pQuest36 = pQuests->add_list();
	pQuest36->set_id(4011);
	pQuest36->set_type(TourGuide);
	auto* pProgress36 = pQuest36->add_progress();
	pProgress36->set_cur(1);
	pProgress36->set_max(2);

	// Assist Quests (Type: Assist) - Partial due to length, continuing...
	// Quest 100101
	auto* pQuest37 = pQuests->add_list();
	pQuest37->set_id(100101);
	pQuest37->set_type(Assist);
	pQuest37->set_status(2);
	auto* pProgress37 = pQuest37->add_progress();
	pProgress37->set_cur(1);
	pProgress37->set_max(1);

	// Quest 100102
	auto* pQuest38 = pQuests->add_list();
	pQuest38->set_id(100102);
	pQuest38->set_type(Assist);
	pQuest38->set_status(2);
	auto* pProgress38 = pQuest38->add_progress();
	pProgress38->set_cur(10);
	pProgress38->set_max(10);

	// Quest 100103
	auto* pQuest39 = pQuests->add_list();
	pQuest39->set_id(100103);
	pQuest39->set_type(Assist);
	pQuest39->set_status(2);
	auto* pProgress39 = pQuest39->add_progress();
	pProgress39->set_cur(1);
	pProgress39->set_max(1);

	// Quest 100104
	auto* pQuest40 = pQuests->add_list();
	pQuest40->set_id(100104);
	pQuest40->set_type(Assist);
	auto* pProgress40 = pQuest40->add_progress();
	pProgress40->set_cur(1);
	pProgress40->set_max(10);

	// Quest 100105
	auto* pQuest41 = pQuests->add_list();
	pQuest41->set_id(100105);
	pQuest41->set_type(Assist);
	pQuest41->set_status(2);
	auto* pProgress41 = pQuest41->add_progress();
	pProgress41->set_cur(1);
	pProgress41->set_max(1);

	// Quest 100201
	auto* pQuest42 = pQuests->add_list();
	pQuest42->set_id(100201);
	pQuest42->set_type(Assist);
	auto* pProgress42 = pQuest42->add_progress();
	pProgress42->set_cur(1);
	pProgress42->set_max(2);

	// Quest 100202
	auto* pQuest43 = pQuests->add_list();
	pQuest43->set_id(100202);
	pQuest43->set_type(Assist);
	pQuest43->set_status(1);
	auto* pProgress43 = pQuest43->add_progress();
	pProgress43->set_cur(1);
	pProgress43->set_max(1);

	// Quest 100203
	auto* pQuest44 = pQuests->add_list();
	pQuest44->set_id(100203);
	pQuest44->set_type(Assist);
	auto* pProgress44 = pQuest44->add_progress();
	pProgress44->set_max(1);

	// Quest 100204
	auto* pQuest45 = pQuests->add_list();
	pQuest45->set_id(100204);
	pQuest45->set_type(Assist);
	pQuest45->set_status(1);
	auto* pProgress45 = pQuest45->add_progress();
	pProgress45->set_cur(1);
	pProgress45->set_max(1);

	// Quest 100205
	auto* pQuest46 = pQuests->add_list();
	pQuest46->set_id(100205);
	pQuest46->set_type(Assist);
	pQuest46->set_status(1);
	auto* pProgress46 = pQuest46->add_progress();
	pProgress46->set_cur(1);
	pProgress46->set_max(1);

	// Quest 100206
	auto* pQuest47 = pQuests->add_list();
	pQuest47->set_id(100206);
	pQuest47->set_type(Assist);
	pQuest47->set_status(1);
	auto* pProgress47 = pQuest47->add_progress();
	pProgress47->set_cur(20);
	pProgress47->set_max(20);

	// Quest 100207
	auto* pQuest48 = pQuests->add_list();
	pQuest48->set_id(100207);
	pQuest48->set_type(Assist);
	auto* pProgress48 = pQuest48->add_progress();
	pProgress48->set_cur(1);
	pProgress48->set_max(20);

	// Quest 100208
	auto* pQuest49 = pQuests->add_list();
	pQuest49->set_id(100208);
	pQuest49->set_type(Assist);
	auto* pProgress49 = pQuest49->add_progress();
	pProgress49->set_cur(1);
	pProgress49->set_max(20);

	// Quest 100301
	auto* pQuest50 = pQuests->add_list();
	pQuest50->set_id(100301);
	pQuest50->set_type(Assist);
	auto* pProgress50 = pQuest50->add_progress();
	pProgress50->set_max(1);

	// Quest 100302
	auto* pQuest51 = pQuests->add_list();
	pQuest51->set_id(100302);
	pQuest51->set_type(Assist);
	auto* pProgress51 = pQuest51->add_progress();
	pProgress51->set_cur(1);
	pProgress51->set_max(4);

	// Quest 100303
	auto* pQuest52 = pQuests->add_list();
	pQuest52->set_id(100303);
	pQuest52->set_type(Assist);
	pQuest52->set_status(1);
	auto* pProgress52 = pQuest52->add_progress();
	pProgress52->set_cur(3);
	pProgress52->set_max(3);

	// Quest 100304
	auto* pQuest53 = pQuests->add_list();
	pQuest53->set_id(100304);
	pQuest53->set_type(Assist);
	pQuest53->set_status(1);
	auto* pProgress53 = pQuest53->add_progress();
	pProgress53->set_cur(1);
	pProgress53->set_max(1);

	// Quest 100305
	auto* pQuest54 = pQuests->add_list();
	pQuest54->set_id(100305);
	pQuest54->set_type(Assist);
	auto* pProgress54 = pQuest54->add_progress();
	pProgress54->set_max(1);

	// Quest 100306
	auto* pQuest55 = pQuests->add_list();
	pQuest55->set_id(100306);
	pQuest55->set_type(Assist);
	auto* pProgress55 = pQuest55->add_progress();
	pProgress55->set_max(1);

	// Quest 100307
	auto* pQuest56 = pQuests->add_list();
	pQuest56->set_id(100307);
	pQuest56->set_type(Assist);
	pQuest56->set_status(1);
	auto* pProgress56 = pQuest56->add_progress();
	pProgress56->set_cur(3);
	pProgress56->set_max(3);

	// Quest 100308
	auto* pQuest57 = pQuests->add_list();
	pQuest57->set_id(100308);
	pQuest57->set_type(Assist);
	auto* pProgress57 = pQuest57->add_progress();
	pProgress57->set_cur(1);
	pProgress57->set_max(3);

	// Quest 100309
	auto* pQuest58 = pQuests->add_list();
	pQuest58->set_id(100309);
	pQuest58->set_type(Assist);
	pQuest58->set_status(1);
	auto* pProgress58 = pQuest58->add_progress();
	pProgress58->set_cur(1);
	pProgress58->set_max(1);

	// Quest 100310
	auto* pQuest59 = pQuests->add_list();
	pQuest59->set_id(100310);
	pQuest59->set_type(Assist);
	auto* pProgress59 = pQuest59->add_progress();
	pProgress59->set_max(1);

	// Quest 100401
	auto* pQuest60 = pQuests->add_list();
	pQuest60->set_id(100401);
	pQuest60->set_type(Assist);
	auto* pProgress60 = pQuest60->add_progress();
	pProgress60->set_cur(1);
	pProgress60->set_max(6);

	// Quest 100402
	auto* pQuest61 = pQuests->add_list();
	pQuest61->set_id(100402);
	pQuest61->set_type(Assist);
	pQuest61->set_status(1);
	auto* pProgress61 = pQuest61->add_progress();
	pProgress61->set_cur(1);
	pProgress61->set_max(1);

	// Quest 100403
	auto* pQuest62 = pQuests->add_list();
	pQuest62->set_id(100403);
	pQuest62->set_type(Assist);
	auto* pProgress62 = pQuest62->add_progress();
	pProgress62->set_max(1);

	// Quest 100404
	auto* pQuest63 = pQuests->add_list();
	pQuest63->set_id(100404);
	pQuest63->set_type(Assist);
	auto* pProgress63 = pQuest63->add_progress();
	pProgress63->set_max(1);

	// Quest 100405
	auto* pQuest64 = pQuests->add_list();
	pQuest64->set_id(100405);
	pQuest64->set_type(Assist);
	pQuest64->set_status(1);
	auto* pProgress64 = pQuest64->add_progress();
	pProgress64->set_cur(3);
	pProgress64->set_max(3);

	// Quest 100406
	auto* pQuest65 = pQuests->add_list();
	pQuest65->set_id(100406);
	pQuest65->set_type(Assist);
	auto* pProgress65 = pQuest65->add_progress();
	pProgress65->set_cur(1);
	pProgress65->set_max(3);

	// Quest 100407
	auto* pQuest66 = pQuests->add_list();
	pQuest66->set_id(100407);
	pQuest66->set_type(Assist);
	auto* pProgress66 = pQuest66->add_progress();
	pProgress66->set_max(1);

	// Quest 100408
	auto* pQuest67 = pQuests->add_list();
	pQuest67->set_id(100408);
	pQuest67->set_type(Assist);
	auto* pProgress67 = pQuest67->add_progress();
	pProgress67->set_max(1);

	// Quest 100501
	auto* pQuest68 = pQuests->add_list();
	pQuest68->set_id(100501);
	pQuest68->set_type(Assist);
	auto* pProgress68 = pQuest68->add_progress();
	pProgress68->set_cur(1);
	pProgress68->set_max(8);

	// Quest 100502
	auto* pQuest69 = pQuests->add_list();
	pQuest69->set_id(100502);
	pQuest69->set_type(Assist);
	pQuest69->set_status(1);
	auto* pProgress69 = pQuest69->add_progress();
	pProgress69->set_cur(1);
	pProgress69->set_max(1);

	// Quest 100503
	auto* pQuest70 = pQuests->add_list();
	pQuest70->set_id(100503);
	pQuest70->set_type(Assist);
	auto* pProgress70 = pQuest70->add_progress();
	pProgress70->set_max(1);

	// Quest 100504
	auto* pQuest71 = pQuests->add_list();
	pQuest71->set_id(100504);
	pQuest71->set_type(Assist);
	auto* pProgress71 = pQuest71->add_progress();
	pProgress71->set_max(1);

	// Quest 100505
	auto* pQuest72 = pQuests->add_list();
	pQuest72->set_id(100505);
	pQuest72->set_type(Assist);
	pQuest72->set_status(1);
	auto* pProgress72 = pQuest72->add_progress();
	pProgress72->set_cur(3);
	pProgress72->set_max(3);

	// Quest 100506
	auto* pQuest73 = pQuests->add_list();
	pQuest73->set_id(100506);
	pQuest73->set_type(Assist);
	auto* pProgress73 = pQuest73->add_progress();
	pProgress73->set_cur(1);
	pProgress73->set_max(3);

	// Quest 100507
	auto* pQuest74 = pQuests->add_list();
	pQuest74->set_id(100507);
	pQuest74->set_type(Assist);
	auto* pProgress74 = pQuest74->add_progress();
	pProgress74->set_max(1);

	// Quest 100508
	auto* pQuest75 = pQuests->add_list();
	pQuest75->set_id(100508);
	pQuest75->set_type(Assist);
	auto* pProgress75 = pQuest75->add_progress();
	pProgress75->set_max(1);

	// Quest 200101
	auto* pQuest76 = pQuests->add_list();
	pQuest76->set_id(200101);
	pQuest76->set_type(Assist);
	pQuest76->set_status(1);
	auto* pProgress76 = pQuest76->add_progress();
	pProgress76->set_cur(1);
	pProgress76->set_max(1);

	// Quest 200102
	auto* pQuest77 = pQuests->add_list();
	pQuest77->set_id(200102);
	pQuest77->set_type(Assist);
	pQuest77->set_status(1);
	auto* pProgress77 = pQuest77->add_progress();
	pProgress77->set_cur(10);
	pProgress77->set_max(10);

	// Quest 200103
	auto* pQuest78 = pQuests->add_list();
	pQuest78->set_id(200103);
	pQuest78->set_type(Assist);
	pQuest78->set_status(1);
	auto* pProgress78 = pQuest78->add_progress();
	pProgress78->set_cur(1);
	pProgress78->set_max(1);

	// Quest 200104
	auto* pQuest79 = pQuests->add_list();
	pQuest79->set_id(200104);
	pQuest79->set_type(Assist);
	pQuest79->set_status(1);
	auto* pProgress79 = pQuest79->add_progress();
	pProgress79->set_cur(10);
	pProgress79->set_max(10);

	// Quest 200105
	auto* pQuest80 = pQuests->add_list();
	pQuest80->set_id(200105);
	pQuest80->set_type(Assist);
	pQuest80->set_status(1);
	auto* pProgress80 = pQuest80->add_progress();
	pProgress80->set_cur(1);
	pProgress80->set_max(1);

	// Quest 200201
	auto* pQuest81 = pQuests->add_list();
	pQuest81->set_id(200201);
	pQuest81->set_type(Assist);
	auto* pProgress81 = pQuest81->add_progress();
	pProgress81->set_cur(1);
	pProgress81->set_max(2);

	// Quest 200202
	auto* pQuest82 = pQuests->add_list();
	pQuest82->set_id(200202);
	pQuest82->set_type(Assist);
	pQuest82->set_status(1);
	auto* pProgress82 = pQuest82->add_progress();
	pProgress82->set_cur(1);
	pProgress82->set_max(1);

	// Quest 200203
	auto* pQuest83 = pQuests->add_list();
	pQuest83->set_id(200203);
	pQuest83->set_type(Assist);
	pQuest83->set_status(1);
	auto* pProgress83 = pQuest83->add_progress();
	pProgress83->set_cur(1);
	pProgress83->set_max(1);

	// Quest 200204
	auto* pQuest84 = pQuests->add_list();
	pQuest84->set_id(200204);
	pQuest84->set_type(Assist);
	pQuest84->set_status(1);
	auto* pProgress84 = pQuest84->add_progress();
	pProgress84->set_cur(1);
	pProgress84->set_max(1);

	// Quest 200205
	auto* pQuest85 = pQuests->add_list();
	pQuest85->set_id(200205);
	pQuest85->set_type(Assist);
	auto* pProgress85 = pQuest85->add_progress();
	pProgress85->set_max(1);

	// Quest 200206
	auto* pQuest86 = pQuests->add_list();
	pQuest86->set_id(200206);
	pQuest86->set_type(Assist);
	pQuest86->set_status(1);
	auto* pProgress86 = pQuest86->add_progress();
	pProgress86->set_cur(20);
	pProgress86->set_max(20);

	// Quest 200207
	auto* pQuest87 = pQuests->add_list();
	pQuest87->set_id(200207);
	pQuest87->set_type(Assist);
	pQuest87->set_status(1);
	auto* pProgress87 = pQuest87->add_progress();
	pProgress87->set_cur(20);
	pProgress87->set_max(20);

	// Quest 200208
	auto* pQuest88 = pQuests->add_list();
	pQuest88->set_id(200208);
	pQuest88->set_type(Assist);
	auto* pProgress88 = pQuest88->add_progress();
	pProgress88->set_cur(1);
	pProgress88->set_max(20);

	// Quest 200301
	auto* pQuest89 = pQuests->add_list();
	pQuest89->set_id(200301);
	pQuest89->set_type(Assist);
	auto* pProgress89 = pQuest89->add_progress();
	pProgress89->set_max(1);

	// Quest 200302
	auto* pQuest90 = pQuests->add_list();
	pQuest90->set_id(200302);
	pQuest90->set_type(Assist);
	auto* pProgress90 = pQuest90->add_progress();
	pProgress90->set_cur(1);
	pProgress90->set_max(4);

	// Quest 200303
	auto* pQuest91 = pQuests->add_list();
	pQuest91->set_id(200303);
	pQuest91->set_type(Assist);
	auto* pProgress91 = pQuest91->add_progress();
	pProgress91->set_cur(2);
	pProgress91->set_max(3);

	// Quest 200304
	auto* pQuest92 = pQuests->add_list();
	pQuest92->set_id(200304);
	pQuest92->set_type(Assist);
	pQuest92->set_status(1);
	auto* pProgress92 = pQuest92->add_progress();
	pProgress92->set_cur(1);
	pProgress92->set_max(1);

	// Quest 200305
	auto* pQuest93 = pQuests->add_list();
	pQuest93->set_id(200305);
	pQuest93->set_type(Assist);
	pQuest93->set_status(1);
	auto* pProgress93 = pQuest93->add_progress();
	pProgress93->set_cur(1);
	pProgress93->set_max(1);

	// Quest 200306
	auto* pQuest94 = pQuests->add_list();
	pQuest94->set_id(200306);
	pQuest94->set_type(Assist);
	auto* pProgress94 = pQuest94->add_progress();
	pProgress94->set_max(1);

	// Quest 200307
	auto* pQuest95 = pQuests->add_list();
	pQuest95->set_id(200307);
	pQuest95->set_type(Assist);
	auto* pProgress95 = pQuest95->add_progress();
	pProgress95->set_cur(2);
	pProgress95->set_max(3);

	// Quest 200308
	auto* pQuest96 = pQuests->add_list();
	pQuest96->set_id(200308);
	pQuest96->set_type(Assist);
	pQuest96->set_status(1);
	auto* pProgress96 = pQuest96->add_progress();
	pProgress96->set_cur(3);
	pProgress96->set_max(3);

	// Quest 200309
	auto* pQuest97 = pQuests->add_list();
	pQuest97->set_id(200309);
	pQuest97->set_type(Assist);
	pQuest97->set_status(1);
	auto* pProgress97 = pQuest97->add_progress();
	pProgress97->set_cur(1);
	pProgress97->set_max(1);

	// Quest 200310
	auto* pQuest98 = pQuests->add_list();
	pQuest98->set_id(200310);
	pQuest98->set_type(Assist);
	pQuest98->set_status(1);
	auto* pProgress98 = pQuest98->add_progress();
	pProgress98->set_cur(1);
	pProgress98->set_max(1);

	// Quest 200401
	auto* pQuest99 = pQuests->add_list();
	pQuest99->set_id(200401);
	pQuest99->set_type(Assist);
	auto* pProgress99 = pQuest99->add_progress();
	pProgress99->set_cur(1);
	pProgress99->set_max(6);

	// Quest 200402
	auto* pQuest100 = pQuests->add_list();
	pQuest100->set_id(200402);
	pQuest100->set_type(Assist);
	pQuest100->set_status(1);
	auto* pProgress100 = pQuest100->add_progress();
	pProgress100->set_cur(1);
	pProgress100->set_max(1);

	// Quest 200403
	auto* pQuest101 = pQuests->add_list();
	pQuest101->set_id(200403);
	pQuest101->set_type(Assist);
	pQuest101->set_status(1);
	auto* pProgress101 = pQuest101->add_progress();
	pProgress101->set_cur(1);
	pProgress101->set_max(1);

	// Quest 200404
	auto* pQuest102 = pQuests->add_list();
	pQuest102->set_id(200404);
	pQuest102->set_type(Assist);
	auto* pProgress102 = pQuest102->add_progress();
	pProgress102->set_max(1);

	// Quest 200405
	auto* pQuest103 = pQuests->add_list();
	pQuest103->set_id(200405);
	pQuest103->set_type(Assist);
	auto* pProgress103 = pQuest103->add_progress();
	pProgress103->set_cur(2);
	pProgress103->set_max(3);

	// Quest 200406
	auto* pQuest104 = pQuests->add_list();
	pQuest104->set_id(200406);
	pQuest104->set_type(Assist);
	pQuest104->set_status(1);
	auto* pProgress104 = pQuest104->add_progress();
	pProgress104->set_cur(3);
	pProgress104->set_max(3);

	// Quest 200407
	auto* pQuest105 = pQuests->add_list();
	pQuest105->set_id(200407);
	pQuest105->set_type(Assist);
	pQuest105->set_status(1);
	auto* pProgress105 = pQuest105->add_progress();
	pProgress105->set_cur(1);
	pProgress105->set_max(1);

	// Quest 200408
	auto* pQuest106 = pQuests->add_list();
	pQuest106->set_id(200408);
	pQuest106->set_type(Assist);
	pQuest106->set_status(1);
	auto* pProgress106 = pQuest106->add_progress();
	pProgress106->set_cur(1);
	pProgress106->set_max(1);

	// Quest 200501
	auto* pQuest107 = pQuests->add_list();
	pQuest107->set_id(200501);
	pQuest107->set_type(Assist);
	auto* pProgress107 = pQuest107->add_progress();
	pProgress107->set_cur(1);
	pProgress107->set_max(8);

	// Quest 200502
	auto* pQuest108 = pQuests->add_list();
	pQuest108->set_id(200502);
	pQuest108->set_type(Assist);
	pQuest108->set_status(1);
	auto* pProgress108 = pQuest108->add_progress();
	pProgress108->set_cur(1);
	pProgress108->set_max(1);

	// Quest 200503
	auto* pQuest109 = pQuests->add_list();
	pQuest109->set_id(200503);
	pQuest109->set_type(Assist);
	pQuest109->set_status(1);
	auto* pProgress109 = pQuest109->add_progress();
	pProgress109->set_cur(1);
	pProgress109->set_max(1);

	// Quest 200504
	auto* pQuest110 = pQuests->add_list();
	pQuest110->set_id(200504);
	pQuest110->set_type(Assist);
	auto* pProgress110 = pQuest110->add_progress();
	pProgress110->set_max(1);

	// Quest 200505
	auto* pQuest111 = pQuests->add_list();
	pQuest111->set_id(200505);
	pQuest111->set_type(Assist);
	auto* pProgress111 = pQuest111->add_progress();
	pProgress111->set_cur(2);
	pProgress111->set_max(3);

	// Quest 200506
	auto* pQuest112 = pQuests->add_list();
	pQuest112->set_id(200506);
	pQuest112->set_type(Assist);
	pQuest112->set_status(1);
	auto* pProgress112 = pQuest112->add_progress();
	pProgress112->set_cur(3);
	pProgress112->set_max(3);

	// Quest 200507
	auto* pQuest113 = pQuests->add_list();
	pQuest113->set_id(200507);
	pQuest113->set_type(Assist);
	pQuest113->set_status(1);
	auto* pProgress113 = pQuest113->add_progress();
	pProgress113->set_cur(1);
	pProgress113->set_max(1);

	// Quest 200508
	auto* pQuest114 = pQuests->add_list();
	pQuest114->set_id(200508);
	pQuest114->set_type(Assist);
	auto* pProgress114 = pQuest114->add_progress();
	pProgress114->set_max(1);

	// Tower Quests
	// Quest 502
	auto* pQuest115 = pQuests->add_list();
	pQuest115->set_id(502);
	pQuest115->set_type(Tower);
	pQuest115->set_status(1);
	auto* pProgress115 = pQuest115->add_progress();
	pProgress115->set_cur(1);
	pProgress115->set_max(1);

	// Quest 501
	auto* pQuest116 = pQuests->add_list();
	pQuest116->set_id(501);
	pQuest116->set_type(Tower);
	pQuest116->set_status(1);
	auto* pProgress116 = pQuest116->add_progress();
	pProgress116->set_cur(8);
	pProgress116->set_max(8);

	// Quest 504
	auto* pQuest117 = pQuests->add_list();
	pQuest117->set_id(504);
	pQuest117->set_type(Tower);
	pQuest117->set_status(1);
	auto* pProgress117 = pQuest117->add_progress();
	pProgress117->set_cur(10);
	pProgress117->set_max(10);

	// Quest 500
	auto* pQuest118 = pQuests->add_list();
	pQuest118->set_id(500);
	pQuest118->set_type(Tower);
	auto* pProgress118 = pQuest118->add_progress();
	pProgress118->set_max(2);

	// Vampire Survivor Normal Quests (Type: VampireSurvivorNormal)
	// Quest 10401, 10601, 20403, 20501, 20703, 20201
	auto* pQuest119 = pQuests->add_list();
	pQuest119->set_id(10401);
	pQuest119->set_type(VampireSurvivorNormal);
	auto* pProgress119 = pQuest119->add_progress();
	pProgress119->set_max(1);

	auto* pQuest120 = pQuests->add_list();
	pQuest120->set_id(10601);
	pQuest120->set_type(VampireSurvivorNormal);
	auto* pProgress120 = pQuest120->add_progress();
	pProgress120->set_max(1);

	auto* pQuest121 = pQuests->add_list();
	pQuest121->set_id(20403);
	pQuest121->set_type(VampireSurvivorNormal);
	auto* pProgress121 = pQuest121->add_progress();
	pProgress121->set_max(1);

	auto* pQuest122 = pQuests->add_list();
	pQuest122->set_id(20501);
	pQuest122->set_type(VampireSurvivorNormal);
	auto* pProgress122 = pQuest122->add_progress();
	pProgress122->set_max(1);

	auto* pQuest123 = pQuests->add_list();
	pQuest123->set_id(20703);
	pQuest123->set_type(VampireSurvivorNormal);
	auto* pProgress123 = pQuest123->add_progress();
	pProgress123->set_max(1);

	auto* pQuest124 = pQuests->add_list();
	pQuest124->set_id(20201);
	pQuest124->set_type(VampireSurvivorNormal);
	auto* pProgress124 = pQuest124->add_progress();
	pProgress124->set_max(1);

	// Quest 10101
	auto* pQuest125 = pQuests->add_list();
	pQuest125->set_id(10101);
	pQuest125->set_type(VampireSurvivorNormal);
	pQuest125->set_status(2);
	auto* pProgress125 = pQuest125->add_progress();
	pProgress125->set_cur(1);
	pProgress125->set_max(1);

	// Quest 10301
	auto* pQuest126 = pQuests->add_list();
	pQuest126->set_id(10301);
	pQuest126->set_type(VampireSurvivorNormal);
	auto* pProgress126 = pQuest126->add_progress();
	pProgress126->set_max(1);

	// Quest 10501
	auto* pQuest127 = pQuests->add_list();
	pQuest127->set_id(10501);
	pQuest127->set_type(VampireSurvivorNormal);
	auto* pProgress127 = pQuest127->add_progress();
	pProgress127->set_max(1);

	// Quest 10602
	auto* pQuest128 = pQuests->add_list();
	pQuest128->set_id(10602);
	pQuest128->set_type(VampireSurvivorNormal);
	auto* pProgress128 = pQuest128->add_progress();
	pProgress128->set_max(1);

	// Quest 10701
	auto* pQuest129 = pQuests->add_list();
	pQuest129->set_id(10701);
	pQuest129->set_type(VampireSurvivorNormal);
	auto* pProgress129 = pQuest129->add_progress();
	pProgress129->set_max(1);

	// Quest 10702
	auto* pQuest130 = pQuests->add_list();
	pQuest130->set_id(10702);
	pQuest130->set_type(VampireSurvivorNormal);
	auto* pProgress130 = pQuest130->add_progress();
	pProgress130->set_max(1);

	// Quest 20302
	auto* pQuest131 = pQuests->add_list();
	pQuest131->set_id(20302);
	pQuest131->set_type(VampireSurvivorNormal);
	auto* pProgress131 = pQuest131->add_progress();
	pProgress131->set_max(1);

	// Quest 20103
	auto* pQuest132 = pQuests->add_list();
	pQuest132->set_id(20103);
	pQuest132->set_type(VampireSurvivorNormal);
	auto* pProgress132 = pQuest132->add_progress();
	pProgress132->set_max(1);

	// Quest 10102
	auto* pQuest133 = pQuests->add_list();
	pQuest133->set_id(10102);
	pQuest133->set_type(VampireSurvivorNormal);
	pQuest133->set_status(2);
	auto* pProgress133 = pQuest133->add_progress();
	pProgress133->set_cur(2500);
	pProgress133->set_max(2500);

	// Quest 10103
	auto* pQuest134 = pQuests->add_list();
	pQuest134->set_id(10103);
	pQuest134->set_type(VampireSurvivorNormal);
	pQuest134->set_status(2);
	auto* pProgress134 = pQuest134->add_progress();
	pProgress134->set_cur(4000);
	pProgress134->set_max(4000);

	// Quest 10402
	auto* pQuest135 = pQuests->add_list();
	pQuest135->set_id(10402);
	pQuest135->set_type(VampireSurvivorNormal);
	auto* pProgress135 = pQuest135->add_progress();
	pProgress135->set_max(1);

	// Quest 10502
	auto* pQuest136 = pQuests->add_list();
	pQuest136->set_id(10502);
	pQuest136->set_type(VampireSurvivorNormal);
	auto* pProgress136 = pQuest136->add_progress();
	pProgress136->set_max(1);

	// Quest 10202
	auto* pQuest137 = pQuests->add_list();
	pQuest137->set_id(10202);
	pQuest137->set_type(VampireSurvivorNormal);
	auto* pProgress137 = pQuest137->add_progress();
	pProgress137->set_cur(220);
	pProgress137->set_max(10000);

	// Quest 20203
	auto* pQuest138 = pQuests->add_list();
	pQuest138->set_id(20203);
	pQuest138->set_type(VampireSurvivorNormal);
	auto* pProgress138 = pQuest138->add_progress();
	pProgress138->set_max(1);

	// Quest 20402
	auto* pQuest139 = pQuests->add_list();
	pQuest139->set_id(20402);
	pQuest139->set_type(VampireSurvivorNormal);
	auto* pProgress139 = pQuest139->add_progress();
	pProgress139->set_max(1);

	// Quest 20502
	auto* pQuest140 = pQuests->add_list();
	pQuest140->set_id(20502);
	pQuest140->set_type(VampireSurvivorNormal);
	auto* pProgress140 = pQuest140->add_progress();
	pProgress140->set_max(1);

	// Quest 20701
	auto* pQuest141 = pQuests->add_list();
	pQuest141->set_id(20701);
	pQuest141->set_type(VampireSurvivorNormal);
	auto* pProgress141 = pQuest141->add_progress();
	pProgress141->set_max(1);

	// Quest 20101
	auto* pQuest142 = pQuests->add_list();
	pQuest142->set_id(20101);
	pQuest142->set_type(VampireSurvivorNormal);
	auto* pProgress142 = pQuest142->add_progress();
	pProgress142->set_max(1);

	// Quest 20301
	auto* pQuest143 = pQuests->add_list();
	pQuest143->set_id(20301);
	pQuest143->set_type(VampireSurvivorNormal);
	auto* pProgress143 = pQuest143->add_progress();
	pProgress143->set_max(1);

	// Quest 20503
	auto* pQuest144 = pQuests->add_list();
	pQuest144->set_id(20503);
	pQuest144->set_type(VampireSurvivorNormal);
	auto* pProgress144 = pQuest144->add_progress();
	pProgress144->set_max(1);

	// Quest 20603
	auto* pQuest145 = pQuests->add_list();
	pQuest145->set_id(20603);
	pQuest145->set_type(VampireSurvivorNormal);
	auto* pProgress145 = pQuest145->add_progress();
	pProgress145->set_max(1);

	// Quest 10302
	auto* pQuest146 = pQuests->add_list();
	pQuest146->set_id(10302);
	pQuest146->set_type(VampireSurvivorNormal);
	auto* pProgress146 = pQuest146->add_progress();
	pProgress146->set_max(1);

	// Quest 10303
	auto* pQuest147 = pQuests->add_list();
	pQuest147->set_id(10303);
	pQuest147->set_type(VampireSurvivorNormal);
	auto* pProgress147 = pQuest147->add_progress();
	pProgress147->set_max(1);

	// Quest 20102
	auto* pQuest148 = pQuests->add_list();
	pQuest148->set_id(20102);
	pQuest148->set_type(VampireSurvivorNormal);
	auto* pProgress148 = pQuest148->add_progress();
	pProgress148->set_max(1);

	// Quest 20303
	auto* pQuest149 = pQuests->add_list();
	pQuest149->set_id(20303);
	pQuest149->set_type(VampireSurvivorNormal);
	auto* pProgress149 = pQuest149->add_progress();
	pProgress149->set_max(1);

	// Quest 20702
	auto* pQuest150 = pQuests->add_list();
	pQuest150->set_id(20702);
	pQuest150->set_type(VampireSurvivorNormal);
	auto* pProgress150 = pQuest150->add_progress();
	pProgress150->set_max(1);

	// Quest 10201
	auto* pQuest151 = pQuests->add_list();
	pQuest151->set_id(10201);
	pQuest151->set_type(VampireSurvivorNormal);
	auto* pProgress151 = pQuest151->add_progress();
	pProgress151->set_max(1);

	// Quest 10403
	auto* pQuest152 = pQuests->add_list();
	pQuest152->set_id(10403);
	pQuest152->set_type(VampireSurvivorNormal);
	auto* pProgress152 = pQuest152->add_progress();
	pProgress152->set_max(1);

	// Quest 10603
	auto* pQuest153 = pQuests->add_list();
	pQuest153->set_id(10603);
	pQuest153->set_type(VampireSurvivorNormal);
	auto* pProgress153 = pQuest153->add_progress();
	pProgress153->set_max(1);

	// Quest 20601
	auto* pQuest154 = pQuests->add_list();
	pQuest154->set_id(20601);
	pQuest154->set_type(VampireSurvivorNormal);
	auto* pProgress154 = pQuest154->add_progress();
	pProgress154->set_max(1);

	// Quest 10203
	auto* pQuest155 = pQuests->add_list();
	pQuest155->set_id(10203);
	pQuest155->set_type(VampireSurvivorNormal);
	auto* pProgress155 = pQuest155->add_progress();
	pProgress155->set_cur(220);
	pProgress155->set_max(15000);

	// Quest 10503
	auto* pQuest156 = pQuests->add_list();
	pQuest156->set_id(10503);
	pQuest156->set_type(VampireSurvivorNormal);
	auto* pProgress156 = pQuest156->add_progress();
	pProgress156->set_max(1);

	// Quest 10703
	auto* pQuest157 = pQuests->add_list();
	pQuest157->set_id(10703);
	pQuest157->set_type(VampireSurvivorNormal);
	auto* pProgress157 = pQuest157->add_progress();
	pProgress157->set_max(1);

	// Quest 20202
	auto* pQuest158 = pQuests->add_list();
	pQuest158->set_id(20202);
	pQuest158->set_type(VampireSurvivorNormal);
	auto* pProgress158 = pQuest158->add_progress();
	pProgress158->set_max(1);

	// Quest 20401
	auto* pQuest159 = pQuests->add_list();
	pQuest159->set_id(20401);
	pQuest159->set_type(VampireSurvivorNormal);
	auto* pProgress159 = pQuest159->add_progress();
	pProgress159->set_max(1);

	// Quest 20602
	auto* pQuest160 = pQuests->add_list();
	pQuest160->set_id(20602);
	pQuest160->set_type(VampireSurvivorNormal);
	auto* pProgress160 = pQuest160->add_progress();
	pProgress160->set_max(1);

	// Vampire Survivor Season Quests (Type: VampireSurvivorSeason)
	// Quest 301
	auto* pQuest161 = pQuests->add_list();
	pQuest161->set_id(301);
	pQuest161->set_type(VampireSurvivorSeason);
	pQuest161->set_expire(1780369199);
	auto* pProgress161 = pQuest161->add_progress();
	pProgress161->set_max(10000);

	// Quest 302
	auto* pQuest162 = pQuests->add_list();
	pQuest162->set_id(302);
	pQuest162->set_type(VampireSurvivorSeason);
	pQuest162->set_expire(1780369199);
	auto* pProgress162 = pQuest162->add_progress();
	pProgress162->set_max(15000);

	// Quest 303
	auto* pQuest163 = pQuests->add_list();
	pQuest163->set_id(303);
	pQuest163->set_type(VampireSurvivorSeason);
	pQuest163->set_expire(1780369199);
	auto* pProgress163 = pQuest163->add_progress();
	pProgress163->set_max(20000);

	// Quest 304
	auto* pQuest164 = pQuests->add_list();
	pQuest164->set_id(304);
	pQuest164->set_type(VampireSurvivorSeason);
	pQuest164->set_expire(1780369199);
	auto* pProgress164 = pQuest164->add_progress();
	pProgress164->set_max(30000);

	// Quest 305
	auto* pQuest165 = pQuests->add_list();
	pQuest165->set_id(305);
	pQuest165->set_type(VampireSurvivorSeason);
	pQuest165->set_expire(1780369199);
	auto* pProgress165 = pQuest165->add_progress();
	pProgress165->set_max(40000);

	// Quest 306
	auto* pQuest166 = pQuests->add_list();
	pQuest166->set_id(306);
	pQuest166->set_type(VampireSurvivorSeason);
	pQuest166->set_expire(1780369199);
	auto* pProgress166 = pQuest166->add_progress();
	pProgress166->set_max(50000);

	// Quest 307
	auto* pQuest167 = pQuests->add_list();
	pQuest167->set_id(307);
	pQuest167->set_type(VampireSurvivorSeason);
	pQuest167->set_expire(1780369199);
	auto* pProgress167 = pQuest167->add_progress();
	pProgress167->set_max(60000);

	// Quest 31001
	auto* pQuest168 = pQuests->add_list();
	pQuest168->set_id(31001);
	pQuest168->set_type(VampireSurvivorSeason);
	pQuest168->set_expire(1780369199);
	auto* pProgress168 = pQuest168->add_progress();
	pProgress168->set_max(1);

	// Quest 31002
	auto* pQuest169 = pQuests->add_list();
	pQuest169->set_id(31002);
	pQuest169->set_type(VampireSurvivorSeason);
	pQuest169->set_expire(1780369199);
	auto* pProgress169 = pQuest169->add_progress();
	pProgress169->set_max(1);

	// Quest 31003
	auto* pQuest170 = pQuests->add_list();
	pQuest170->set_id(31003);
	pQuest170->set_type(VampireSurvivorSeason);
	pQuest170->set_expire(1780369199);
	auto* pProgress170 = pQuest170->add_progress();
	pProgress170->set_max(1);

	// Quest 31004
	auto* pQuest171 = pQuests->add_list();
	pQuest171->set_id(31004);
	pQuest171->set_type(VampireSurvivorSeason);
	pQuest171->set_expire(1780369199);
	auto* pProgress171 = pQuest171->add_progress();
	pProgress171->set_max(1);

	// Quest 31005
	auto* pQuest172 = pQuests->add_list();
	pQuest172->set_id(31005);
	pQuest172->set_type(VampireSurvivorSeason);
	pQuest172->set_expire(1780369199);
	auto* pProgress172 = pQuest172->add_progress();
	pProgress172->set_max(1);

	// TowerEvent Quests (Type: TowerEvent) - Partial due to length, continuing with remaining...
	// Note: Due to the large number of TowerEvent quests, I'll continue with the pattern
	auto* pQuest173 = pQuests->add_list();
	pQuest173->set_id(11802);
	pQuest173->set_type(TowerEvent);
	auto* pProgress173 = pQuest173->add_progress();
	pProgress173->set_max(2);

	auto* pQuest174 = pQuests->add_list();
	pQuest174->set_id(11803);
	pQuest174->set_type(TowerEvent);
	auto* pProgress174 = pQuest174->add_progress();
	pProgress174->set_max(3);

	auto* pQuest175 = pQuests->add_list();
	pQuest175->set_id(11003);
	pQuest175->set_type(TowerEvent);
	auto* pProgress175 = pQuest175->add_progress();
	pProgress175->set_max(3);

	auto* pQuest176 = pQuests->add_list();
	pQuest176->set_id(11801);
	pQuest176->set_type(TowerEvent);
	auto* pProgress176 = pQuest176->add_progress();
	pProgress176->set_max(1);

	auto* pQuest177 = pQuests->add_list();
	pQuest177->set_id(13403);
	pQuest177->set_type(TowerEvent);
	pQuest177->set_status(1);
	auto* pProgress177 = pQuest177->add_progress();
	pProgress177->set_cur(3);
	pProgress177->set_max(3);

	auto* pQuest178 = pQuests->add_list();
	pQuest178->set_id(14101);
	pQuest178->set_type(TowerEvent);
	auto* pProgress178 = pQuest178->add_progress();
	pProgress178->set_max(1);

	auto* pQuest179 = pQuests->add_list();
	pQuest179->set_id(15801);
	pQuest179->set_type(TowerEvent);
	pQuest179->set_status(1);
	auto* pProgress179 = pQuest179->add_progress();
	pProgress179->set_cur(1);
	pProgress179->set_max(1);

	auto* pQuest180 = pQuests->add_list();
	pQuest180->set_id(15901);
	pQuest180->set_type(TowerEvent);
	auto* pProgress180 = pQuest180->add_progress();
	pProgress180->set_max(1);

	auto* pQuest181 = pQuests->add_list();
	pQuest181->set_id(12703);
	pQuest181->set_type(TowerEvent);
	auto* pProgress181 = pQuest181->add_progress();
	pProgress181->set_max(3);

	auto* pQuest182 = pQuests->add_list();
	pQuest182->set_id(14401);
	pQuest182->set_type(TowerEvent);
	auto* pProgress182 = pQuest182->add_progress();
	pProgress182->set_max(1);

	auto* pQuest183 = pQuests->add_list();
	pQuest183->set_id(14703);
	pQuest183->set_type(TowerEvent);
	auto* pProgress183 = pQuest183->add_progress();
	pProgress183->set_max(3);

	auto* pQuest184 = pQuests->add_list();
	pQuest184->set_id(917402);
	pQuest184->set_type(TowerEvent);
	auto* pProgress184 = pQuest184->add_progress();
	pProgress184->set_cur(7);
	pProgress184->set_max(10);

	auto* pQuest185 = pQuests->add_list();
	pQuest185->set_id(11402);
	pQuest185->set_type(TowerEvent);
	auto* pProgress185 = pQuest185->add_progress();
	pProgress185->set_max(2);

	auto* pQuest186 = pQuests->add_list();
	pQuest186->set_id(13402);
	pQuest186->set_type(TowerEvent);
	pQuest186->set_status(1);
	auto* pProgress186 = pQuest186->add_progress();
	pProgress186->set_cur(2);
	pProgress186->set_max(2);

	auto* pQuest187 = pQuests->add_list();
	pQuest187->set_id(15603);
	pQuest187->set_type(TowerEvent);
	auto* pProgress187 = pQuest187->add_progress();
	pProgress187->set_max(3);

	auto* pQuest188 = pQuests->add_list();
	pQuest188->set_id(11302);
	pQuest188->set_type(TowerEvent);
	auto* pProgress188 = pQuest188->add_progress();
	pProgress188->set_max(2);

	auto* pQuest189 = pQuests->add_list();
	pQuest189->set_id(12702);
	pQuest189->set_type(TowerEvent);
	auto* pProgress189 = pQuest189->add_progress();
	pProgress189->set_max(2);

	auto* pQuest190 = pQuests->add_list();
	pQuest190->set_id(13502);
	pQuest190->set_type(TowerEvent);
	auto* pProgress190 = pQuest190->add_progress();
	pProgress190->set_max(2);

	auto* pQuest191 = pQuests->add_list();
	pQuest191->set_id(14403);
	pQuest191->set_type(TowerEvent);
	auto* pProgress191 = pQuest191->add_progress();
	pProgress191->set_max(3);

	auto* pQuest192 = pQuests->add_list();
	pQuest192->set_id(917302);
	pQuest192->set_type(TowerEvent);
	auto* pProgress192 = pQuest192->add_progress();
	pProgress192->set_cur(8);
	pProgress192->set_max(10);

	auto* pQuest193 = pQuests->add_list();
	pQuest193->set_id(11001);
	pQuest193->set_type(TowerEvent);
	auto* pProgress193 = pQuest193->add_progress();
	pProgress193->set_max(1);

	auto* pQuest194 = pQuests->add_list();
	pQuest194->set_id(12002);
	pQuest194->set_type(TowerEvent);
	auto* pProgress194 = pQuest194->add_progress();
	pProgress194->set_max(2);

	auto* pQuest195 = pQuests->add_list();
	pQuest195->set_id(14301);
	pQuest195->set_type(TowerEvent);
	auto* pProgress195 = pQuest195->add_progress();
	pProgress195->set_max(1);

	auto* pQuest196 = pQuests->add_list();
	pQuest196->set_id(14303);
	pQuest196->set_type(TowerEvent);
	auto* pProgress196 = pQuest196->add_progress();
	pProgress196->set_max(3);

	auto* pQuest197 = pQuests->add_list();
	pQuest197->set_id(913303);
	pQuest197->set_type(TowerEvent);
	auto* pProgress197 = pQuest197->add_progress();
	pProgress197->set_cur(6);
	pProgress197->set_max(20);

	auto* pQuest198 = pQuests->add_list();
	pQuest198->set_id(15802);
	pQuest198->set_type(TowerEvent);
	pQuest198->set_status(1);
	auto* pProgress198 = pQuest198->add_progress();
	pProgress198->set_cur(2);
	pProgress198->set_max(2);

	auto* pQuest199 = pQuests->add_list();
	pQuest199->set_id(14503);
	pQuest199->set_type(TowerEvent);
	auto* pProgress199 = pQuest199->add_progress();
	pProgress199->set_max(3);

	auto* pQuest200 = pQuests->add_list();
	pQuest200->set_id(10802);
	pQuest200->set_type(TowerEvent);
	auto* pProgress200 = pQuest200->add_progress();
	pProgress200->set_max(2);

	// Continuing due to length, I'll add more
	auto* pQuest201 = pQuests->add_list();
	pQuest201->set_id(12501);
	pQuest201->set_type(TowerEvent);
	auto* pProgress201 = pQuest201->add_progress();
	pProgress201->set_max(1);

	auto* pQuest202 = pQuests->add_list();
	pQuest202->set_id(13003);
	pQuest202->set_type(TowerEvent);
	auto* pProgress202 = pQuest202->add_progress();
	pProgress202->set_max(3);

	auto* pQuest203 = pQuests->add_list();
	pQuest203->set_id(917202);
	pQuest203->set_type(TowerEvent);
	auto* pProgress203 = pQuest203->add_progress();
	pProgress203->set_cur(5);
	pProgress203->set_max(10);

	auto* pQuest204 = pQuests->add_list();
	pQuest204->set_id(11002);
	pQuest204->set_type(TowerEvent);
	auto* pProgress204 = pQuest204->add_progress();
	pProgress204->set_max(2);

	auto* pQuest205 = pQuests->add_list();
	pQuest205->set_id(11901);
	pQuest205->set_type(TowerEvent);
	auto* pProgress205 = pQuest205->add_progress();
	pProgress205->set_max(1);

	auto* pQuest206 = pQuests->add_list();
	pQuest206->set_id(14103);
	pQuest206->set_type(TowerEvent);
	auto* pProgress206 = pQuest206->add_progress();
	pProgress206->set_max(3);

	auto* pQuest207 = pQuests->add_list();
	pQuest207->set_id(14201);
	pQuest207->set_type(TowerEvent);
	auto* pProgress207 = pQuest207->add_progress();
	pProgress207->set_max(1);

	auto* pQuest208 = pQuests->add_list();
	pQuest208->set_id(14402);
	pQuest208->set_type(TowerEvent);
	auto* pProgress208 = pQuest208->add_progress();
	pProgress208->set_max(2);

	auto* pQuest209 = pQuests->add_list();
	pQuest209->set_id(14901);
	pQuest209->set_type(TowerEvent);
	auto* pProgress209 = pQuest209->add_progress();
	pProgress209->set_max(1);

	auto* pQuest210 = pQuests->add_list();
	pQuest210->set_id(10801);
	pQuest210->set_type(TowerEvent);
	auto* pProgress210 = pQuest210->add_progress();
	pProgress210->set_max(1);

	auto* pQuest211 = pQuests->add_list();
	pQuest211->set_id(12001);
	pQuest211->set_type(TowerEvent);
	auto* pProgress211 = pQuest211->add_progress();
	pProgress211->set_max(1);

	auto* pQuest212 = pQuests->add_list();
	pQuest212->set_id(12503);
	pQuest212->set_type(TowerEvent);
	auto* pProgress212 = pQuest212->add_progress();
	pProgress212->set_max(3);

	auto* pQuest213 = pQuests->add_list();
	pQuest213->set_id(13302);
	pQuest213->set_type(TowerEvent);
	auto* pProgress213 = pQuest213->add_progress();
	pProgress213->set_max(2);

	auto* pQuest214 = pQuests->add_list();
	pQuest214->set_id(14702);
	pQuest214->set_type(TowerEvent);
	auto* pProgress214 = pQuest214->add_progress();
	pProgress214->set_max(2);

	auto* pQuest215 = pQuests->add_list();
	pQuest215->set_id(11501);
	pQuest215->set_type(TowerEvent);
	auto* pProgress215 = pQuest215->add_progress();
	pProgress215->set_max(1);

	auto* pQuest216 = pQuests->add_list();
	pQuest216->set_id(12603);
	pQuest216->set_type(TowerEvent);
	pQuest216->set_status(1);
	auto* pProgress216 = pQuest216->add_progress();
	pProgress216->set_cur(3);
	pProgress216->set_max(3);

	auto* pQuest217 = pQuests->add_list();
	pQuest217->set_id(15003);
	pQuest217->set_type(TowerEvent);
	auto* pProgress217 = pQuest217->add_progress();
	pProgress217->set_max(3);

	auto* pQuest218 = pQuests->add_list();
	pQuest218->set_id(11601);
	pQuest218->set_type(TowerEvent);
	auto* pProgress218 = pQuest218->add_progress();
	pProgress218->set_max(1);

	auto* pQuest219 = pQuests->add_list();
	pQuest219->set_id(13501);
	pQuest219->set_type(TowerEvent);
	auto* pProgress219 = pQuest219->add_progress();
	pProgress219->set_max(1);

	auto* pQuest220 = pQuests->add_list();
	pQuest220->set_id(12502);
	pQuest220->set_type(TowerEvent);
	auto* pProgress220 = pQuest220->add_progress();
	pProgress220->set_max(2);

	auto* pQuest221 = pQuests->add_list();
	pQuest221->set_id(13303);
	pQuest221->set_type(TowerEvent);
	auto* pProgress221 = pQuest221->add_progress();
	pProgress221->set_max(3);

	auto* pQuest222 = pQuests->add_list();
	pQuest222->set_id(13401);
	pQuest222->set_type(TowerEvent);
	pQuest222->set_status(1);
	auto* pProgress222 = pQuest222->add_progress();
	pProgress222->set_cur(1);
	pProgress222->set_max(1);

	auto* pQuest223 = pQuests->add_list();
	pQuest223->set_id(14203);
	pQuest223->set_type(TowerEvent);
	auto* pProgress223 = pQuest223->add_progress();
	pProgress223->set_max(3);

	auto* pQuest224 = pQuests->add_list();
	pQuest224->set_id(15902);
	pQuest224->set_type(TowerEvent);
	auto* pProgress224 = pQuest224->add_progress();
	pProgress224->set_max(2);

	auto* pQuest225 = pQuests->add_list();
	pQuest225->set_id(917303);
	pQuest225->set_type(TowerEvent);
	auto* pProgress225 = pQuest225->add_progress();
	pProgress225->set_cur(8);
	pProgress225->set_max(20);

	auto* pQuest226 = pQuests->add_list();
	pQuest226->set_id(917403);
	pQuest226->set_type(TowerEvent);
	auto* pProgress226 = pQuest226->add_progress();
	pProgress226->set_cur(7);
	pProgress226->set_max(20);

	auto* pQuest227 = pQuests->add_list();
	pQuest227->set_id(11503);
	pQuest227->set_type(TowerEvent);
	auto* pProgress227 = pQuest227->add_progress();
	pProgress227->set_max(3);

	auto* pQuest228 = pQuests->add_list();
	pQuest228->set_id(14302);
	pQuest228->set_type(TowerEvent);
	auto* pProgress228 = pQuest228->add_progress();
	pProgress228->set_max(2);

	auto* pQuest229 = pQuests->add_list();
	pQuest229->set_id(14902);
	pQuest229->set_type(TowerEvent);
	auto* pProgress229 = pQuest229->add_progress();
	pProgress229->set_max(2);

	auto* pQuest230 = pQuests->add_list();
	pQuest230->set_id(15903);
	pQuest230->set_type(TowerEvent);
	auto* pProgress230 = pQuest230->add_progress();
	pProgress230->set_max(3);

	auto* pQuest231 = pQuests->add_list();
	pQuest231->set_id(13503);
	pQuest231->set_type(TowerEvent);
	auto* pProgress231 = pQuest231->add_progress();
	pProgress231->set_max(3);

	auto* pQuest232 = pQuests->add_list();
	pQuest232->set_id(14102);
	pQuest232->set_type(TowerEvent);
	auto* pProgress232 = pQuest232->add_progress();
	pProgress232->set_max(2);

	auto* pQuest233 = pQuests->add_list();
	pQuest233->set_id(13001);
	pQuest233->set_type(TowerEvent);
	auto* pProgress233 = pQuest233->add_progress();
	pProgress233->set_max(1);

	auto* pQuest234 = pQuests->add_list();
	pQuest234->set_id(13002);
	pQuest234->set_type(TowerEvent);
	auto* pProgress234 = pQuest234->add_progress();
	pProgress234->set_max(2);

	auto* pQuest235 = pQuests->add_list();
	pQuest235->set_id(917203);
	pQuest235->set_type(TowerEvent);
	auto* pProgress235 = pQuest235->add_progress();
	pProgress235->set_cur(5);
	pProgress235->set_max(20);

	auto* pQuest236 = pQuests->add_list();
	pQuest236->set_id(11301);
	pQuest236->set_type(TowerEvent);
	auto* pProgress236 = pQuest236->add_progress();
	pProgress236->set_max(1);

	auto* pQuest237 = pQuests->add_list();
	pQuest237->set_id(13301);
	pQuest237->set_type(TowerEvent);
	auto* pProgress237 = pQuest237->add_progress();
	pProgress237->set_max(1);

	auto* pQuest238 = pQuests->add_list();
	pQuest238->set_id(15803);
	pQuest238->set_type(TowerEvent);
	pQuest238->set_status(1);
	auto* pProgress238 = pQuest238->add_progress();
	pProgress238->set_cur(3);
	pProgress238->set_max(3);

	auto* pQuest239 = pQuests->add_list();
	pQuest239->set_id(14903);
	pQuest239->set_type(TowerEvent);
	auto* pProgress239 = pQuest239->add_progress();
	pProgress239->set_max(3);

	auto* pQuest240 = pQuests->add_list();
	pQuest240->set_id(12701);
	pQuest240->set_type(TowerEvent);
	auto* pProgress240 = pQuest240->add_progress();
	pProgress240->set_max(1);

	auto* pQuest241 = pQuests->add_list();
	pQuest241->set_id(15001);
	pQuest241->set_type(TowerEvent);
	auto* pProgress241 = pQuest241->add_progress();
	pProgress241->set_max(1);

	auto* pQuest242 = pQuests->add_list();
	pQuest242->set_id(15503);
	pQuest242->set_type(TowerEvent);
	pQuest242->set_status(1);
	auto* pProgress242 = pQuest242->add_progress();
	pProgress242->set_cur(3);
	pProgress242->set_max(3);

	auto* pQuest243 = pQuests->add_list();
	pQuest243->set_id(14701);
	pQuest243->set_type(TowerEvent);
	auto* pProgress243 = pQuest243->add_progress();
	pProgress243->set_max(1);

	auto* pQuest244 = pQuests->add_list();
	pQuest244->set_id(913302);
	pQuest244->set_type(TowerEvent);
	auto* pProgress244 = pQuest244->add_progress();
	pProgress244->set_cur(6);
	pProgress244->set_max(10);

	auto* pQuest245 = pQuests->add_list();
	pQuest245->set_id(11401);
	pQuest245->set_type(TowerEvent);
	auto* pProgress245 = pQuest245->add_progress();
	pProgress245->set_max(1);

	auto* pQuest246 = pQuests->add_list();
	pQuest246->set_id(11603);
	pQuest246->set_type(TowerEvent);
	auto* pProgress246 = pQuest246->add_progress();
	pProgress246->set_max(3);

	auto* pQuest247 = pQuests->add_list();
	pQuest247->set_id(11403);
	pQuest247->set_type(TowerEvent);
	auto* pProgress247 = pQuest247->add_progress();
	pProgress247->set_max(3);

	auto* pQuest248 = pQuests->add_list();
	pQuest248->set_id(15602);
	pQuest248->set_type(TowerEvent);
	auto* pProgress248 = pQuest248->add_progress();
	pProgress248->set_max(2);

	auto* pQuest249 = pQuests->add_list();
	pQuest249->set_id(10803);
	pQuest249->set_type(TowerEvent);
	auto* pProgress249 = pQuest249->add_progress();
	pProgress249->set_max(3);

	auto* pQuest250 = pQuests->add_list();
	pQuest250->set_id(11303);
	pQuest250->set_type(TowerEvent);
	auto* pProgress250 = pQuest250->add_progress();
	pProgress250->set_max(3);

	auto* pQuest251 = pQuests->add_list();
	pQuest251->set_id(11602);
	pQuest251->set_type(TowerEvent);
	auto* pProgress251 = pQuest251->add_progress();
	pProgress251->set_max(2);

	auto* pQuest252 = pQuests->add_list();
	pQuest252->set_id(11502);
	pQuest252->set_type(TowerEvent);
	auto* pProgress252 = pQuest252->add_progress();
	pProgress252->set_max(2);

	auto* pQuest253 = pQuests->add_list();
	pQuest253->set_id(11902);
	pQuest253->set_type(TowerEvent);
	auto* pProgress253 = pQuest253->add_progress();
	pProgress253->set_max(2);

	auto* pQuest254 = pQuests->add_list();
	pQuest254->set_id(11903);
	pQuest254->set_type(TowerEvent);
	auto* pProgress254 = pQuest254->add_progress();
	pProgress254->set_max(3);

	auto* pQuest255 = pQuests->add_list();
	pQuest255->set_id(12003);
	pQuest255->set_type(TowerEvent);
	auto* pProgress255 = pQuest255->add_progress();
	pProgress255->set_max(3);

	auto* pQuest256 = pQuests->add_list();
	pQuest256->set_id(14202);
	pQuest256->set_type(TowerEvent);
	auto* pProgress256 = pQuest256->add_progress();
	pProgress256->set_max(2);

	auto* pQuest257 = pQuests->add_list();
	pQuest257->set_id(15601);
	pQuest257->set_type(TowerEvent);
	auto* pProgress257 = pQuest257->add_progress();
	pProgress257->set_max(1);

	auto* pQuest258 = pQuests->add_list();
	pQuest258->set_id(14501);
	pQuest258->set_type(TowerEvent);
	auto* pProgress258 = pQuest258->add_progress();
	pProgress258->set_max(1);

	auto* pQuest259 = pQuests->add_list();
	pQuest259->set_id(14502);
	pQuest259->set_type(TowerEvent);
	auto* pProgress259 = pQuest259->add_progress();
	pProgress259->set_max(2);

	auto* pQuest260 = pQuests->add_list();
	pQuest260->set_id(12602);
	pQuest260->set_type(TowerEvent);
	pQuest260->set_status(1);
	auto* pProgress260 = pQuest260->add_progress();
	pProgress260->set_cur(2);
	pProgress260->set_max(2);

	auto* pQuest261 = pQuests->add_list();
	pQuest261->set_id(15002);
	pQuest261->set_type(TowerEvent);
	auto* pProgress261 = pQuest261->add_progress();
	pProgress261->set_max(2);


	// === RegionBossLevels ===
	auto* pRegionBossLevels = info.mutable_regionbosslevels();

	auto* pRegionBoss1 = pRegionBossLevels->Add();
	pRegionBoss1->set_id(6009108);
	pRegionBoss1->set_star(3);
	pRegionBoss1->set_first(true);
	pRegionBoss1->set_threestar(true);
	pRegionBoss1->set_buildid(1767792420);

	auto* pRegionBoss2 = pRegionBossLevels->Add();
	pRegionBoss2->set_id(6010102);
	pRegionBoss2->set_star(3);
	pRegionBoss2->set_first(true);
	pRegionBoss2->set_threestar(true);
	pRegionBoss2->set_buildid(1761393282);

	auto* pRegionBoss3 = pRegionBossLevels->Add();
	pRegionBoss3->set_id(6011103);
	pRegionBoss3->set_star(3);
	pRegionBoss3->set_first(true);
	pRegionBoss3->set_threestar(true);
	pRegionBoss3->set_buildid(1761454633);

	auto* pRegionBoss4 = pRegionBossLevels->Add();
	pRegionBoss4->set_id(6010104);
	pRegionBoss4->set_star(3);
	pRegionBoss4->set_first(true);
	pRegionBoss4->set_threestar(true);
	pRegionBoss4->set_buildid(1761657209);

	auto* pRegionBoss5 = pRegionBossLevels->Add();
	pRegionBoss5->set_id(6011104);
	pRegionBoss5->set_star(3);
	pRegionBoss5->set_first(true);
	pRegionBoss5->set_threestar(true);
	pRegionBoss5->set_buildid(1761657209);

	auto* pRegionBoss6 = pRegionBossLevels->Add();
	pRegionBoss6->set_id(6009105);
	pRegionBoss6->set_star(3);
	pRegionBoss6->set_first(true);
	pRegionBoss6->set_threestar(true);
	pRegionBoss6->set_buildid(1762681661);

	auto* pRegionBoss7 = pRegionBossLevels->Add();
	pRegionBoss7->set_id(6010106);
	pRegionBoss7->set_star(3);
	pRegionBoss7->set_first(true);
	pRegionBoss7->set_threestar(true);
	pRegionBoss7->set_buildid(1764786061);

	auto* pRegionBoss8 = pRegionBossLevels->Add();
	pRegionBoss8->set_id(6010108);
	pRegionBoss8->set_star(3);
	pRegionBoss8->set_first(true);
	pRegionBoss8->set_threestar(true);
	pRegionBoss8->set_buildid(1764786061);

	auto* pRegionBoss9 = pRegionBossLevels->Add();
	pRegionBoss9->set_id(6009101);
	pRegionBoss9->set_star(3);
	pRegionBoss9->set_first(true);
	pRegionBoss9->set_threestar(true);
	pRegionBoss9->set_buildid(1760972241);

	auto* pRegionBoss10 = pRegionBossLevels->Add();
	pRegionBoss10->set_id(6009103);
	pRegionBoss10->set_star(3);
	pRegionBoss10->set_first(true);
	pRegionBoss10->set_threestar(true);
	pRegionBoss10->set_buildid(1761393282);

	auto* pRegionBoss11 = pRegionBossLevels->Add();
	pRegionBoss11->set_id(6010103);
	pRegionBoss11->set_star(3);
	pRegionBoss11->set_first(true);
	pRegionBoss11->set_threestar(true);
	pRegionBoss11->set_buildid(1761393282);

	auto* pRegionBoss12 = pRegionBossLevels->Add();
	pRegionBoss12->set_id(6011102);
	pRegionBoss12->set_star(3);
	pRegionBoss12->set_first(true);
	pRegionBoss12->set_threestar(true);
	pRegionBoss12->set_buildid(1761454633);

	auto* pRegionBoss13 = pRegionBossLevels->Add();
	pRegionBoss13->set_id(6009106);
	pRegionBoss13->set_star(3);
	pRegionBoss13->set_first(true);
	pRegionBoss13->set_threestar(true);
	pRegionBoss13->set_buildid(1764059826);

	auto* pRegionBoss14 = pRegionBossLevels->Add();
	pRegionBoss14->set_id(6011106);
	pRegionBoss14->set_star(3);
	pRegionBoss14->set_first(true);
	pRegionBoss14->set_threestar(true);
	pRegionBoss14->set_buildid(1764786061);

	auto* pRegionBoss15 = pRegionBossLevels->Add();
	pRegionBoss15->set_id(6011108);
	pRegionBoss15->set_star(3);
	pRegionBoss15->set_first(true);
	pRegionBoss15->set_threestar(true);
	pRegionBoss15->set_buildid(1762003356);

	auto* pRegionBoss16 = pRegionBossLevels->Add();
	pRegionBoss16->set_id(6009102);
	pRegionBoss16->set_star(3);
	pRegionBoss16->set_first(true);
	pRegionBoss16->set_threestar(true);
	pRegionBoss16->set_buildid(1761056854);

	auto* pRegionBoss17 = pRegionBossLevels->Add();
	pRegionBoss17->set_id(6011105);
	pRegionBoss17->set_star(3);
	pRegionBoss17->set_first(true);
	pRegionBoss17->set_threestar(true);
	pRegionBoss17->set_buildid(1762003356);

	auto* pRegionBoss18 = pRegionBossLevels->Add();
	pRegionBoss18->set_id(6010105);
	pRegionBoss18->set_star(3);
	pRegionBoss18->set_first(true);
	pRegionBoss18->set_threestar(true);
	pRegionBoss18->set_buildid(1762681661);

	auto* pRegionBoss19 = pRegionBossLevels->Add();
	pRegionBoss19->set_id(6010101);
	pRegionBoss19->set_star(3);
	pRegionBoss19->set_first(true);
	pRegionBoss19->set_threestar(true);
	pRegionBoss19->set_buildid(1761056854);

	auto* pRegionBoss20 = pRegionBossLevels->Add();
	pRegionBoss20->set_id(6011101);
	pRegionBoss20->set_star(3);
	pRegionBoss20->set_first(true);
	pRegionBoss20->set_threestar(true);
	pRegionBoss20->set_buildid(1761056854);

	auto* pRegionBoss21 = pRegionBossLevels->Add();
	pRegionBoss21->set_id(6009104);
	pRegionBoss21->set_star(3);
	pRegionBoss21->set_first(true);
	pRegionBoss21->set_threestar(true);
	pRegionBoss21->set_buildid(1761657209);

	auto* pRegionBoss22 = pRegionBossLevels->Add();
	pRegionBoss22->set_id(6010107);
	pRegionBoss22->set_star(3);
	pRegionBoss22->set_first(true);
	pRegionBoss22->set_threestar(true);
	pRegionBoss22->set_buildid(1764786061);

	auto* pRegionBoss23 = pRegionBossLevels->Add();
	pRegionBoss23->set_id(6011107);
	pRegionBoss23->set_star(3);
	pRegionBoss23->set_first(true);
	pRegionBoss23->set_threestar(true);
	pRegionBoss23->set_buildid(1762003356);

	auto* pRegionBoss24 = pRegionBossLevels->Add();
	pRegionBoss24->set_id(6009107);
	pRegionBoss24->set_star(3);
	pRegionBoss24->set_first(true);
	pRegionBoss24->set_threestar(true);
	pRegionBoss24->set_buildid(1767792420);

	// === Res ===
	auto* pRes = info.mutable_res();

	auto* pRes1 = pRes->Add();
	pRes1->set_tid(31);
	pRes1->set_qty(1200);

	auto* pRes2 = pRes->Add();
	pRes2->set_tid(34);
	pRes2->set_qty(16650);

	auto* pRes3 = pRes->Add();
	pRes3->set_tid(28);
	pRes3->set_qty(3);

	auto* pRes4 = pRes->Add();
	pRes4->set_tid(1);
	pRes4->set_qty(96060650);

	auto* pRes5 = pRes->Add();
	pRes5->set_tid(24);
	pRes5->set_qty(1400);

	auto* pRes6 = pRes->Add();
	pRes6->set_tid(23);
	pRes6->set_qty(74300);

	auto* pRes7 = pRes->Add();
	pRes7->set_tid(12);
	pRes7->set_qty(6417);

	auto* pRes8 = pRes->Add();
	pRes8->set_tid(2);
	pRes8->set_qty(24985);


	// === RglPassedIds ===
	auto* pRglPassedIds = info.mutable_rglpassedids();

	pRglPassedIds->Add(401);
	pRglPassedIds->Add(202);
	pRglPassedIds->Add(102);
	pRglPassedIds->Add(103);
	pRglPassedIds->Add(104);
	pRglPassedIds->Add(204);
	pRglPassedIds->Add(304);
	pRglPassedIds->Add(105);
	pRglPassedIds->Add(206);
	pRglPassedIds->Add(306);
	pRglPassedIds->Add(107);
	pRglPassedIds->Add(108);
	pRglPassedIds->Add(308);


	// === SkillInstances ===
	auto* pSkillInstances = info.mutable_skillinstances();

	auto* pSkillInstance1 = pSkillInstances->Add();
	pSkillInstance1->set_id(3002);
	pSkillInstance1->set_star(3);
	pSkillInstance1->set_first(true);
	pSkillInstance1->set_threestar(true);
	pSkillInstance1->set_buildid(1761657209);

	auto* pSkillInstance2 = pSkillInstances->Add();
	pSkillInstance2->set_id(2001);
	pSkillInstance2->set_star(3);
	pSkillInstance2->set_first(true);
	pSkillInstance2->set_threestar(true);
	pSkillInstance2->set_buildid(1761657209);

	auto* pSkillInstance3 = pSkillInstances->Add();
	pSkillInstance3->set_id(3001);
	pSkillInstance3->set_star(3);
	pSkillInstance3->set_first(true);
	pSkillInstance3->set_threestar(true);
	pSkillInstance3->set_buildid(1761393282);

	auto* pSkillInstance4 = pSkillInstances->Add();
	pSkillInstance4->set_id(2002);
	pSkillInstance4->set_star(3);
	pSkillInstance4->set_first(true);
	pSkillInstance4->set_threestar(true);
	pSkillInstance4->set_buildid(1761657209);

	auto* pSkillInstance5 = pSkillInstances->Add();
	pSkillInstance5->set_id(1002);
	pSkillInstance5->set_star(3);
	pSkillInstance5->set_first(true);
	pSkillInstance5->set_threestar(true);
	pSkillInstance5->set_buildid(1771756242);

	auto* pSkillInstance6 = pSkillInstances->Add();
	pSkillInstance6->set_id(1003);
	pSkillInstance6->set_star(3);
	pSkillInstance6->set_first(true);
	pSkillInstance6->set_threestar(true);
	pSkillInstance6->set_buildid(1771756242);

	auto* pSkillInstance7 = pSkillInstances->Add();
	pSkillInstance7->set_id(1004);
	pSkillInstance7->set_star(3);
	pSkillInstance7->set_first(true);
	pSkillInstance7->set_threestar(true);
	pSkillInstance7->set_buildid(1771756242);

	auto* pSkillInstance8 = pSkillInstances->Add();
	pSkillInstance8->set_id(1005);
	pSkillInstance8->set_star(3);
	pSkillInstance8->set_first(true);
	pSkillInstance8->set_threestar(true);
	pSkillInstance8->set_buildid(1771756242);

	auto* pSkillInstance9 = pSkillInstances->Add();
	pSkillInstance9->set_id(2000);
	pSkillInstance9->set_star(3);
	pSkillInstance9->set_first(true);
	pSkillInstance9->set_threestar(true);
	pSkillInstance9->set_buildid(1761056854);

	auto* pSkillInstance10 = pSkillInstances->Add();
	pSkillInstance10->set_id(2003);
	pSkillInstance10->set_star(3);
	pSkillInstance10->set_first(true);
	pSkillInstance10->set_threestar(true);
	pSkillInstance10->set_buildid(1762681661);

	auto* pSkillInstance11 = pSkillInstances->Add();
	pSkillInstance11->set_id(3003);
	pSkillInstance11->set_star(3);
	pSkillInstance11->set_first(true);
	pSkillInstance11->set_threestar(true);
	pSkillInstance11->set_buildid(1764786061);

	auto* pSkillInstance12 = pSkillInstances->Add();
	pSkillInstance12->set_id(2005);
	pSkillInstance12->set_star(3);
	pSkillInstance12->set_first(true);
	pSkillInstance12->set_threestar(true);
	pSkillInstance12->set_buildid(1764786061);

	auto* pSkillInstance13 = pSkillInstances->Add();
	pSkillInstance13->set_id(3004);
	pSkillInstance13->set_star(3);
	pSkillInstance13->set_first(true);
	pSkillInstance13->set_threestar(true);
	pSkillInstance13->set_buildid(1764786061);

	auto* pSkillInstance14 = pSkillInstances->Add();
	pSkillInstance14->set_id(3005);
	pSkillInstance14->set_star(3);
	pSkillInstance14->set_first(true);
	pSkillInstance14->set_threestar(true);
	pSkillInstance14->set_buildid(1771588091);

	auto* pSkillInstance15 = pSkillInstances->Add();
	pSkillInstance15->set_id(1000);
	pSkillInstance15->set_star(3);
	pSkillInstance15->set_first(true);
	pSkillInstance15->set_threestar(true);
	pSkillInstance15->set_buildid(1761056854);

	auto* pSkillInstance16 = pSkillInstances->Add();
	pSkillInstance16->set_id(3000);
	pSkillInstance16->set_star(3);
	pSkillInstance16->set_first(true);
	pSkillInstance16->set_threestar(true);
	pSkillInstance16->set_buildid(1761056854);

	auto* pSkillInstance17 = pSkillInstances->Add();
	pSkillInstance17->set_id(1001);
	pSkillInstance17->set_star(3);
	pSkillInstance17->set_first(true);
	pSkillInstance17->set_threestar(true);
	pSkillInstance17->set_buildid(1761657209);

	auto* pSkillInstance18 = pSkillInstances->Add();
	pSkillInstance18->set_id(2004);
	pSkillInstance18->set_star(3);
	pSkillInstance18->set_first(true);
	pSkillInstance18->set_threestar(true);
	pSkillInstance18->set_buildid(1762681661);


	// === State ===
	auto* pState = info.mutable_state();

	// Achievement
	auto* pAchievement = pState->mutable_achievement();
	pAchievement->set_new_(true);

	// Activities
	auto* pActivitiesState = pState->mutable_activities();
	auto* pActivityState1 = pActivitiesState->Add();
	pActivityState1->set_id(101002);

	auto* pActivityState2 = pActivitiesState->Add();
	pActivityState2->set_reddot(true);

	auto* pActivityState3 = pActivitiesState->Add();
	pActivityState3->set_id(101003);
	pActivityState3->set_banner(true);

	auto* pActivityState4 = pActivitiesState->Add();
	pActivityState4->set_id(102001);

	auto* pActivityState5 = pActivitiesState->Add();
	pActivityState5->set_id(102002);

	auto* pActivityState6 = pActivitiesState->Add();
	pActivityState6->set_id(800001);

	auto* pActivityState7 = pActivitiesState->Add();
	pActivityState7->set_id(1010703);

	// BattlePass
	auto* pBattlePass = pState->mutable_battlepass();
	pBattlePass->set_state(1);

	// CharAffinityRewards
	auto* pCharAffinityRewards = pState->mutable_charaffinityrewards();

	auto* pCharAffinityReward1 = pCharAffinityRewards->Add();
	pCharAffinityReward1->set_charid(115);
	pCharAffinityReward1->add_questids(11503);
	pCharAffinityReward1->add_questids(11506);
	pCharAffinityReward1->add_questids(11504);

	auto* pCharAffinityReward2 = pCharAffinityRewards->Add();
	pCharAffinityReward2->set_charid(126);
	pCharAffinityReward2->add_questids(12603);
	pCharAffinityReward2->add_questids(12606);
	pCharAffinityReward2->add_questids(12604);

	auto* pCharAffinityReward3 = pCharAffinityRewards->Add();
	pCharAffinityReward3->set_charid(107);
	pCharAffinityReward3->add_questids(10704);

	// FriendEnergy
	auto* pFriendEnergy = pState->mutable_friendenergy();
	pFriendEnergy->set_state(true);

	// Mail
	auto* pMail = pState->mutable_mail();

	// MallPackage
	auto* pMallPackage = pState->mutable_mallpackage();

	// NpcAffinityReward
	pState->set_npcaffinityreward(true);

	// ScoreBoss
	auto* pScoreBoss = pState->mutable_scoreboss();

	// StarTower
	auto* pStarTower = pState->mutable_startower();

	// StarTowerBook
	auto* pStarTowerBook = pState->mutable_startowerbook();
	pStarTowerBook->add_eventids(12602);
	pStarTowerBook->add_eventids(15801);
	pStarTowerBook->add_eventids(13401);
	pStarTowerBook->add_eventids(15503);
	pStarTowerBook->add_eventids(12603);
	pStarTowerBook->add_eventids(13402);
	pStarTowerBook->add_eventids(15802);
	pStarTowerBook->add_eventids(13403);
	pStarTowerBook->add_eventids(15803);

	// StorySet
	pState->set_storyset(true);

	// TravelerDuelQuest
	auto* pTravelerDuelQuest = pState->mutable_travelerduelquest();
	pTravelerDuelQuest->set_type(TravelerDuel);

	// WorldClassReward
	auto* pWorldClassReward = pState->mutable_worldclassreward();
	pWorldClassReward->set_flag(Base64Decode("AAAAAAA="));



	// === Story ===
	auto* pStory = info.mutable_story();

	// Evidences
	auto* pEvidences = pStory->mutable_evidences();
	pEvidences->Add(103);
	pEvidences->Add(202);
	pEvidences->Add(101);
	pEvidences->Add(102);
	pEvidences->Add(201);
	pEvidences->Add(203);

	// Stories
	auto* pStories = pStory->mutable_stories();

	// Story 1 (Idx: 208)
	auto* pStory1 = pStories->Add();
	pStory1->set_idx(208);

	// Story 2 (Idx: 107)
	auto* pStory2 = pStories->Add();
	pStory2->set_idx(107);
	auto* pMajor2 = pStory2->mutable_major();
	auto* pMajorItem2_1 = pMajor2->Add();
	pMajorItem2_1->set_group(1);
	pMajorItem2_1->set_value(71);
	auto* pPersonality2 = pStory2->mutable_personality();
	auto* pPersonalityItem2_1 = pPersonality2->Add();
	pPersonalityItem2_1->set_group(99);
	pPersonalityItem2_1->set_value(273);

	// Story 3 (Idx: 101)
	auto* pStory3 = pStories->Add();
	pStory3->set_idx(101);

	// Story 4 (Idx: 205)
	auto* pStory4 = pStories->Add();
	pStory4->set_idx(205);

	// Story 5 (Idx: 216)
	auto* pStory5 = pStories->Add();
	pStory5->set_idx(216);

	// Story 6 (Idx: 203)
	auto* pStory6 = pStories->Add();
	pStory6->set_idx(203);

	// Story 7 (Idx: 215)
	auto* pStory7 = pStories->Add();
	pStory7->set_idx(215);

	// Story 8 (Idx: 108)
	auto* pStory8 = pStories->Add();
	pStory8->set_idx(108);

	// Story 9 (Idx: 206)
	auto* pStory9 = pStories->Add();
	pStory9->set_idx(206);

	// Story 10 (Idx: 202)
	auto* pStory10 = pStories->Add();
	pStory10->set_idx(202);

	// Story 11 (Idx: 100)
	auto* pStory11 = pStories->Add();
	pStory11->set_idx(100);

	// Story 12 (Idx: 213)
	auto* pStory12 = pStories->Add();
	pStory12->set_idx(213);

	// Story 13 (Idx: 214)
	auto* pStory13 = pStories->Add();
	pStory13->set_idx(214);
	auto* pPersonality13 = pStory13->mutable_personality();
	auto* pPersonalityItem13_1 = pPersonality13->Add();
	pPersonalityItem13_1->set_group(1);
	pPersonalityItem13_1->set_value(290);

	// Story 14 (Idx: 204)
	auto* pStory14 = pStories->Add();
	pStory14->set_idx(204);
	auto* pMajor14 = pStory14->mutable_major();
	auto* pMajorItem14_1 = pMajor14->Add();
	pMajorItem14_1->set_group(1);
	pMajorItem14_1->set_value(39);

	// Story 15 (Idx: 111)
	auto* pStory15 = pStories->Add();
	pStory15->set_idx(111);
	auto* pPersonality15 = pStory15->mutable_personality();
	auto* pPersonalityItem15_1 = pPersonality15->Add();
	pPersonalityItem15_1->set_group(2);
	pPersonalityItem15_1->set_value(273);

	// Story 16 (Idx: 212)
	auto* pStory16 = pStories->Add();
	pStory16->set_idx(212);

	// Story 17 (Idx: 105)
	auto* pStory17 = pStories->Add();
	pStory17->set_idx(105);

	// Story 18 (Idx: 211)
	auto* pStory18 = pStories->Add();
	pStory18->set_idx(211);
	auto* pPersonality18 = pStory18->mutable_personality();
	auto* pPersonalityItem18_1 = pPersonality18->Add();
	pPersonalityItem18_1->set_group(1);
	pPersonalityItem18_1->set_value(324);

	// Story 19 (Idx: 304)
	auto* pStory19 = pStories->Add();
	pStory19->set_idx(304);
	auto* pPersonality19 = pStory19->mutable_personality();
	auto* pPersonalityItem19_1 = pPersonality19->Add();
	pPersonalityItem19_1->set_group(1);
	pPersonalityItem19_1->set_value(327);

	// Story 20 (Idx: 112)
	auto* pStory20 = pStories->Add();
	pStory20->set_idx(112);

	// Story 21 (Idx: 109)
	auto* pStory21 = pStories->Add();
	pStory21->set_idx(109);

	// Story 22 (Idx: 217)
	auto* pStory22 = pStories->Add();
	pStory22->set_idx(217);

	// Story 23 (Idx: 209)
	auto* pStory23 = pStories->Add();
	pStory23->set_idx(209);

	// Story 24 (Idx: 305)
	auto* pStory24 = pStories->Add();
	pStory24->set_idx(305);
	auto* pPersonality24 = pStory24->mutable_personality();
	auto* pPersonalityItem24_1 = pPersonality24->Add();
	pPersonalityItem24_1->set_group(1);
	pPersonalityItem24_1->set_value(324);

	// Story 25 (Idx: 110)
	auto* pStory25 = pStories->Add();
	pStory25->set_idx(110);

	// Story 26 (Idx: 207)
	auto* pStory26 = pStories->Add();
	pStory26->set_idx(207);

	// Story 27 (Idx: 103)
	auto* pStory27 = pStories->Add();
	pStory27->set_idx(103);

	// Story 28 (Idx: 201)
	auto* pStory28 = pStories->Add();
	pStory28->set_idx(201);

	// Story 29 (Idx: 106)
	auto* pStory29 = pStories->Add();
	pStory29->set_idx(106);

	// Story 30 (Idx: 102)
	auto* pStory30 = pStories->Add();
	pStory30->set_idx(102);

	// Story 31 (Idx: 210)
	auto* pStory31 = pStories->Add();
	pStory31->set_idx(210);

	// Story 32 (Idx: 104)
	auto* pStory32 = pStories->Add();
	pStory32->set_idx(104);
	auto* pPersonality32 = pStory32->mutable_personality();
	auto* pPersonalityItem32_1 = pPersonality32->Add();
	pPersonalityItem32_1->set_group(1);
	pPersonalityItem32_1->set_value(273);



	// === Titles ===
	auto* pTitles = info.mutable_titles();

	pTitles->Add()->set_titleid(1);
	pTitles->Add()->set_titleid(2);
	pTitles->Add()->set_titleid(3);
	pTitles->Add()->set_titleid(4);
	pTitles->Add()->set_titleid(5);
	pTitles->Add()->set_titleid(6);
	pTitles->Add()->set_titleid(7);
	pTitles->Add()->set_titleid(8);
	pTitles->Add()->set_titleid(9);
	pTitles->Add()->set_titleid(10);
	pTitles->Add()->set_titleid(13);
	pTitles->Add()->set_titleid(14);
	pTitles->Add()->set_titleid(25);
	pTitles->Add()->set_titleid(26);


	// === TutorialLevels ===
	auto* pTutorialLevels = info.mutable_tutoriallevels();

	auto* pTutorialLevel1 = pTutorialLevels->Add();
	pTutorialLevel1->set_levelid(1);
	pTutorialLevel1->set_passed(true);
	pTutorialLevel1->set_rewardreceived(true);


	// === VampireSurvivorRecord ===
	auto* pVampireSurvivorRecord = info.mutable_vampiresurvivorrecord();

	// Records
	auto* pVampireSurvivorRecordRecords = pVampireSurvivorRecord->mutable_records();

	// Record 1 (Id: 101)
	auto* pVampireSurvivorRecordRecord1 = pVampireSurvivorRecordRecords->Add();
	pVampireSurvivorRecordRecord1->set_id(101);
	pVampireSurvivorRecordRecord1->set_passed(true);
	pVampireSurvivorRecordRecord1->set_score(6254);
	pVampireSurvivorRecordRecord1->add_buildids(1761657209);

	// Record 2 (Id: 102)
	auto* pVampireSurvivorRecordRecord2 = pVampireSurvivorRecordRecords->Add();
	pVampireSurvivorRecordRecord2->set_id(102);
	pVampireSurvivorRecordRecord2->add_buildids(1761657209);

	// Season (empty)
	auto* pSeason = pVampireSurvivorRecord->mutable_season();

	// === WeekBossLevels ===
	auto* pWeekBossLevels = info.mutable_weekbosslevels();

	auto* pWeekBoss1 = pWeekBossLevels->Add();
	pWeekBoss1->set_id(6201101);
	pWeekBoss1->set_time(40);
	pWeekBoss1->set_first(true);
	pWeekBoss1->set_buildid(1764786061);

	auto* pWeekBoss2 = pWeekBossLevels->Add();
	pWeekBoss2->set_id(6203101);
	pWeekBoss2->set_time(80);
	pWeekBoss2->set_first(true);
	pWeekBoss2->set_buildid(1764786061);

	auto* pWeekBoss3 = pWeekBossLevels->Add();
	pWeekBoss3->set_id(6203102);
	pWeekBoss3->set_time(20);
	pWeekBoss3->set_first(true);
	pWeekBoss3->set_buildid(1769695515);

	auto* pWeekBoss4 = pWeekBossLevels->Add();
	pWeekBoss4->set_id(6203103);
	pWeekBoss4->set_time(5);
	pWeekBoss4->set_first(true);
	pWeekBoss4->set_buildid(1771588091);

	auto* pWeekBoss5 = pWeekBossLevels->Add();
	pWeekBoss5->set_id(6205101);
	pWeekBoss5->set_time(55);
	pWeekBoss5->set_first(true);
	pWeekBoss5->set_buildid(1769695515);



	// === WorldClass ===
	auto* pWorldClass = info.mutable_worldclass();
	pWorldClass->set_cur(40);

	info.set_achievements(std::string(64, '\0'));
	return info;
}

void Player::Init() {
	return;
}