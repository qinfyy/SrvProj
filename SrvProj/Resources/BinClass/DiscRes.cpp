#include "DiscRes.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../../proto/table_cpp/client_table.pb.h"

void DiscRes::OnLoad() {
}

bool DiscRes::LoadFromPb(std::string data) {
	nova::client::Disc disc;
	if (!disc.ParseFromString(data)) {
		return false;
	}

	Id = disc.id();
	Visible = disc.visible();
	Available = disc.available();
	EET = disc.eet();
	StrengthenGroupId = disc.strengthengroupid();
	PromoteGroupId = disc.promotegroupid();
	TransformItemId = disc.transformitemid();
	MaxStarTransformItem.clear();
	for (const auto& item : disc.maxstartransformitem()) {
		MaxStarTransformItem.push_back(item);
	}

	ReadReward.clear();
	for (const auto& item : disc.readreward()) {
		ReadReward.push_back(item);
	}

	SecondarySkillGroupId1 = disc.secondaryskillgroupid1();
	SecondarySkillGroupId2 = disc.secondaryskillgroupid2();
	SubNoteSkillGroupId = disc.subnoteskillgroupid();
	return true;
}