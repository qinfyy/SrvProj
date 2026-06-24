#include "TowerRuntime.h"

#include "CharacterMgr.h"
#include "Player.h"
#include "TowerRooms.h"
#include "TowerMgr.h"
#include "../Game/ChangeInfoUtil.h"
#include "../Resources/BinClass/ItemsRes.h"
#include "../Resources/BinClass/DiscRes.h"
#include "../Resources/BinClass/StarTowerRes.h"
#include "../Resources/GameData.h"
#include "../Util.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace TowerRuntime
{
namespace
{
constexpr uint32_t kTowerCoinItemId = 11;
constexpr size_t kTowerCharSlotCount = 3;
constexpr size_t kTowerDiscSlotCount = 6;
constexpr uint32_t kInvalidTowerHp = (std::numeric_limits<uint32_t>::max)();

double RandomDouble()
{
    static thread_local std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

bool RandomChance(double chance)
{
    return chance > 0.0 && RandomDouble() < chance;
}

int RandomInt(int minValue, int maxValue)
{
    if (maxValue <= minValue)
    {
        return minValue;
    }

    static thread_local std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(minValue, maxValue);
    return dist(rng);
}

template<typename T>
T* FindMutablePairValue(std::vector<std::pair<uint32_t, T>>& items, uint32_t id)
{
    for (auto& [tid, value] : items)
    {
        if (tid == id)
        {
            return &value;
        }
    }

    return nullptr;
}
}

Game::~Game() = default;

void ShopGoods::ApplyDiscount(double percentage)
{
    if (percentage <= 0.0)
    {
        return;
    }

    Discount = static_cast<int32_t>(std::ceil(static_cast<double>(Price) * (1.0 - percentage)));
}

int32_t ShopGoods::GetPrice() const
{
    return Price - Discount;
}

int32_t ShopGoods::GetDisplayPrice() const
{
    return Price;
}

int32_t ShopGoods::GetCount() const
{
    if (Type == 2)
    {
        return Idx == 8 ? 15 : 5;
    }

    return 1;
}

uint32_t ShopGoods::GetCharId(const Game& game) const
{
    if (CharPos == 0)
    {
        return 0;
    }

    const size_t index = static_cast<size_t>(CharPos - 1);
    if (index >= game.CharIds.size())
    {
        return 0;
    }

    return game.CharIds[index];
}

void Build::LoadFromBin(const ServerProto::TowerBuildBin& bin)
{
    Uid = bin.uid();
    Name = bin.name();
    Lock = bin.lock();
    Preference = bin.preference();
    Score = bin.score();

    CharIds.assign(bin.charids().begin(), bin.charids().end());
    DiscIds.assign(bin.discids().begin(), bin.discids().end());
    ActiveSecondaryIds.assign(bin.activesecondaryids().begin(), bin.activesecondaryids().end());

    CharPotentials.clear();
    for (const auto& [charId, count] : bin.charpotentials())
    {
        CharPotentials.emplace_back(charId, count);
    }

    Potentials.clear();
    for (const auto& [potentialId, level] : bin.potentials())
    {
        Potentials.emplace_back(potentialId, level);
    }

    SubNoteSkills.clear();
    for (const auto& [itemId, count] : bin.subnoteskills())
    {
        SubNoteSkills.emplace_back(itemId, count);
    }
}

void Build::SaveToBin(ServerProto::TowerBuildBin& bin) const
{
    bin.Clear();
    bin.set_uid(Uid);
    bin.set_name(Name);
    bin.set_lock(Lock);
    bin.set_preference(Preference);
    bin.set_score(Score);

    for (uint32_t charId : CharIds)
    {
        bin.add_charids(charId);
    }
    for (uint32_t discId : DiscIds)
    {
        bin.add_discids(discId);
    }
    for (uint32_t secondaryId : ActiveSecondaryIds)
    {
        bin.add_activesecondaryids(secondaryId);
    }
    for (const auto& [charId, count] : CharPotentials)
    {
        (*bin.mutable_charpotentials())[charId] = count;
    }
    for (const auto& [potentialId, level] : Potentials)
    {
        (*bin.mutable_potentials())[potentialId] = level;
    }
    for (const auto& [itemId, count] : SubNoteSkills)
    {
        (*bin.mutable_subnoteskills())[itemId] = count;
    }
}

proto::StarTowerBuildBrief Build::ToBriefProto() const
{
    proto::StarTowerBuildBrief out;
    out.set_id(Uid);
    out.set_name(Name);
    out.set_lock(Lock);
    out.set_preference(Preference);
    out.set_score(Score);

    for (uint32_t discId : DiscIds)
    {
        out.add_discids(discId);
    }

    for (uint32_t charId : CharIds)
    {
        auto* ch = out.add_chars();
        ch->set_charid(charId);

        uint32_t count = 0;
        for (const auto& [mappedCharId, potentialCount] : CharPotentials)
        {
            if (mappedCharId == charId)
            {
                count = potentialCount;
                break;
            }
        }
        ch->set_potentialcnt(count);
    }

    return out;
}

proto::StarTowerBuildDetail Build::ToDetailProto() const
{
    proto::StarTowerBuildDetail out;

    for (const auto& [potentialId, level] : Potentials)
    {
        auto* info = out.add_potentials();
        info->set_potentialid(potentialId);
        info->set_level(level);
    }

    for (const auto& [itemId, count] : SubNoteSkills)
    {
        auto* info = out.add_subnoteskills();
        info->set_tid(itemId);
        info->set_qty(count);
    }

    for (uint32_t secondaryId : ActiveSecondaryIds)
    {
        out.add_activesecondaryids(secondaryId);
    }

    return out;
}

proto::StarTowerBuildInfo Build::ToProto() const
{
    proto::StarTowerBuildInfo out;
    TowerRuntime::Build copy = *this;
    copy.Score = 0;
    copy.CharPotentials.clear();

    for (const auto& [potentialId, level] : Potentials)
    {
        const auto potentialIt = GameData::PotentialDataTable.find(std::to_string(potentialId));
        if (potentialIt == GameData::PotentialDataTable.end())
        {
            continue;
        }

        copy.Score += static_cast<uint32_t>((std::max)(potentialIt->second.GetBuildScore(static_cast<int>(level)), 0));
        bool found = false;
        for (auto& [charId, count] : copy.CharPotentials)
        {
            if (charId == static_cast<uint32_t>(potentialIt->second.CharId))
            {
                count += level;
                found = true;
                break;
            }
        }
        if (!found)
        {
            copy.CharPotentials.emplace_back(static_cast<uint32_t>(potentialIt->second.CharId), level);
        }
    }

    for (const auto& [itemId, count] : SubNoteSkills)
    {
        (void)itemId;
        copy.Score += static_cast<uint32_t>((std::max)(count, 0) * 15);
    }

    for (uint32_t secondaryId : ActiveSecondaryIds)
    {
        const auto secondaryIt = GameData::SecondarySkillDataTable.find(std::to_string(secondaryId));
        if (secondaryIt != GameData::SecondarySkillDataTable.end())
        {
            copy.Score += static_cast<uint32_t>((std::max)(secondaryIt->second.Score, 0));
        }
    }

    copy.CharIds = CharIds;
    copy.DiscIds = DiscIds;
    copy.Name = Name;
    copy.Uid = Uid;
    copy.Lock = Lock;
    copy.Preference = Preference;

    out.mutable_brief()->CopyFrom(copy.ToBriefProto());
    out.mutable_detail()->CopyFrom(copy.ToDetailProto());
    return out;
}

void Preset::LoadFromBin(const ServerProto::TowerPotentialPresetBin& bin)
{
    Uid = bin.uid();
    Name = bin.name();
    Preference = bin.preference();
    Timestamp = bin.timestamp();

    CharPotentials.clear();
    for (const auto& ch : bin.chars())
    {
        std::vector<PotentialInfo> list;
        list.reserve(static_cast<size_t>(ch.potentials_size()));
        for (const auto& potential : ch.potentials())
        {
            list.push_back({ potential.id(), potential.level() });
        }
        CharPotentials.emplace_back(ch.charid(), std::move(list));
    }
}

void Preset::SaveToBin(ServerProto::TowerPotentialPresetBin& bin) const
{
    bin.Clear();
    bin.set_uid(Uid);
    bin.set_name(Name);
    bin.set_preference(Preference);
    bin.set_timestamp(Timestamp);

    for (const auto& [charId, potentials] : CharPotentials)
    {
        auto* ch = bin.add_chars();
        ch->set_charid(charId);
        for (const auto& potential : potentials)
        {
            auto* info = ch->add_potentials();
            info->set_id(potential.Id);
            info->set_level(potential.Level);
        }
    }
}

proto::PotentialPreselection Preset::ToProto() const
{
    proto::PotentialPreselection out;
    out.set_id(Uid);
    out.set_name(Name);
    out.set_preference(Preference);
    out.set_timestamp(Timestamp);

    for (const auto& [charId, potentials] : CharPotentials)
    {
        auto* ch = out.add_charpotentials();
        ch->set_charid(charId);
        for (const auto& potential : potentials)
        {
            auto* info = ch->add_potentials();
            info->set_id(potential.Id);
            info->set_level(potential.Level);
        }
    }

    return out;
}

uint64_t GenerateUid()
{
    std::string token;
    if (!GenerateToken(token, true))
    {
        return 0;
    }

    uint64_t value = 0;
    for (size_t i = 0; i < token.size() && i < 16; ++i)
    {
        value <<= 4;
        const char c = token[i];
        if (c >= '0' && c <= '9')
        {
            value |= static_cast<uint64_t>(c - '0');
        }
        else if (c >= 'a' && c <= 'f')
        {
            value |= static_cast<uint64_t>(10 + c - 'a');
        }
    }
    return value;
}

uint32_t BuildScoreFromPotentialLevel(uint32_t level, const std::vector<int>& buildScores)
{
    if (buildScores.empty() || level == 0)
    {
        return 0;
    }

    size_t index = static_cast<size_t>(level - 1);
    if (index >= buildScores.size())
    {
        index = buildScores.size() - 1;
    }
    return static_cast<uint32_t>((std::max)(buildScores[index], 0));
}

uint32_t ClampNameLength(std::string& name)
{
    if (name.size() > 32)
    {
        name.resize(31);
    }
    return static_cast<uint32_t>(name.size());
}

proto::StarTowerInfo Game::ToProto() const
{
    proto::StarTowerInfo out;
    auto* meta = out.mutable_meta();
    meta->set_id(TowerId);
    meta->set_charhp(CharHp < 0 ? kInvalidTowerHp : static_cast<uint32_t>(CharHp));
    meta->set_teamlevel(TeamLevel);
    meta->set_teamexp(TeamExp);
    meta->set_totaltime(BattleTime);
    meta->set_buildid(BuildId);

    for (size_t i = 0; i < kTowerCharSlotCount; ++i)
    {
        auto* ch = meta->add_chars();
        if (i >= CharIds.size() || !Manager)
        {
            continue;
        }

        const auto* character = Manager->GetPlayer()->Characters().GetCharacterById(static_cast<int>(CharIds[i]));
        if (!character)
        {
            continue;
        }

        ch->set_id(static_cast<uint32_t>(character->charid()));
        ch->set_level(character->level());
        ch->set_affinitylevel(character->affinitylevel());
        ch->set_advance(character->advance());
        ch->set_talentnodes(character->talents());
        for (uint32_t skillLv : character->skills())
        {
            ch->add_skilllvs(skillLv);
        }
    }

    for (size_t i = 0; i < kTowerDiscSlotCount; ++i)
    {
        auto* disc = meta->add_discs();
        if (i >= DiscIds.size() || !Manager)
        {
            continue;
        }

        const auto* discInfo = Manager->GetPlayer()->Characters().GetDiscById(static_cast<int>(DiscIds[i]));
        if (!discInfo)
        {
            continue;
        }

        disc->set_id(static_cast<uint32_t>(discInfo->discid()));
        disc->set_level(static_cast<uint32_t>(discInfo->level()));
        disc->set_phase(static_cast<uint32_t>((std::max)(discInfo->phase(), 0)));
        disc->set_star(static_cast<uint32_t>((std::max)(discInfo->star(), 0)));
    }
    for (uint32_t secondaryId : ActiveSecondaryIds)
    {
        meta->add_activesecondaryids(secondaryId);
    }

    if (Room)
    {
        out.mutable_room()->CopyFrom(Room->ToProto());
        if (out.mutable_room()->has_data())
        {
            out.mutable_room()->mutable_data()->set_floor(FloorCount);
        }
    }

    auto* bag = out.mutable_bag();
    for (uint32_t tid : FateCards)
    {
        auto* info = bag->add_fatecard();
        info->set_tid(tid);
        info->set_qty(1);
    }
    for (const auto& [tid, level] : Potentials)
    {
        auto* info = bag->add_potentials();
        info->set_tid(tid);
        info->set_level(level);
    }
    for (const auto& [tid, count] : Items)
    {
        auto* info = bag->add_items();
        info->set_tid(tid);
        info->set_qty(count);
    }
    for (const auto& [tid, count] : Res)
    {
        auto* info = bag->add_res();
        info->set_tid(tid);
        info->set_qty(count);
    }
    return out;
}

void Game::SaveToBin(ServerProto::TowerGameBin& bin) const
{
    bin.Clear();
    bin.set_towerid(TowerId);
    bin.set_formationid(FormationId);
    bin.set_buildid(BuildId);
    bin.set_floorcount(FloorCount);
    bin.set_stagenum(StageNum);
    bin.set_stagefloor(StageFloor);
    bin.set_teamlevel(TeamLevel);
    bin.set_teamexp(TeamExp);
    bin.set_nextlevelexp(NextLevelExp);
    bin.set_charhp(CharHp);
    bin.set_battletime(BattleTime);
    bin.set_pendingpotentialcases(PendingPotentialCases);
    bin.set_pendingrarepotentialcases(PendingRarePotentialCases);
    bin.set_completed(Completed);
    bin.set_sweep(Sweep);
    for (uint32_t id : CharIds)
    {
        bin.add_charids(id);
    }
    for (uint32_t id : DiscIds)
    {
        bin.add_discids(id);
    }
    for (uint32_t id : ActiveSecondaryIds)
    {
        bin.add_activesecondaryids(id);
    }
    for (uint32_t id : FateCards)
    {
        bin.add_fatecards(id);
    }
    for (uint64_t damage : TotalDamages)
    {
        bin.add_totaldamages(damage);
    }
    for (const auto& [tid, count] : Items)
    {
        (*bin.mutable_items())[tid] = count;
    }
    for (const auto& [tid, count] : Res)
    {
        (*bin.mutable_res())[tid] = count;
    }
    for (const auto& [tid, level] : Potentials)
    {
        (*bin.mutable_potentials())[tid] = level;
    }
    for (const auto& [tid, count] : RarePotentialCount)
    {
        (*bin.mutable_rarepotentialcount())[tid] = count;
    }
    if (Room)
    {
        Room->SaveToBin(*bin.mutable_room());
    }
}

void Game::LoadFromBin(const ServerProto::TowerGameBin& bin)
{
    TowerId = bin.towerid();
    FormationId = bin.formationid();
    BuildId = bin.buildid();
    FloorCount = bin.floorcount();
    StageNum = bin.stagenum();
    StageFloor = bin.stagefloor();
    TeamLevel = bin.teamlevel();
    TeamExp = bin.teamexp();
    NextLevelExp = bin.nextlevelexp();
    CharHp = bin.charhp();
    BattleTime = bin.battletime();
    PendingPotentialCases = bin.pendingpotentialcases();
    PendingRarePotentialCases = bin.pendingrarepotentialcases();
    Completed = bin.completed();
    Sweep = bin.sweep();

    CharIds.assign(bin.charids().begin(), bin.charids().end());
    DiscIds.assign(bin.discids().begin(), bin.discids().end());
    ActiveSecondaryIds.assign(bin.activesecondaryids().begin(), bin.activesecondaryids().end());
    FateCards.assign(bin.fatecards().begin(), bin.fatecards().end());
    TotalDamages.assign(bin.totaldamages().begin(), bin.totaldamages().end());

    Items.clear();
    for (const auto& [tid, count] : bin.items())
    {
        Items.emplace_back(tid, count);
    }
    Res.clear();
    for (const auto& [tid, count] : bin.res())
    {
        Res.emplace_back(tid, count);
    }
    Potentials.clear();
    for (const auto& [tid, count] : bin.potentials())
    {
        Potentials.emplace_back(tid, count);
    }
    RarePotentialCount.clear();
    for (const auto& [tid, count] : bin.rarepotentialcount())
    {
        RarePotentialCount.emplace_back(tid, count);
    }
}

int Game::GetItemCount(uint32_t id) const
{
    for (const auto& [tid, count] : Items)
    {
        if (tid == id)
        {
            return count;
        }
    }
    return 0;
}

int Game::GetResCount(uint32_t id) const
{
    for (const auto& [tid, count] : Res)
    {
        if (tid == id)
        {
            return count;
        }
    }
    return 0;
}

int Game::GetPotentialLevel(uint32_t id) const
{
    for (const auto& [tid, level] : Potentials)
    {
        if (tid == id)
        {
            return level;
        }
    }
    return 0;
}

int Game::GetRarePotentialCount(uint32_t charId) const
{
    for (const auto& [tid, count] : RarePotentialCount)
    {
        if (tid == charId)
        {
            return count;
        }
    }
    return 0;
}

void Game::SetHp(int hp)
{
    CharHp = hp;
}

void Game::AddBattleTime(uint32_t amount)
{
    BattleTime += amount;
}

void Game::AddExp(uint32_t amount)
{
    TeamExp += amount;
}

int Game::LevelUp()
{
    int picks = 0;
    while (TeamExp >= NextLevelExp && NextLevelExp > 0 && NextLevelExp != static_cast<uint32_t>(INT32_MAX))
    {
        ++TeamLevel;
        ++picks;
        TeamExp -= NextLevelExp;

        const auto it = GameData::StarTowerTeamExpDataTable.find(std::to_string(TeamLevel + 1));
        if (it != GameData::StarTowerTeamExpDataTable.end())
        {
            NextLevelExp = static_cast<uint32_t>((std::max)(it->second.NeedExp, 0));
        }
        else
        {
            NextLevelExp = static_cast<uint32_t>(INT32_MAX);
        }
    }
    return picks;
}

bool Game::AddRuntimeItem(uint32_t id, int count, proto::ChangeInfo* change)
{
    if (id == 0 || count == 0)
    {
        return false;
    }

    const auto itemIt = GameData::ItemDataTable.find(std::to_string(id));
    if (itemIt == GameData::ItemDataTable.end())
    {
        return false;
    }

    const int itemSubType = itemIt->second.Stype;
    const int itemType = itemIt->second.Type;

    if (itemSubType == 20 || itemSubType == 21)
    {
        const auto potentialIt = GameData::PotentialDataTable.find(std::to_string(id));
        if (potentialIt == GameData::PotentialDataTable.end())
        {
            return false;
        }

        const int current = GetPotentialLevel(id);
        int next = current + count;
        const int maxLevel = potentialIt->second.GetMaxLevel(GetExtraPotentialMaxLevel());
        if (next > maxLevel)
        {
            next = maxLevel;
        }
        if (next < 0)
        {
            next = 0;
        }
        if (next == current)
        {
            return false;
        }

        bool found = false;
        for (auto& [tid, level] : Potentials)
        {
            if (tid == id)
            {
                level = next;
                found = true;
                break;
            }
        }
        if (!found)
        {
            Potentials.emplace_back(id, next);
        }

        if (potentialIt->second.IsSpecial())
        {
            bool rareFound = false;
            for (auto& [charTid, rareCount] : RarePotentialCount)
            {
                if (charTid == static_cast<uint32_t>(potentialIt->second.CharId))
                {
                    rareCount += 1;
                    rareFound = true;
                    break;
                }
            }
            if (!rareFound)
            {
                RarePotentialCount.emplace_back(static_cast<uint32_t>(potentialIt->second.CharId), 1);
            }
        }

        if (change)
        {
            proto::PotentialInfo info;
            info.set_tid(id);
            info.set_level(next - current);
            ChangeInfoUtil::AddProp(*change, info);
        }
        return true;
    }

    if (itemType == ChangeInfoUtil::ItemType::Res)
    {
        bool found = false;
        for (auto& [tid, qty] : Res)
        {
            if (tid == id)
            {
                qty += count;
                found = true;
                break;
            }
        }
        if (!found)
        {
            Res.emplace_back(id, count);
        }
        if (change)
        {
            proto::TowerResInfo info;
            info.set_tid(id);
            info.set_qty(count);
            ChangeInfoUtil::AddProp(*change, info);
        }
        return true;
    }

    bool found = false;
    for (auto& [tid, qty] : Items)
    {
        if (tid == id)
        {
            qty += count;
            found = true;
            break;
        }
    }
    if (!found)
    {
        Items.emplace_back(id, count);
    }

    if (change)
    {
        proto::TowerItemInfo info;
        info.set_tid(id);
        info.set_qty(count);
        ChangeInfoUtil::AddProp(*change, info);
    }
    return true;
}

void Game::AddPotentialSelectors(uint32_t amount)
{
    PendingPotentialCases += amount;
}

void Game::AddRarePotentialSelectors(uint32_t amount)
{
    PendingRarePotentialCases += amount;
}

uint32_t Game::GetDifficulty() const
{
    const auto towerIt = GameData::StarTowerDataTable.find(std::to_string(TowerId));
    if (towerIt == GameData::StarTowerDataTable.end())
    {
        return 0;
    }

    return static_cast<uint32_t>((std::max)(towerIt->second.Difficulty, 0));
}

int Game::GetExtraPotentialMaxLevel() const
{
    if (!Manager)
    {
        return 0;
    }

    if (GetDifficulty() >= 7 && Manager->HasGrowthNode(30301))
    {
        return 3;
    }
    if (GetDifficulty() >= 6 && Manager->HasGrowthNode(20601))
    {
        return 1;
    }
    return 0;
}

int Game::GetRandomSubNoteId() const
{
    const auto towerIt = GameData::StarTowerDataTable.find(std::to_string(TowerId));
    if (towerIt == GameData::StarTowerDataTable.end())
    {
        return 0;
    }

    return SubNoteSkillDropGroupRes::GetRandomDrop(towerIt->second.SubNoteSkillDropGroupId);
}

void Game::RefreshSecondarySkills(proto::TowerChangeData* data)
{
    std::vector<int> calculated = SecondarySkillRes::CalculateSecondarySkills(DiscIds, ItemParamMap{ std::map<int, int>(Items.begin(), Items.end()) });
    std::vector<uint32_t> nextSkills;
    nextSkills.reserve(calculated.size());
    for (int id : calculated)
    {
        if (id > 0)
        {
            nextSkills.push_back(static_cast<uint32_t>(id));
        }
    }

    if (data)
    {
        for (uint32_t id : nextSkills)
        {
            if (std::find(ActiveSecondaryIds.begin(), ActiveSecondaryIds.end(), id) == ActiveSecondaryIds.end())
            {
                auto* info = data->add_secondaries();
                info->set_secondaryid(id);
                info->set_active(true);
            }
        }

        for (uint32_t id : ActiveSecondaryIds)
        {
            if (std::find(nextSkills.begin(), nextSkills.end(), id) == nextSkills.end())
            {
                auto* info = data->add_secondaries();
                info->set_secondaryid(id);
                info->set_active(false);
            }
        }
    }

    ActiveSecondaryIds = std::move(nextSkills);
}

void Game::InitializeSubNotesFromDiscs()
{
    Items.clear();
    for (size_t i = 3; i < DiscIds.size() && i < 6; ++i)
    {
        const auto* disc = Manager ? Manager->GetPlayer()->Characters().GetDiscById(static_cast<int>(DiscIds[i])) : nullptr;
        if (!disc)
        {
            continue;
        }

        const auto discResIt = GameData::DiscDataTable.find(std::to_string(disc->discid()));
        if (discResIt == GameData::DiscDataTable.end())
        {
            continue;
        }

        const int groupId = discResIt->second.SubNoteSkillGroupId * 100 + disc->phase();
        const auto promoteIt = GameData::SubNoteSkillPromoteGroupDataTable.find(std::to_string(groupId));
        if (promoteIt == GameData::SubNoteSkillPromoteGroupDataTable.end())
        {
            continue;
        }

        for (const auto& [itemId, count] : promoteIt->second.Items.Items)
        {
            if (itemId > 0 && count > 0)
            {
                AddRuntimeItem(static_cast<uint32_t>(itemId), count, nullptr);
            }
        }
    }

    RefreshSecondarySkills(nullptr);
}

void Game::AddStartingItems()
{
    if (!Manager)
    {
        return;
    }

    if (Manager->HasGrowthNode(10103))
    {
        AddRuntimeItem(kTowerCoinItemId, 50, nullptr);
    }
    if (Manager->HasGrowthNode(10403))
    {
        AddRuntimeItem(kTowerCoinItemId, 100, nullptr);
    }
    if (Manager->HasGrowthNode(10702))
    {
        AddRuntimeItem(kTowerCoinItemId, 200, nullptr);
    }

    int subNotes = 0;
    if (Manager->HasGrowthNode(10102))
    {
        subNotes += 3;
    }

    for (int i = 0; i < subNotes; ++i)
    {
        const int id = GetRandomSubNoteId();
        if (id > 0)
        {
            AddRuntimeItem(static_cast<uint32_t>(id), 1, nullptr);
        }
    }

    RefreshSecondarySkills(nullptr);
}

bool Game::EnterNextRoom()
{
    const auto towerIt = GameData::StarTowerDataTable.find(std::to_string(TowerId));
    if (towerIt == GameData::StarTowerDataTable.end())
    {
        return false;
    }

    ++FloorCount;

    const uint32_t nextStageFloor = StageFloor + 1;
    if (StageFloor >= static_cast<uint32_t>((std::max)(towerIt->second.GetMaxFloor(static_cast<int>(StageNum)), 0)))
    {
        ++StageNum;
        StageFloor = 1;
    }
    else
    {
        StageFloor = nextStageFloor;
    }

    const uint32_t stageId = (TowerId * 10000u) + (StageNum * 100u) + StageFloor;
    const auto stageIt = GameData::StarTowerStageDataTable.find(std::to_string(stageId));
    if (stageIt == GameData::StarTowerStageDataTable.end())
    {
        return false;
    }

    const TowerRoomType roomType = static_cast<TowerRoomType>((std::max)(stageIt->second.RoomType, 0));
    if (roomType == TowerRoomType::EventRoom)
    {
        Room = std::make_unique<TowerEventRoom>(this, stageId, roomType);
    }
    else if (roomType == TowerRoomType::ShopRoom)
    {
        Room = std::make_unique<TowerHawkerRoom>(this, stageId, roomType);
    }
    else
    {
        Room = std::make_unique<TowerBattleRoom>(this, stageId, roomType);
    }

    Room->OnEnter();
    return true;
}

uint32_t Game::GetTotalPotentialCount() const
{
    uint32_t total = 0;
    for (const auto& [_, level] : Potentials)
    {
        if (level > 0)
        {
            total += static_cast<uint32_t>(level);
        }
    }
    return total;
}

Build Game::BuildSnapshot() const
{
    Build build;
    build.Uid = BuildId;
    build.CharIds = CharIds;
    build.DiscIds = DiscIds;
    build.ActiveSecondaryIds = ActiveSecondaryIds;
    build.Name.clear();
    build.Lock = false;
    build.Preference = false;

    for (const auto& [potentialId, level] : Potentials)
    {
        build.Potentials.emplace_back(potentialId, static_cast<uint32_t>((std::max)(level, 0)));

        const auto potentialIt = GameData::PotentialDataTable.find(std::to_string(potentialId));
        if (potentialIt == GameData::PotentialDataTable.end())
        {
            continue;
        }

        build.Score += static_cast<uint32_t>((std::max)(potentialIt->second.GetBuildScore(level), 0));
        bool found = false;
        for (auto& [charId, count] : build.CharPotentials)
        {
            if (charId == static_cast<uint32_t>(potentialIt->second.CharId))
            {
                count += static_cast<uint32_t>((std::max)(level, 0));
                found = true;
                break;
            }
        }
        if (!found)
        {
            build.CharPotentials.emplace_back(static_cast<uint32_t>(potentialIt->second.CharId), static_cast<uint32_t>((std::max)(level, 0)));
        }
    }

    for (const auto& [itemId, count] : Items)
    {
        build.SubNoteSkills.emplace_back(itemId, count);
        build.Score += static_cast<uint32_t>((std::max)(count, 0) * 15);
    }

    for (uint32_t secondaryId : ActiveSecondaryIds)
    {
        const auto secondaryIt = GameData::SecondarySkillDataTable.find(std::to_string(secondaryId));
        if (secondaryIt != GameData::SecondarySkillDataTable.end())
        {
            build.Score += static_cast<uint32_t>((std::max)(secondaryIt->second.Score, 0));
        }
    }

    return build;
}

void Game::Settle(bool victory, proto::StarTowerInteractResp& rsp)
{
    Completed = true;

    auto* settle = rsp.mutable_settle();
    settle->set_totaltime(BattleTime);
    settle->mutable_build()->CopyFrom(BuildSnapshot().ToProto());
    settle->mutable_change();
    for (uint64_t damage : TotalDamages)
    {
        settle->add_totaldamages(damage);
    }

    if (victory && Manager)
    {
        if (std::find(Manager->Bin().startowerlog().begin(), Manager->Bin().startowerlog().end(), TowerId) == Manager->Bin().startowerlog().end())
        {
            Manager->MutableBin()->add_startowerlog(TowerId);
        }

        int tickets = 50 + RandomInt(static_cast<int>(GetDifficulty()) * 50, static_cast<int>(GetDifficulty()) * 100);
        if (Manager->HasGrowthNode(20403))
        {
            tickets *= 2;
        }
        else if (Manager->HasGrowthNode(20102))
        {
            tickets = static_cast<int>(tickets * 1.6);
        }
        else if (Manager->HasGrowthNode(10501))
        {
            tickets = static_cast<int>(tickets * 1.3);
        }

        const uint32_t ticketQty = static_cast<uint32_t>((std::max)(tickets, 0));
        if (ticketQty > 0)
        {
            auto* reward = settle->add_towerrewards();
            reward->set_tid(12);
            reward->set_qty(ticketQty);

            uint32_t weeklyLimit = 2000;
            if (Manager->HasGrowthNode(10502))
            {
                weeklyLimit = 3000;
            }
            else if (Manager->HasGrowthNode(10201))
            {
                weeklyLimit = 2500;
            }

            const uint32_t current = Manager->GetTowerTickets();
            const uint32_t remain = current >= weeklyLimit ? 0 : (weeklyLimit - current);
            Manager->MutableBin()->set_towertickets(current + (std::min)(ticketQty, remain));
        }
    }
}

bool Game::IsOnFinalFloor(const StarTowerRes& tower) const
{
    return FloorCount + 1 > static_cast<uint32_t>((std::max)(tower.MaxFloors, 0));
}

uint32_t Game::GetNextStageId(const StarTowerRes& tower) const
{
    uint32_t stage = StageNum;
    uint32_t floor = StageFloor + 1;

    if (floor > static_cast<uint32_t>((std::max)(tower.GetMaxFloor(static_cast<int>(stage)), 0)))
    {
        floor = 1;
        ++stage;
    }

    return (TowerId * 10000u) + (stage * 100u) + floor;
}
}
