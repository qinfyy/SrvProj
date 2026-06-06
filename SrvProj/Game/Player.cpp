#include "Player.h"

#include "ActivityMgr.h"
#include "CharacterMgr.h"
#include "InventoryMgr.h"
#include "QuestMgr.h"
#include "../GameConstants.h"
#include "../GameSession.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <initializer_list>
#include <utility>

namespace {
int64_t NowSeconds()
{
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

int64_t NowEpochDay()
{
    return NowSeconds() / 86400;
}

std::string BytesFrom(std::initializer_list<uint8_t> values)
{
    std::string out;
    out.reserve(values.size());

    for (uint8_t value : values) {
        out.push_back(static_cast<char>(value));
    }

    return out;
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
    mInventoryMgr = std::make_unique<InventoryMgr>(this);
    mQuestMgr = std::make_unique<QuestMgr>(this);
}

bool Player::InitNewPlayer(uint32_t uid, std::string name, bool gender)
{
    mUid = uid;
    mPlayerSaveData.Clear();

    auto* data = GetMutablePlayerData();
    const int64_t now = NowSeconds();

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
	data->set_level(40); // 先设定为40级，方便测试，正式服应该是1级，现在请不要修改它
    data->set_exp(0);
    data->set_energy(GameConstants::MaxEnergy);
    data->set_energylastupdate(now);
    data->set_signinindex(1);
    data->set_lastepochday(0);
    data->set_lastlogin(now);

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
    mInventoryMgr->OnCreate();
    mQuestMgr->OnCreate();
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
    mInventoryMgr->OnLoad();
    mQuestMgr->OnLoad();
    return true;
}

std::vector<uint8_t> Player::SaveToBlob() const
{
    mCharacterStor->BeforeSave();
    mActivityMgr->BeforeSave();
    mInventoryMgr->BeforeSave();
    mQuestMgr->BeforeSave();

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
    mQuestMgr->EncodePlayerInfo(info);
    EncodeMinimalSystems(info);

    return info;
}

void Player::OnLogin()
{
    auto* data = GetMutablePlayerData();
    const int64_t now = NowSeconds();
    data->set_lastlogin(now);
    data->set_lastepochday(NowEpochDay());

    mCharacterStor->OnLogin();
    mActivityMgr->OnLogin();
    mInventoryMgr->OnLogin();
    mQuestMgr->OnLogin();
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
    const int64_t now = NowSeconds();

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
    const int64_t now = NowSeconds();
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
        data->set_energylastupdate(NowSeconds());
    }

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

void Player::EncodeBasicInfo(proto::PlayerInfo& info)
{
    const auto& data = GetPlayerData();

    info.set_serverts(NowSeconds());
    info.set_signinindex(data.signinindex());
    info.set_musicinfo(data.music());
    info.set_achievements(std::string(64, '\0'));
    info.set_dailyshoprewardstatus(true);
    info.set_dailymallrewardstatus(true);

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
    state->mutable_mail()->set_new_(true);
    state->mutable_battlepass()->set_state(1);
    state->mutable_achievement();
    state->mutable_friendenergy();
    state->mutable_mallpackage();
    state->mutable_scoreboss();
    state->mutable_startower();
    state->mutable_startowerbook();
    state->mutable_worldclassreward()->set_flag(std::string(8, '\0'));
    state->mutable_travelerduelquest()->set_type(proto::TravelerDuel);
    state->set_storyset(true);

    info.add_titles()->set_titleid(1);
    info.add_titles()->set_titleid(2);
    info.add_honorlist(111001);
    info.mutable_agent();
    info.mutable_formation();
    info.mutable_phone()->set_newmessage(Characters().GetNewPhoneMessageCount());
    info.mutable_story();

    auto* handbookChars = info.add_handbook();
    handbookChars->set_type(1);
    handbookChars->set_data(BytesFrom({0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x01}));

    auto* handbookDiscs = info.add_handbook();
    handbookDiscs->set_type(2);
    handbookDiscs->set_data(BytesFrom({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}));

    auto* handbookCg = info.add_handbook();
    handbookCg->set_type(3);
    handbookCg->set_data(BytesFrom({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}));
}
