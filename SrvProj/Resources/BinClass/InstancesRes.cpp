#include "DiscRes.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../../proto/table_cpp/client_table.pb.h"
#include "InstancesRes.h"

using namespace nova::client;

bool DailyInstanceRes::LoadFromPb(std::string data)
{
    DailyInstance di;
    if (!di.ParseFromString(data)) {
        return false;
    }

	Id = di.id();
	AwardDropId = di.awarddropid();

	return true;
}
