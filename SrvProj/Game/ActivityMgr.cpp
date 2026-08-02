#include "ActivityMgr.h"

#include "../Resources/GameData.h"
#include "../Resources/BinClass/ActivityRes.h"

#include <climits>

void ActivityMgr::EncodePlayerInfo(proto::PlayerInfo& out) const
{
    static constexpr uint32_t activities[] = { 2010301, 2010303, 2010304, 700123, 700124, 102002 };

    for (uint32_t id : activities)
    {
        if (GameData::ActivityDataTable.find(static_cast<int>(id)) == GameData::ActivityDataTable.end())
        {
            continue;
        }

        auto* activity = out.add_activities();
        activity->set_id(id);
        activity->set_starttime(1);
        activity->set_endtime(INT_MAX);
    }
}
