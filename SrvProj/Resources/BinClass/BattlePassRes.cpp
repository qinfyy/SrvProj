#include "BattlePassRes.h"
#include "../ResourceJsonUtil.h"

#include "../../GameTime.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool BattlePassRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "StartTimeText", StartTimeText);
    ReadResourceJsonField(data, "EndTimeText", EndTimeText);
    ReadResourceJsonField(data, "LuxuryProductId", LuxuryProductId);
    ReadResourceJsonField(data, "PremiumProductId", PremiumProductId);
    ReadResourceJsonField(data, "LuxuryBonusLevel", LuxuryBonusLevel);
    ReadResourceJsonField(data, "LuxuryTid", LuxuryTid);
    ReadResourceJsonField(data, "LuxuryQty", LuxuryQty);
    ReadResourceJsonField(data, "ComplementaryTid", ComplementaryTid);
    ReadResourceJsonField(data, "ComplementaryQty", ComplementaryQty);
    return true;
}

bool BattlePassRes::LoadFromPb(std::string data)
{
    BattlePass bp;
    if (!bp.ParseFromString(data)) {
        return false;
    }

    Id = bp.id();
    StartTimeText = bp.starttime();
    EndTimeText = bp.endtime();
    LuxuryProductId = bp.luxuryproductid();
    PremiumProductId = bp.premiumproductid();
    LuxuryBonusLevel = bp.luxurybonuslevel();
    LuxuryTid = bp.luxurytid();
    LuxuryQty = bp.luxuryqty();
    ComplementaryTid = bp.complementarytid();
    ComplementaryQty = bp.complementaryqty();
    return true;
}

void BattlePassRes::OnLoad()
{
    StartTime = GameTime::DateToSecondsInConfiguredTimeZone(StartTimeText);
    EndTime = GameTime::DateToSecondsInConfiguredTimeZone(EndTimeText);
}

bool BattlePassLevelRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "ID", ID);
    ReadResourceJsonField(data, "Exp", Exp);
    ReadResourceJsonField(data, "Tid", Tid);
    ReadResourceJsonField(data, "Qty", Qty);
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

bool BattlePassQuestRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "Type", Type);
    ReadResourceJsonField(data, "Exp", Exp);
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

bool BattlePassRewardRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "ID", ID);
    ReadResourceJsonField(data, "Level", Level);
    ReadResourceJsonField(data, "Tid1", Tid1);
    ReadResourceJsonField(data, "Qty1", Qty1);
    ReadResourceJsonField(data, "Tid2", Tid2);
    ReadResourceJsonField(data, "Qty2", Qty2);
    ReadResourceJsonField(data, "Tid3", Tid3);
    ReadResourceJsonField(data, "Qty3", Qty3);
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
