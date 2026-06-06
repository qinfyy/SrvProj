#include "BattlePass.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool BattlePassRes::LoadFromPb(std::string data)
{
    BattlePass bp;    
    if (!bp.ParseFromString(data)) {
        return false;
    }
    Id = bp.id();
    return true;
}

bool BattlePassLevelRes::LoadFromPb(std::string data)
{
    BattlePassLevel bpl;
    if (!bpl.ParseFromString(data)) {
        return false;
    }
    ID = bpl.id();
    Exp = bpl.exp();
    Tid = bpl.tid();
    Qty = bpl.qty();

    return true;
}

bool BattlePassQuestRes::LoadFromPb(std::string data)
{
    BattlePassQuest bpq;
    if (!bpq.ParseFromString(data)) {
        return false;
    }

    Id = bpq.id();
    Type = bpq.type();
    Exp = bpq.exp();
    return true;

}

bool BattlePassRewardRes::LoadFromPb(std::string data)
{
    BattlePassReward bpr;
    if (!bpr.ParseFromString(data)) {
        return false;
    }

    ID = bpr.id();
    Level = bpr.level();
    Tid1 = bpr.tid1();
    Qty1 = bpr.qty1();

    Tid2 = bpr.tid2();
    Qty2 = bpr.qty2();

    Tid3 = bpr.tid3();
    Qty3 = bpr.qty3();

    return true;
}

