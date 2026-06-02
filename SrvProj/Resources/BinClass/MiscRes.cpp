#include "MiscRes.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool WorldClassRes::LoadFromPb(std::string data)
{
	WorldClass wc;
	if (!wc.ParseFromString(data)) {
		return false;
	}
	Id = wc.id();
	Exp = wc.exp();
	Reward = wc.reward();
	return true;
}

bool GuideGroupRes::LoadFromPb(std::string data)
{
	GuideGroup gg;
	if (!gg.ParseFromString(data)) {
		return false;
	}
	Id = gg.id();
	IsActive = gg.isactive();

	return true;
}

bool HandbookRes::LoadFromPb(std::string data)
{
	Handbook hb;
	if (!hb.ParseFromString(data)) {
		return false;
	}
	Id = hb.id();
	Index = hb.index();
	Type = hb.type();

	return true;
}

bool SignInRes::LoadFromPb(std::string data)
{
	SignIn si;
	if (!si.ParseFromString(data)) {
		return false;
	}
	Group = si.group();
	Day = si.day();
	ItemId = si.itemid();
	ItemQty = si.itemqty();
	return true;
}
