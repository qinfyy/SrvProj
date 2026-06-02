#include "AchievementsRes.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool AchievementRes::LoadFromPb(std::string data)
{
	Achievement a;
	if (!a.ParseFromString(data)) {
		return false;
	}

	Id = a.id();
	Type = a.type();
	CompleteCond = a.completecond();
	AimNumShow = a.aimnumshow();

	Prerequisites.clear();
	for (const auto& pre : a.prerequisites()) {
		Prerequisites.push_back(pre);
	}

	Tid1 = a.tid1();
	Qty1 = a.qty1();

	return true;
}
