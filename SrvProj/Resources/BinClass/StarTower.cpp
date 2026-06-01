#include "StarTower.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool StarTowerGrowthNodeRes::LoadFromPb(std::string data)
{
	StarTowerGrowthNode s;
	if (!s.ParseFromString(data)) {
		return false;
	}

	Id = s.id();
	NodeId = s.nodeid();
	Group = s.group();
	ItemId1 = s.itemid1();
	ItemQty1 = s.itemqty1();

    return true;
}
