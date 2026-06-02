#include "QuestMgr.h"

#include "Player.h"
#include "../Resources/GameData.h"
#include "../Resources/BinClass/QuestRes.h"
#include "../Resources/BinClass/BattlePass.h"
#include "../Resources/BinClass/AchievementsRes.h"
#include "../Resources/BinClass/MiscRes.h"
#include "../Logger.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/notify.pb.h"
#include <google/protobuf/any.pb.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>

#undef max

namespace {
struct QuestSeed {
    uint32_t id;
    proto::QuestType type;
    uint32_t status;
    uint32_t cur;
    uint32_t max;
};

std::string BytesFrom(std::initializer_list<uint8_t> values)
{
    std::string out;
    out.reserve(values.size());

    for (uint8_t value : values) {
        out.push_back(static_cast<char>(value));
    }

    return out;
}

proto::Quest BuildQuestProto(uint32_t id, proto::QuestType type, uint32_t status, uint32_t cur, uint32_t max)
{
    proto::Quest quest;
    quest.set_id(id);
    quest.set_type(type);
    quest.set_status(status);
    quest.set_expire(0);

    auto* progress = quest.add_progress();
    progress->set_cur(cur);
    progress->set_max(max);

    return quest;
}

proto::Achievement BuildAchievementProto(uint32_t id, uint32_t cur, uint32_t max)
{
    proto::Achievement achievement;
    achievement.set_id(id);
    achievement.set_status(0);
    achievement.set_completed(0);

    auto* progress = achievement.add_progress();
    progress->set_cur(cur);
    progress->set_max(max);

    return achievement;
}

proto::SigninRewardUpdate BuildSigninRewardUpdate()
{
    proto::SigninRewardUpdate update;
    update.set_index(1);
    update.set_switch_(true);

    auto* item = update.mutable_change()->add_props();
    proto::Item rewardItem;
    rewardItem.set_tid(30003);
    rewardItem.set_qty(2);
    item->PackFrom(rewardItem);

    return update;
}

proto::HandbookInfo BuildHandbookInfo(uint32_t type, const std::string& data)
{
    proto::HandbookInfo info;
    info.set_type(type);
    info.set_data(data);
    return info;
}

bool HasQuestResource(uint32_t id, proto::QuestType type)
{
    const auto key = std::to_string(id);

    switch (type)
    {
    case proto::Daily:
        return GameData::DailyQuestDataTable.find(key) != GameData::DailyQuestDataTable.end();
    case proto::Weekly:
        return GameData::WeeklyQuestDataTable.find(key) != GameData::WeeklyQuestDataTable.end();
    case proto::BattlePassDaily:
    case proto::BattlePassWeekly:
        return GameData::BattlePassQuestDataTable.find(key) != GameData::BattlePassQuestDataTable.end();
    default:
        return true;
    }
}

void ValidateQuestSeed(const QuestSeed& seed)
{
    if (!HasQuestResource(seed.id, seed.type))
    {
        LOG_WARNING("Quest resource missing: id={}, type={}", seed.id, static_cast<int>(seed.type));
    }
}

constexpr QuestSeed kDefaultQuests[] = {
    {2002, proto::Daily, 0, 0, 1},
    {2003, proto::Daily, 0, 0, 1},
    {2001, proto::Daily, 0, 0, 1},
    {2004, proto::Daily, 0, 0, 1},
    {2005, proto::Daily, 0, 0, 1},
    {1005, proto::Weekly, 0, 0, 10},
    {1004, proto::Weekly, 0, 0, 5},
    {1007, proto::Weekly, 0, 0, 40},
    {1006, proto::Weekly, 0, 0, 20},
    {1001, proto::Weekly, 1, 1, 1},
    {1003, proto::Weekly, 0, 0, 3},
    {1002, proto::Weekly, 1, 1, 5},
    {1002, proto::Daily, 0, 0, 1},
    {1003, proto::Daily, 0, 0, 5},
    {1001, proto::Daily, 1, 1, 1},
    {1006, proto::Daily, 0, 0, 1},
    {1007, proto::Daily, 0, 0, 1},
    {1004, proto::Daily, 0, 0, 100},
    {1005, proto::Daily, 0, 0, 1},
    {1010, proto::Daily, 0, 0, 1},
    {1008, proto::Daily, 0, 0, 1},
    {1009, proto::Daily, 0, 0, 1},
    {1013, proto::Weekly, 0, 0, 3},
    {1012, proto::Weekly, 0, 0, 10},
    {1009, proto::Weekly, 0, 0, 1000},
    {1008, proto::Weekly, 0, 0, 500},
    {1011, proto::Weekly, 0, 0, 5},
    {1010, proto::Weekly, 0, 0, 5},
};

constexpr QuestSeed kQuestNotifySeeds[] = {
    {1008, proto::Daily, 0, 0, 1},
    {1006, proto::Daily, 0, 0, 1},
    {1004, proto::Daily, 0, 0, 100},
    {1010, proto::Daily, 0, 0, 1},
    {2002, proto::Daily, 0, 0, 1},
    {1002, proto::Daily, 0, 0, 1},
    {2001, proto::Daily, 0, 0, 1},
    {2003, proto::Daily, 0, 0, 1},
    {2005, proto::Daily, 0, 0, 1},
    {2004, proto::Daily, 0, 0, 1},
    {1009, proto::Daily, 0, 0, 1},
    {1005, proto::Daily, 0, 0, 1},
    {1001, proto::Daily, 0, 0, 1},
    {1003, proto::Daily, 0, 0, 5},
    {1007, proto::Daily, 0, 0, 1},
    {1008, proto::Weekly, 0, 0, 500},
    {1006, proto::Weekly, 0, 0, 20},
    {1004, proto::Weekly, 0, 0, 5},
    {1010, proto::Weekly, 0, 0, 5},
    {1012, proto::Weekly, 0, 0, 10},
    {1002, proto::Weekly, 0, 0, 5},
    {1009, proto::Weekly, 0, 0, 1000},
    {1011, proto::Weekly, 0, 0, 5},
    {1005, proto::Weekly, 0, 0, 10},
    {1001, proto::Weekly, 0, 0, 1},
    {1003, proto::Weekly, 0, 0, 3},
    {1013, proto::Weekly, 0, 0, 3},
    {1007, proto::Weekly, 0, 0, 40},
};

constexpr QuestSeed kBattlePassNotifySeeds[] = {
    {1004, proto::BattlePassDaily, 0, 0, 5},
    {2008, proto::BattlePassWeekly, 0, 0, 1200},
    {2002, proto::BattlePassWeekly, 0, 0, 3},
    {1002, proto::BattlePassDaily, 0, 0, 160},
    {2001, proto::BattlePassWeekly, 0, 0, 1},
    {2003, proto::BattlePassWeekly, 0, 0, 20},
    {2006, proto::BattlePassWeekly, 0, 0, 100000},
    {2007, proto::BattlePassWeekly, 0, 0, 5},
    {2005, proto::BattlePassWeekly, 0, 0, 3},
    {2004, proto::BattlePassWeekly, 0, 0, 5},
    {1001, proto::BattlePassDaily, 0, 0, 1},
    {1003, proto::BattlePassDaily, 0, 0, 6},
};

constexpr QuestSeed kFinalQuestOverrides[] = {
    {1001, proto::Weekly, 1, 1, 1},
    {1002, proto::Weekly, 0, 1, 5},
    {1001, proto::Daily, 1, 1, 1},
    {2004, proto::BattlePassWeekly, 0, 1, 5},
    {1001, proto::BattlePassDaily, 1, 1, 1},
};

constexpr std::array<uint32_t, 10> kAchievementIds1 = {8, 9, 10, 11, 12, 388, 389, 390, 391, 392};
constexpr std::array<uint32_t, 10> kAchievementCur = {1, 1, 1, 1, 1, 3, 3, 3, 3, 3};
constexpr std::array<uint32_t, 10> kAchievementMax = {10, 20, 30, 50, 100, 10, 20, 30, 40, 50};
}

ServerProto::QuestCompBin* QuestMgr::MutableBin()
{
    return GetPlayer()->SaveData().mutable_questcomp();
}

const ServerProto::QuestCompBin& QuestMgr::Bin() const
{
    return GetPlayer()->SaveData().questcomp();
}

void QuestMgr::InitializeDefaultQuests(bool markFirstLoginDone)
{
    auto* bin = MutableBin();
    bin->clear_quests();

    for (const auto& seed : kDefaultQuests)
    {
        ValidateQuestSeed(seed);

        auto* quest = bin->add_quests();
        quest->set_id(seed.id);
        quest->set_type(static_cast<uint32_t>(seed.type));
        quest->set_status(seed.status);
        quest->set_expire(0);

        auto* progress = quest->add_progress();
        progress->set_cur(seed.cur);
        progress->set_max(seed.max);
    }

    bin->set_firstloginnotifydone(markFirstLoginDone);
}

void QuestMgr::OnCreate()
{
    InitializeDefaultQuests(false);
}

void QuestMgr::OnLoad()
{
    if (!GetPlayer())
    {
        return;
    }

    const bool hadQuestComp = GetPlayer()->SaveData().has_questcomp();
    auto* bin = MutableBin();

    if (!hadQuestComp || bin->quests_size() == 0)
    {
        InitializeDefaultQuests(true);
        return;
    }

    bin->set_firstloginnotifydone(true);
}

void QuestMgr::PushFirstLoginNotifications()
{
    auto* player = GetPlayer();
    if (!player || !player->GetSessionRef())
    {
        return;
    }

    for (const auto& seed : kQuestNotifySeeds)
    {
        ValidateQuestSeed(seed);
        player->PushNextPackage(quest_change_notify, BuildQuestProto(seed.id, seed.type, seed.status, seed.cur, seed.max));
    }

    for (const auto& seed : kBattlePassNotifySeeds)
    {
        ValidateQuestSeed(seed);
        player->PushNextPackage(quest_change_notify, BuildQuestProto(seed.id, seed.type, seed.status, seed.cur, seed.max));
    }

    for (const auto& seed : kFinalQuestOverrides)
    {
        ValidateQuestSeed(seed);
        player->PushNextPackage(quest_change_notify, BuildQuestProto(seed.id, seed.type, seed.status, seed.cur, seed.max));
    }

    for (std::size_t i = 0; i < kAchievementIds1.size(); ++i)
    {
        const auto id = kAchievementIds1[i];
        const auto progress = BuildAchievementProto(id, kAchievementCur[i], kAchievementMax[i]);
        if (GameData::AchievementDataTable.find(std::to_string(id)) == GameData::AchievementDataTable.end())
        {
            LOG_WARNING("Achievement resource missing: id={}", id);
        }
        player->PushNextPackage(achievement_change_notify, progress);
    }

    player->PushNextPackage(signin_reward_change_notify, BuildSigninRewardUpdate());

    if (GameData::HandbookDataTable.find("1") == GameData::HandbookDataTable.end())
    {
        LOG_WARNING("Handbook resource missing: type=1");
    }
    if (GameData::HandbookDataTable.find("2") == GameData::HandbookDataTable.end())
    {
        LOG_WARNING("Handbook resource missing: type=2");
    }

    player->PushNextPackage(handbook_change_notify, BuildHandbookInfo(1, BytesFrom({0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x01})));
    player->PushNextPackage(handbook_change_notify, BuildHandbookInfo(2, BytesFrom({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00})));
}

void QuestMgr::OnLogin()
{
    auto* bin = MutableBin();
    if (bin->firstloginnotifydone())
    {
        return;
    }

    bin->set_firstloginnotifydone(true);
    PushFirstLoginNotifications();
}

void QuestMgr::EncodePlayerInfo(proto::PlayerInfo& out) const
{
    auto* quests = out.mutable_quests();

    for (const auto& quest : Bin().quests())
    {
        quests->add_list()->CopyFrom(BuildQuestProto(
            quest.id(),
            static_cast<proto::QuestType>(quest.type()),
            quest.status(),
            quest.progress_size() > 0 ? quest.progress(0).cur() : 0,
            quest.progress_size() > 0 ? quest.progress(0).max() : 0));
    }

    out.set_tourguidequestgroup(9);
}
