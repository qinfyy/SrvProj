#include "Commissions.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool AgentRes::LoadFromPb(std::string data)
{
	Agent a;
	if (!a.ParseFromString(data)) {
		return false;
	}
	Id = a.id();
	Level = a.level();
	MemberLimit = a.memberlimit();

	ExtraTags.clear();
	for (const auto& tag : a.extratags()) {
		ExtraTags.push_back(tag);
	}

	Time1 = a.time1();
	RewardPreview1 = a.rewardpreview1();
	BonusPreview1 = a.bonuspreview1();

	Time2 = a.time2();
	RewardPreview2 = a.rewardpreview2();
	BonusPreview2 = a.bonuspreview2();

	Time3 = a.time3();
	RewardPreview3 = a.rewardpreview3();
	BonusPreview3 = a.bonuspreview3();

	Time4 = a.time4();
	RewardPreview4 = a.rewardpreview4();
	BonusPreview4 = a.bonuspreview4();

	return true;
}
