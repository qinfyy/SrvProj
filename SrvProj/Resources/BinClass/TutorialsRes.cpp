#include "TutorialsRes.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

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

