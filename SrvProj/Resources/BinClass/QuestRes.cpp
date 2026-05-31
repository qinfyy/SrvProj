#include "QuestRes.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool DailyQuestRes::LoadFromPb(std::string data)
{
	DailyQuest dq;
	if (!dq.ParseFromString(data)) {
		return false;
	}
	Id = dq.id();
	Apear = dq.apear();

	Active = dq.active();
	ItemTid = dq.itemtid();
	ItemQty = dq.itemqty();
	CompleteCond = dq.completecond();
	CompleteCondClient = dq.completecondclient();
	CompleteCondParams = dq.completecondparams();


	return true;
}

bool DailyQuestActiveRes::LoadFromPb(std::string data)
{
	DailyQuestActive dqa;
	if (!dqa.ParseFromString(data)) {
		return false;
	}
	
	Id = dqa.id();
	Active = dqa.active();
	ItemTid1 = dqa.itemtid1();
	Number1 = dqa.number1();
	ItemTid2 = dqa.itemtid2();
	Number2 = dqa.number2();

	return true;
}

bool WeeklyQuestRes::LoadFromPb(std::string data)
{
	WeeklyQuest wq;
	if (!wq.ParseFromString(data)) {
		return false;
	}
	Id = wq.id();
	Apear = wq.apear();
	Active = wq.active();
	ItemTid = wq.itemtid();
	ItemQty = wq.itemqty();
	CompleteCond = wq.completecond();
	CompleteCondClient = wq.completecondclient();
	CompleteCondParams = wq.completecondparams();

	return true;
}

bool WeeklyQuestActiveRes::LoadFromPb(std::string data)
{
	WeeklyQuestActive wqa;
	if (!wqa.ParseFromString(data)) {
		return false;
	}
	Id = wqa.id();
	Active = wqa.active();
	ItemTid1 = wqa.itemtid1();
	Number1 = wqa.number1();
	ItemTid2 = wqa.itemtid2();
	Number2 = wqa.number2();

	return true;
}
