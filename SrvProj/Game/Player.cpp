#include "Player.h"

#include "ActivityMgr.h"
#include "AchievementMgr.h"
#include "BattlePassMgr.h"
#include "Bitset.h"
#include "ChangeInfoUtil.h"
#include "CharacterMgr.h"
#include "../Config.h"
#include "FormationMgr.h"
#include "GachaMgr.h"
#include "InventoryMgr.h"
#include "MailMgr.h"
#include "QuestMgr.h"
#include "TowerMgr.h"
#include "../GameConstants.h"
#include "../GameSession.h"
#include "../GameTime.h"
#include "../Resources/BinClass/MiscRes.h"
#include "../Resources/BinClass/ShopsRes.h"
#include "../Resources/GameData.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/notify.pb.h"
#include "../proto/proto_cpp/notify_gm.pb.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <initializer_list>
#include <unordered_set>
#include <utility>
#include "../DbMgr.h"

namespace {
std::string BuildHandbookFlag(uint32_t type, std::initializer_list<uint32_t> handbookIds)
{
    Bitset bitset;
    for (uint32_t id : handbookIds)
    {
        const auto it = GameData::HandbookDataTable.find(id);
        if (it == GameData::HandbookDataTable.end())
        {
            continue;
        }
        if (static_cast<uint32_t>(std::max(it->second.Type, 0)) != type)
        {
            continue;
        }
        bitset.SetBit(static_cast<uint32_t>(std::max(it->second.Index, 0)));
    }

    return bitset.ToByteArray();
}

void AddCompletedNewbies(proto::AccInfo* acc)
{
    static constexpr uint32_t newbieGroups[] = {
        25, 49, 50, 8, 9, 232, 24, 26, 16, 17, 23, 18, 106, 229, 303, 27,
        47, 48, 51, 304, 302, 32, 52, 201, 46, 41, 45, 44, 42, 43, 301, 29,
        202, 4, 12, 13, 28, 102, 21, 22, 20, 104, 105, 101, 2, 6, 15, 14,
        11, 10, 3, 7, 5, 1
    };

    for (uint32_t groupId : newbieGroups)
    {
        auto* newbie = acc->add_newbies();
        newbie->set_groupid(groupId);
        newbie->set_stepid(-1);
    }
}

bool MatchShopCondition(const Player& player, int condType, const std::vector<int>& params)
{
    if (condType == 0)
    {
        return true;
    }

    if (condType == 71)
    {
        const int requiredLevel = params.empty() ? 0 : params.front();
        return player.GetPlayerData().level() >= requiredLevel;
    }

    return false;
}

bool IsPackageVisibleForPlayer(const Player& player, const MallPackageRes& data)
{
    const int64_t now = GameTime::ServerNowSeconds();
    if (data.ListTimeSeconds > 0 && now < data.ListTimeSeconds)
    {
        return false;
    }
    if (data.DeListTimeSeconds > 0 && now >= data.DeListTimeSeconds)
    {
        return false;
    }

    return MatchShopCondition(player, data.ListCondType, data.ListCond);
}

bool CanPurchasePackage(const Player& player, const MallPackageRes& data)
{
    if (!IsPackageVisibleForPlayer(player, data))
    {
        return false;
    }
    if (data.Stock > 0 && player.Inventory().GetMallPackagePurchaseCount(data.Id) >= static_cast<uint32_t>(data.Stock))
    {
        return false;
    }

    return MatchShopCondition(player, data.OrderCondType, data.OrderCond);
}

std::vector<std::string> SplitPermission(const std::string& permission)
{
    std::vector<std::string> parts;
    size_t start = 0;

    while (start <= permission.size())
    {
        const size_t pos = permission.find('.', start);
        if (pos == std::string::npos)
        {
            parts.push_back(permission.substr(start));
            break;
        }

        parts.push_back(permission.substr(start, pos - start));
        start = pos + 1;
    }

    return parts;
}

int64_t GetWeekResetBucket(uint32_t epochDay)
{
    using namespace std::chrono;
    const sys_days day{ days{ static_cast<int64_t>(epochDay) } };
    const int isoWeekDay = weekday{ day }.iso_encoding();
    return (static_cast<int64_t>(epochDay) - (isoWeekDay - 1)) / 7;
}

int64_t GetMonthResetBucket(uint32_t epochDay)
{
    using namespace std::chrono;
    const sys_days day{ days{ static_cast<int64_t>(epochDay) } };
    const year_month_day ymd{ day };
    return (static_cast<int>(ymd.year()) * 12LL) + static_cast<unsigned>(ymd.month());
}
}

Player::~Player() = default;

Player::Player(GameSession* sessionRef)
{
    mSessionRef = sessionRef;
    InitManagers();
}

void Player::InitManagers()
{
    mCharacterStor = std::make_unique<CharacterStor>(this);
    mActivityMgr = std::make_unique<ActivityMgr>(this);
    mAchievementMgr = std::make_unique<AchievementMgr>(this);
    mInventoryMgr = std::make_unique<InventoryMgr>(this);
    mGachaMgr = std::make_unique<GachaMgr>(this);
    mMailMgr = std::make_unique<MailMgr>(this);
    mBattlePassMgr = std::make_unique<BattlePassMgr>(this);
    mFormationMgr = std::make_unique<FormationMgr>(this);
    mQuestMgr = std::make_unique<QuestMgr>(this);
    mTowerMgr = std::make_unique<TowerMgr>(this);
}

bool Player::Save()
{
    auto saveData = SaveToBlob();
    return DbMgr::Instance().SavePlayer(GetUid(), std::span<const uint8_t>(saveData.data(), saveData.size()));
}

bool Player::InitNewPlayer(uint32_t uid, std::string name, bool gender)
{
    mUid = uid;
    mPlayerSaveData.Clear();

    auto* data = GetMutablePlayerData();
    const int64_t now = GameTime::NowSeconds();

    data->set_createtime(now);
    data->set_name(name.empty() ? "me" : std::move(name));
    data->set_signature("");
    data->set_gender(gender);
    data->set_headicon(gender ? 101 : 102);
    data->set_skinid(10301);
    data->set_titleprefix(1);
    data->set_titlesuffix(2);
    data->clear_boards();
    data->add_boards(410301);
	data->set_level(1);
    data->set_exp(0);
    data->set_energy(GameConstants::MaxEnergy);
    data->set_energylastupdate(now);
    data->set_signinindex(1);
    data->set_lastepochday(0);
    data->set_lastlogin(now);
    data->clear_permissions();
    for (const auto& permission : Config::Get().playerDefaultPermissions)
    {
        if (!permission.empty())
        {
            data->add_permissions(permission);
        }
    }

    data->clear_showchars();
    data->add_showchars(0);
    data->add_showchars(0);
    data->add_showchars(0);

    Characters().AddCharacterFromId(103);
    Characters().AddCharacterFromId(112);
    Characters().AddCharacterFromId(113);

    Characters().AddDiscFromId(211001);
    Characters().AddDiscFromId(211005);
    Characters().AddDiscFromId(211007);
    Characters().AddDiscFromId(211008);

    OnCreate();
    return true;
}

void Player::OnCreate()
{
    mCharacterStor->OnCreate();
    mActivityMgr->OnCreate();
    mAchievementMgr->OnCreate();
    mInventoryMgr->OnCreate();
    mGachaMgr->OnCreate();
    mMailMgr->OnCreate();
    mBattlePassMgr->OnCreate();
    mFormationMgr->OnCreate();
    mQuestMgr->OnCreate();
    mTowerMgr->OnCreate();
}

bool Player::LoadFromBlob(uint32_t uid, std::span<const uint8_t> data)
{
    mUid = uid;
    mPlayerSaveData.Clear();

    if (data.empty())
    {
        return false;
    }

    if (!mPlayerSaveData.ParseFromArray(data.data(), static_cast<int>(data.size())))
    {
        return false;
    }

    mCharacterStor->OnLoad();
    mActivityMgr->OnLoad();
    mAchievementMgr->OnLoad();
    mInventoryMgr->OnLoad();
    mGachaMgr->OnLoad();
    mMailMgr->OnLoad();
    mBattlePassMgr->OnLoad();
    mFormationMgr->OnLoad();
    mQuestMgr->OnLoad();
    mTowerMgr->OnLoad();
    return true;
}

std::vector<uint8_t> Player::SaveToBlob() const
{
    mCharacterStor->BeforeSave();
    mActivityMgr->BeforeSave();
    mAchievementMgr->BeforeSave();
    mInventoryMgr->BeforeSave();
    mGachaMgr->BeforeSave();
    mMailMgr->BeforeSave();
    mBattlePassMgr->BeforeSave();
    mFormationMgr->BeforeSave();
    mQuestMgr->BeforeSave();
    mTowerMgr->BeforeSave();

    std::vector<uint8_t> out(mPlayerSaveData.ByteSizeLong());
    if (!out.empty())
    {
        mPlayerSaveData.SerializeToArray(out.data(), static_cast<int>(out.size()));
    }
    return out;
}

proto::PlayerInfo Player::ToProto()
{
    proto::PlayerInfo info;

    EncodeBasicInfo(info);
    Characters().EncodePlayerInfo(info);
    mActivityMgr->EncodePlayerInfo(info);
    mInventoryMgr->EncodePlayerInfo(info);
    mMailMgr->EncodePlayerInfo(info);
    mBattlePassMgr->EncodePlayerInfo(info);
    mFormationMgr->EncodePlayerInfo(info);
    mQuestMgr->EncodePlayerInfo(info);
    mTowerMgr->EncodePlayerInfo(info);
    EncodeMinimalSystems(info);

    return info;
}

void Player::OnLogin()
{
    auto* data = GetMutablePlayerData();
    const uint32_t oldLevel = static_cast<uint32_t>((std::max)(data->level(), 0));

    CheckResetDailies();
    data->set_lastlogin(GameTime::NowSeconds());

    mCharacterStor->OnLogin();
    mActivityMgr->OnLogin();
    mAchievementMgr->OnLogin();
    mInventoryMgr->OnLogin();
    mGachaMgr->OnLogin();
    mMailMgr->OnLogin();
    mBattlePassMgr->OnLogin();
    mFormationMgr->OnLogin();
    mQuestMgr->OnLogin();
    mTowerMgr->OnLogin();
    QueueBattlePassUnlockNotify(oldLevel);
}

void Player::PushNextPackage(short msgId, std::unique_ptr<google::protobuf::Message> payload)
{
    if (!mSessionRef || !payload) {
        return;
    }

    mSessionRef->PushNextPackageImpl(msgId, std::move(payload));
}

int32_t Player::GetEnergy()
{
    auto* data = GetMutablePlayerData();
    const int64_t now = GameTime::NowSeconds();

    if (data->energylastupdate() <= 0)
    {
        data->set_energylastupdate(now);
    }

    const int32_t currentEnergy = std::clamp(data->energy(), 0, GameConstants::MaxEnergy);
    if (currentEnergy != data->energy())
    {
        data->set_energy(currentEnergy);
    }

    if (data->energy() >= GameConstants::MaxEnergy)
    {
        data->set_energylastupdate(now);
        return data->energy();
    }

    const int64_t diff = now - data->energylastupdate();
    if (diff < GameConstants::EnergyRegenTime)
    {
        return data->energy();
    }

    const int64_t bonusEnergy = diff / GameConstants::EnergyRegenTime;
    const int64_t nextEnergy = static_cast<int64_t>(data->energy()) + bonusEnergy;
    data->set_energy(static_cast<int32_t>(std::min<int64_t>(nextEnergy, GameConstants::MaxEnergy)));
    data->set_energylastupdate(data->energylastupdate() + (bonusEnergy * GameConstants::EnergyRegenTime));

    if (data->energy() >= GameConstants::MaxEnergy)
    {
        data->set_energylastupdate(now);
    }

    return data->energy();
}

int64_t Player::GetEnergyLastUpdate()
{
    GetEnergy();
    return GetPlayerData().energylastupdate();
}

proto::Energy Player::GetEnergyProto()
{
    const int32_t energy = GetEnergy();
    const int64_t now = GameTime::NowSeconds();
    const int64_t elapsed = now - GetPlayerData().energylastupdate();
    const int64_t nextDuration = std::max<int64_t>(GameConstants::EnergyRegenTime - elapsed, 1);

    proto::Energy proto;
    proto.set_primary(static_cast<uint32_t>(energy));
    proto.set_isprimary(true);
    proto.set_updatetime(GetPlayerData().energylastupdate());
    proto.set_nextduration(nextDuration);
    return proto;
}

bool Player::AddEnergy(int32_t amount)
{
    if (amount <= 0)
    {
        return false;
    }

    auto* data = GetMutablePlayerData();
    GetEnergy();

    const int64_t nextEnergy = static_cast<int64_t>(data->energy()) + amount;
    data->set_energy(static_cast<int32_t>(std::min<int64_t>(nextEnergy, GameConstants::MaxEnergy)));
    return true;
}

bool Player::ConsumeEnergy(int32_t amount)
{
    if (amount <= 0)
    {
        return false;
    }

    auto* data = GetMutablePlayerData();
    if (GetEnergy() < amount)
    {
        return false;
    }

    data->set_energy(data->energy() - amount);
    if (data->energylastupdate() <= 0 || data->energy() == GameConstants::MaxEnergy - amount)
    {
        data->set_energylastupdate(GameTime::NowSeconds());
    }

    Trigger(39, static_cast<uint32_t>(amount));
    return true;
}

ServerProto::PlayerSaveData& Player::SaveData()
{
    return mPlayerSaveData;
}

const ServerProto::PlayerSaveData& Player::SaveData() const
{
    return mPlayerSaveData;
}

CharacterStor& Player::Characters()
{
    return *mCharacterStor;
}

const CharacterStor& Player::Characters() const
{
    return *mCharacterStor;
}

QuestMgr& Player::Quests()
{
    return *mQuestMgr;
}

const QuestMgr& Player::Quests() const
{
    return *mQuestMgr;
}

BattlePassMgr& Player::BattlePasses()
{
    return *mBattlePassMgr;
}

const BattlePassMgr& Player::BattlePasses() const
{
    return *mBattlePassMgr;
}

FormationMgr& Player::Formations()
{
    return *mFormationMgr;
}

const FormationMgr& Player::Formations() const
{
    return *mFormationMgr;
}

TowerMgr& Player::Towers()
{
    return *mTowerMgr;
}

const TowerMgr& Player::Towers() const
{
    return *mTowerMgr;
}

AchievementMgr& Player::Achievements()
{
    return *mAchievementMgr;
}

const AchievementMgr& Player::Achievements() const
{
    return *mAchievementMgr;
}

InventoryMgr& Player::Inventory()
{
    return *mInventoryMgr;
}

const InventoryMgr& Player::Inventory() const
{
    return *mInventoryMgr;
}

GachaMgr& Player::Gachas()
{
    return *mGachaMgr;
}

const GachaMgr& Player::Gachas() const
{
    return *mGachaMgr;
}

MailMgr& Player::Mails()
{
    return *mMailMgr;
}

const MailMgr& Player::Mails() const
{
    return *mMailMgr;
}

void Player::Trigger(uint32_t condition, uint32_t progress, uint32_t param1, uint32_t param2)
{
    mQuestMgr->Trigger(condition, progress, param1, param2);
    mBattlePassMgr->Trigger(condition, progress, param1, param2);
    mAchievementMgr->Trigger(condition, progress, param1, param2);
}

ServerProto::PlayerBasicCompBin* Player::GetMutablePlayerData()
{
    return mPlayerSaveData.mutable_playerdata();
}

const ServerProto::PlayerBasicCompBin& Player::GetPlayerData() const
{
    return mPlayerSaveData.playerdata();
}

uint32_t Player::GetUid() const
{
    return mUid;
}

void Player::SetUid(uint32_t uid)
{
    mUid = uid;
}

bool Player::SetWorldLevel(uint32_t level)
{
    if (level == 0)
    {
        return false;
    }

    if (!GameData::WorldClassDataTable.empty() && GameData::WorldClassDataTable.find(level) == GameData::WorldClassDataTable.end())
    {
        return false;
    }

    auto* data = GetMutablePlayerData();
    const uint32_t oldLevel = static_cast<uint32_t>((std::max)(data->level(), 0));
    data->set_level(static_cast<int32_t>(level));
    data->set_exp(0);

    proto::GmWorldClass notify;
    notify.set_finalclass(level);
    notify.set_lastexp(0);
    PushNextPackage(world_class_number_notify, notify);

    Trigger(71, level, level, 0);
    QueueBattlePassUnlockNotify(oldLevel);
    return true;
}

void Player::SetSignature(const std::string& signature)
{
    GetMutablePlayerData()->set_signature(signature);
}

std::vector<std::string> Player::GetPermissions() const
{
    std::vector<std::string> permissions;
    permissions.reserve(static_cast<size_t>(GetPlayerData().permissions_size()));

    for (const auto& permission : GetPlayerData().permissions())
    {
        permissions.push_back(permission);
    }

    return permissions;
}

bool Player::AddPermission(const std::string& permission)
{
    if (permission.empty())
    {
        return false;
    }

    auto* data = GetMutablePlayerData();
    for (const auto& value : data->permissions())
    {
        if (value == permission)
        {
            return false;
        }
    }

    data->add_permissions(permission);
    return true;
}

bool Player::RemovePermission(const std::string& permission)
{
    if (permission.empty())
    {
        return false;
    }

    auto* data = GetMutablePlayerData();
    for (int i = 0; i < data->permissions_size(); ++i)
    {
        if (data->permissions(i) != permission)
        {
            continue;
        }

        data->mutable_permissions()->DeleteSubrange(i, 1);
        return true;
    }

    return false;
}

void Player::ClearPermissions()
{
    GetMutablePlayerData()->clear_permissions();
}

bool Player::PermissionMatchesWildcard(const std::string& wildcard, const std::vector<std::string>& permissionParts)
{
    const auto wildcardParts = SplitPermission(wildcard);
    if (permissionParts.size() < wildcardParts.size())
    {
        return false;
    }

    for (size_t i = 0; i < wildcardParts.size(); ++i)
    {
        if (wildcardParts[i] == "**")
        {
            return true;
        }

        if (wildcardParts[i] == "*")
        {
            if (i >= permissionParts.size() - 1)
            {
                return true;
            }
            continue;
        }

        if (wildcardParts[i] != permissionParts[i])
        {
            return false;
        }
    }

    return wildcardParts.size() == permissionParts.size();
}

bool Player::HasPermission(const std::string& permission) const
{
    if (permission.empty())
    {
        return true;
    }

    std::vector<std::string> permissions = GetPermissions();

    if (std::find(permissions.begin(), permissions.end(), permission) != permissions.end())
    {
        return true;
    }

    const auto permissionParts = SplitPermission(permission);
    for (const auto& value : permissions)
    {
        if (!value.empty() && value[0] == '-' && PermissionMatchesWildcard(value.substr(1), permissionParts))
        {
            return false;
        }

        if (PermissionMatchesWildcard(value, permissionParts))
        {
            return true;
        }
    }

    return std::find(permissions.begin(), permissions.end(), "*") != permissions.end();
}

bool Player::HasAvailableFreeMallPackage() const
{
    for (const auto& [_, data] : GameData::MallPackageDataTable)
    {
        if (data.CurrencyType != GameConstants::CurrencyTypeFree)
        {
            continue;
        }

        if (CanPurchasePackage(*this, data))
        {
            return true;
        }
    }

    return false;
}

bool Player::IsBattlePassUnlocked() const
{
    return static_cast<uint32_t>((std::max)(GetPlayerData().level(), 0)) >= GameConstants::BattlePassUnlockLevel;
}

void Player::QueueMallPackageStateNotify()
{
    proto::MallPackageState state;
    state.set_new_(HasAvailableFreeMallPackage());
    PushNextPackage(mall_package_state_notify, state);
}

void Player::QueueBattlePassStateNotify()
{
    PushNextPackage(battle_pass_state_notify, BuildBattlePassStateProto());
}

void Player::QueueBattlePassInfoNotify()
{
    PushNextPackage(battle_pass_info_succeed_ack, BattlePasses().ToProto());
}

uint32_t Player::GetMonthlyCardRemainingDays(const std::string& cardId) const
{
    if (cardId.empty())
    {
        return 0;
    }

    const auto& days = GetPlayerData().monthlycardexpiredays();
    const auto it = days.find(cardId);
    if (it == days.end())
    {
        return 0;
    }

    const uint32_t today = GameTime::CurrentEpochDay();
    if (it->second < today)
    {
        return 0;
    }

    return it->second - today;
}

bool Player::ReceivedMonthlyCardRewardToday(const std::string& cardId) const
{
    if (cardId.empty())
    {
        return false;
    }

    const auto& rewardDays = GetPlayerData().monthlycardlastrewarddays();
    const auto it = rewardDays.find(cardId);
    return it != rewardDays.end() && it->second >= GameTime::CurrentEpochDay();
}

int64_t Player::GetMonthlyCardEndTime(const std::string& cardId) const
{
    if (cardId.empty())
    {
        return 0;
    }

    const auto& days = GetPlayerData().monthlycardexpiredays();
    const auto it = days.find(cardId);
    if (it == days.end() || it->second == 0)
    {
        return 0;
    }

    return GameTime::ResetTimeSecondsByEpochDay(it->second + 1);
}

void Player::ActivateMonthlyCard(const std::string& cardId, uint32_t durationDays)
{
    if (cardId.empty() || durationDays == 0)
    {
        return;
    }

    const uint32_t today = GameTime::CurrentEpochDay();
    const uint32_t remainingDays = GetMonthlyCardRemainingDays(cardId);
    const bool claimedToday = ReceivedMonthlyCardRewardToday(cardId);
    const uint32_t newRemainingDays = (remainingDays > 0 || claimedToday)
        ? remainingDays + durationDays
        : (durationDays > 0 ? durationDays - 1 : 0);

    (*GetMutablePlayerData()->mutable_monthlycardexpiredays())[cardId] = today + newRemainingDays;
}

bool Player::CanClaimMonthlyCardReward(const std::string& cardId) const
{
    if (cardId.empty())
    {
        return false;
    }

    const auto& days = GetPlayerData().monthlycardexpiredays();
    const auto it = days.find(cardId);
    if (it == days.end())
    {
        return false;
    }

    const uint32_t today = GameTime::CurrentEpochDay();
    return it->second > 0 && today <= it->second && !ReceivedMonthlyCardRewardToday(cardId);
}

bool Player::CreateMonthlyCardRewardChange(const std::string& cardId, proto::ChangeInfo& out)
{
    if (!CanClaimMonthlyCardReward(cardId))
    {
        return false;
    }

    auto cardIt = GameData::MallMonthlyCardDataTable.find(cardId);
    if (cardIt == GameData::MallMonthlyCardDataTable.end())
    {
        return false;
    }

    const int monthlyCardId = cardIt->second.MonthlyCardId;
    const auto rewardIt = GameData::MonthlyCardDataTable.find(monthlyCardId);
    if (rewardIt == GameData::MonthlyCardDataTable.end() || rewardIt->second.Rewards.Empty())
    {
        return false;
    }

    if (!Inventory().AddItems(rewardIt->second.Rewards, &out))
    {
        return false;
    }

    (*GetMutablePlayerData()->mutable_monthlycardlastrewarddays())[cardId] = GameTime::CurrentEpochDay();
    return true;
}

bool Player::GrantMonthlyCardReward(const std::string& cardId, bool notifyOnly)
{
    auto cardIt = GameData::MallMonthlyCardDataTable.find(cardId);
    if (cardIt == GameData::MallMonthlyCardDataTable.end())
    {
        return false;
    }

    proto::ChangeInfo change;
    if (!CreateMonthlyCardRewardChange(cardId, change))
    {
        return false;
    }

    proto::MonthlyCardRewards notify;
    notify.set_id(static_cast<uint32_t>(cardIt->second.MonthlyCardId));
    notify.set_remaining(GetMonthlyCardRemainingDays(cardId));
    notify.set_switch_(!notifyOnly);
    notify.set_endtime(GetMonthlyCardEndTime(cardId));
    notify.mutable_change()->CopyFrom(change);

    const auto rewardIt = GameData::MonthlyCardDataTable.find(cardIt->second.MonthlyCardId);
    if (rewardIt != GameData::MonthlyCardDataTable.end())
    {
        for (const auto& [tid, qty] : rewardIt->second.Rewards.Items)
        {
            ChangeInfoUtil::AddItemTpl(notify.add_rewards(), static_cast<uint32_t>(tid), qty);
        }
    }

    PushNextPackage(monthly_card_rewards_notify, notify);
    Inventory().PushItemsChange(change);
    return true;
}

void Player::RefreshMonthlyCardRewards(bool notifyOnly)
{
    std::vector<std::string> cardIds;
    cardIds.reserve(static_cast<size_t>(GetPlayerData().monthlycardexpiredays_size()));
    for (const auto& [cardId, _] : GetPlayerData().monthlycardexpiredays())
    {
        cardIds.push_back(cardId);
    }

    for (const auto& cardId : cardIds)
    {
        GrantMonthlyCardReward(cardId, notifyOnly);
    }
}

void Player::CheckResetDailies()
{
    auto* data = GetMutablePlayerData();
    const uint32_t currentDay = GameTime::CurrentEpochDay();
    const uint32_t lastDay = data->lastepochday() < 0 ? 0u : static_cast<uint32_t>(data->lastepochday());

    if (currentDay <= lastDay)
    {
        if (data->signinindex() <= 0)
        {
            data->set_signinindex(1);
        }

        RefreshMonthlyCardRewards(false);
        return;
    }

    const bool hasWeekChanged = GetWeekResetBucket(currentDay) > GetWeekResetBucket(lastDay);
    const bool hasMonthChanged = GetMonthResetBucket(currentDay) > GetMonthResetBucket(lastDay);

    ResetDailies(hasWeekChanged, hasMonthChanged);
    Trigger(51, 1, 0, 0);
    RefreshMonthlyCardRewards(true);

    data->set_lastepochday(currentDay);
}

void Player::ResetDailies(bool resetWeekly, bool resetMonthly)
{
    Quests().ResetDailyQuests(resetWeekly);
    BattlePasses().ResetDailyQuests(resetWeekly);

    const int64_t tickets = Inventory().GetResourceCount(GameConstants::JointDrillTicketId);
    if (tickets < 3)
    {
        Inventory().AddItem(GameConstants::JointDrillTicketId, 3 - tickets);
    }

    if (resetWeekly)
    {
        const int64_t entries = Inventory().GetResourceCount(GameConstants::WeeklyEntryItemId);
        if (entries < 3)
        {
            Inventory().AddItem(GameConstants::WeeklyEntryItemId, 3 - entries);
        }

        Towers().ResetWeeklyTickets();
    }

    if (resetMonthly)
    {
        // 当前 SrvProj 还没有完整的月度商店/通行证购买重置模块，这里保持占位。
    }
}

proto::BattlePassState Player::BuildBattlePassStateProto() const
{
    proto::BattlePassState state;
    state.set_state(IsBattlePassUnlocked() ? BattlePasses().GetClientState() : 0);
    return state;
}

void Player::QueueBattlePassUnlockNotify(uint32_t oldLevel)
{
    if (oldLevel < GameConstants::BattlePassUnlockLevel && IsBattlePassUnlocked())
    {
        QueueBattlePassInfoNotify();
    }
}

void Player::EncodeBasicInfo(proto::PlayerInfo& info)
{
    const auto& data = GetPlayerData();

    info.set_serverts(GameTime::ServerNowSeconds());
    info.set_signinindex(data.signinindex());
    info.set_musicinfo(data.music());
    info.set_achievements(std::string(64, '\0'));
    info.set_dailyshoprewardstatus(Quests().HasDailyShopReward());
    info.set_dailymallrewardstatus(Quests().HasDailyMallReward());

    auto* acc = info.mutable_acc();
    acc->set_id(mUid);
    acc->set_nickname(data.name());
    acc->set_signature(data.signature());
    acc->set_gender(data.gender());
    acc->set_headicon(data.headicon());
    acc->set_skinid(data.skinid());
    acc->set_titleprefix(data.titleprefix());
    acc->set_titlesuffix(data.titlesuffix());
    acc->set_createtime(data.createtime());
    AddCompletedNewbies(acc);

    const int showCount = std::max(3, data.showchars_size());
    for (int i = 0; i < showCount; ++i)
    {
        const uint32_t charId = i < data.showchars_size()
            ? static_cast<uint32_t>(data.showchars(i))
            : 0;

        auto* show = acc->add_chars();
        if (charId == 0)
        {
            continue;
        }

        const auto* character = Characters().GetCharacterById(static_cast<int>(charId));
        if (character != nullptr)
        {
            show->set_charid(character->charid());
            show->set_level(character->level());
            show->set_skin(character->skin());
        }
        else
        {
            show->set_charid(charId);
        }
    }

    auto* worldClass = info.mutable_worldclass();
    worldClass->set_cur(static_cast<uint32_t>(data.level()));
    worldClass->set_lastexp(data.exp());

    info.mutable_energy()->mutable_energy()->CopyFrom(GetEnergyProto());

    for (int board : data.boards())
    {
        info.add_board(static_cast<uint32_t>(board));
    }
}

void Player::EncodeMinimalSystems(proto::PlayerInfo& info) const
{
    auto* state = info.mutable_state();
    state->mutable_mail();
    state->mutable_battlepass()->CopyFrom(BuildBattlePassStateProto());
    state->mutable_achievement()->set_new_(Achievements().HasNewAchievements());
    state->mutable_friendenergy();
    state->mutable_mallpackage();
    state->mutable_scoreboss();
    state->mutable_startower()->CopyFrom(Towers().BuildStateProto());
    state->mutable_startowerbook()->CopyFrom(Towers().BuildBookStateProto());
    state->mutable_worldclassreward()->set_flag(std::string(8, '\0'));
    state->mutable_travelerduelquest()->set_type(proto::TravelerDuel);
    state->set_storyset(true);

    info.add_titles()->set_titleid(1);
    info.add_titles()->set_titleid(2);
    info.add_honorlist(111001);

    info.mutable_agent();
    Formations().EncodePlayerInfo(info);
    info.mutable_phone()->set_newmessage(Characters().GetNewPhoneMessageCount());
    info.mutable_story();

    auto* handbookChars = info.add_handbook();
    handbookChars->set_type(1);
    handbookChars->set_data(BuildHandbookFlag(1, {410301, 410302, 410601}));

    auto* handbookDiscs = info.add_handbook();
    handbookDiscs->set_type(2);
    handbookDiscs->set_data(BuildHandbookFlag(2, {}));

    auto* handbookCg = info.add_handbook();
    handbookCg->set_type(3);
    handbookCg->set_data(BuildHandbookFlag(3, {}));
}
