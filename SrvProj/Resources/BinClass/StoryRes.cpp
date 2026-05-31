#include "StoryRes.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool StoryRes::LoadFromPb(std::string data)
{
    Story s;
	if (!s.ParseFromString(data)) {
		return false;
	}

	Id = s.id();
	Chapter = s.chapter();
	RewardDisplay = s.rewarddisplay();

    return true;
}

bool StorySetSectionRes::LoadFromPb(std::string data)
{
    StorySetSection sss;
	if (!sss.ParseFromString(data)) {
		return false;
	}

	Id = sss.id();
	ChapterId = sss.chapterid();
	RewardItem1Tid = sss.rewarditem1tid();
	RewardItem1Qty = sss.rewarditem1qty();


    return true;
}

bool StoryEvidenceRes::LoadFromPb(std::string data)
{
	StoryEvidence se;
	if (!se.ParseFromString(data)) {
		return false;
	}

	Id = se.id();

    return true;
}

bool MainScreenCGRes::LoadFromPb(std::string data)
{
	MainScreenCG mscg;
	if (!mscg.ParseFromString(data)) {
		return false;
	}

	Id = mscg.id();
	IsShown = mscg.isshown();

    return true;
}
