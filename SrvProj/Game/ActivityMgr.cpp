#include "ActivityMgr.h"

#include <climits>

void ActivityMgr::EncodePlayerInfo(proto::PlayerInfo& out) const
{
    static constexpr uint32_t activities[] = { 102002, 301031, 700118, 700117 };

    for (uint32_t id : activities)
    {
        auto* activity = out.add_activities();
        activity->set_id(id);
        activity->set_starttime(1);
        activity->set_endtime(INT_MAX);
    }
}
