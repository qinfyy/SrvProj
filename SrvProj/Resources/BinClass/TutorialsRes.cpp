#include "TutorialsRes.h"
#include "../ResourceJsonUtil.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool TutorialLevelRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "WorldClass", WorldClass);
    ReadResourceJsonField(data, "Item1", Item1);
    ReadResourceJsonField(data, "Qty1", Qty1);
    return true;
}

bool TutorialLevelRes::LoadFromPb(std::string data)
{
    TutorialLevel tl;
    if (!tl.ParseFromString(data)) {
        return false;
    }

    Id = tl.id();
    WorldClass = tl.worldclass();
    Item1 = tl.item1();
    Qty1 = tl.qty1();

    return true;
}

