#include "Player.h"

#include "ActivityMgr.h"
#include "CharacterMgr.h"
#include "InventoryMgr.h"
#include "QuestMgr.h"

#include <chrono>
#include <climits>
#include <utility>

Player::~Player() = default;  // 在这里定义，此时 InventoryMgr 已完整

namespace {
int64_t NowSeconds()
{
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

void AddCompletedNewbies(proto::AccInfo* acc)
{
    static constexpr uint32_t newbieGroups[] = {
        25, 49, 50, 8, 9, 232, 24, 26, 16, 17, 23, 18, 106, 303, 27, 47,
        48, 51, 304, 302, 32, 52, 201, 46, 41, 45, 44, 42, 43, 301, 29,
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

Player::Player()
{
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
    data->set_name(name.empty() ? "Player" : std::move(name));
    data->set_signature("");
    data->set_gender(gender);
    data->set_headicon(gender ? 101 : 102);
    data->set_skinid(10301);
    data->set_titleprefix(1);
    data->set_titlesuffix(2);
    data->add_boards(410301);
    data->set_level(1);
    data->set_exp(0);
    data->set_energy(240);
    data->set_energylastupdate(now);
    data->set_signinindex(1);
    data->set_lastepochday(0);
    data->set_lastlogin(now);

    Characters().AddCharacterFromId(103);
    Characters().AddCharacterFromId(112);
    Characters().AddCharacterFromId(113);

    Characters().AddDiscFromId(211001);
    Characters().AddDiscFromId(211005);
    Characters().AddDiscFromId(211007);
    Characters().AddDiscFromId(211008);

    mCharacterStor->OnCreate();
    mActivityMgr->OnCreate();
    mInventoryMgr->OnCreate();
    mQuestMgr->OnCreate();

    return true;
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
    GetMutablePlayerData()->set_lastlogin(NowSeconds());
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

void Player::EncodeBasicInfo(proto::PlayerInfo& info) const
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

    auto* worldClass = info.mutable_worldclass();
    worldClass->set_cur(data.level());
    worldClass->set_lastexp(data.exp());

    auto* energy = info.mutable_energy()->mutable_energy();
    energy->set_primary(data.energy());
    energy->set_isprimary(true);
    energy->set_updatetime(data.energylastupdate());
    energy->set_nextduration(1);

    for (int board : data.boards())
    {
        info.add_board(static_cast<uint32_t>(board));
    }
}

void Player::EncodeMinimalSystems(proto::PlayerInfo& info) const
{
    auto* state = info.mutable_state();
    state->mutable_mail();
    state->mutable_battlepass();
    state->mutable_friendenergy();
    state->mutable_mallpackage();
    state->mutable_achievement();
    state->mutable_scoreboss();
    state->mutable_startower();
    state->mutable_startowerbook();
    state->mutable_worldclassreward()->set_flag(std::string(8, '\0'));
    state->mutable_travelerduelquest()->set_type(proto::TravelerDuel);

    info.add_titles()->set_titleid(1);
    info.add_titles()->set_titleid(2);
    info.add_honorlist(111001);
    info.mutable_agent();
    info.mutable_formation();
    info.mutable_phone();
    info.mutable_story();

    auto* handbookChars = info.add_handbook();
    handbookChars->set_type(1);
    handbookChars->set_data(std::string(8, '\0'));

    auto* handbookDiscs = info.add_handbook();
    handbookDiscs->set_type(2);
    handbookDiscs->set_data(std::string(8, '\0'));

    auto* handbookCg = info.add_handbook();
    handbookCg->set_type(3);
    handbookCg->set_data(std::string(8, '\0'));
}
