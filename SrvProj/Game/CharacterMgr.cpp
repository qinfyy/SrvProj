#include "CharacterMgr.h"

#include "Bitset.h"
#include "ChangeInfoUtil.h"
#include "InventoryMgr.h"
#include "Player.h"
#include "../GameTime.h"
#include "../Resources/GameData.h"
#include "../proto/proto_cpp/public.pb.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace {
uint32_t GetMinAdvanceForLevel(uint32_t level)
{
    if (level == 0)
    {
        return 0;
    }

    const uint32_t rawAdvance = (level - 1) / 10;
    return std::min<uint32_t>(rawAdvance, 8);
}

std::string BuildTalentBytes(int talent)
{
    Bitset bitset;
    const int stars = std::clamp(talent, 0, 5);
    for (int i = 0; i < stars; ++i)
    {
        const uint32_t offset = static_cast<uint32_t>(i * 16);
        for (uint32_t node = 1; node <= 10; ++node)
        {
            bitset.SetBit(offset + node);
        }
        bitset.SetBit(offset + 16);
    }

    return bitset.ToByteArray();
}

ItemParamMap FromItemTpls(const google::protobuf::RepeatedPtrField<proto::ItemTpl>& items)
{
    ItemParamMap out;
    for (const auto& item : items)
    {
        out.Add(static_cast<int>(item.tid()), item.qty());
    }
    return out;
}

int GetCharacterMaxLevel(uint32_t advance)
{
    return 10 + static_cast<int>(advance) * 10;
}

int GetDiscMaxLevel(int32_t phase)
{
    return 10 + std::max(phase, 0) * 10;
}

int GetCharacterMaxExp(const ServerProto::CharacterInfo& character)
{
    if (static_cast<int>(character.level()) >= GetCharacterMaxLevel(character.advance()))
    {
        return 0;
    }

    const auto it = GameData::CharacterUpgradeDataTable.find(character.level() + 1);
    return it == GameData::CharacterUpgradeDataTable.end() ? 0 : std::max(it->second.Exp, 0);
}

int GetDiscMaxExp(const DiscRes& data, const ServerProto::GameDiscInfoBin& disc)
{
    if (disc.level() >= GetDiscMaxLevel(disc.phase()))
    {
        return 0;
    }

    const int dataId = data.StrengthenGroupId * 1000 + disc.level() + 1;
    const auto it = GameData::DiscStrengthenDataTable.find(dataId);
    return it == GameData::DiscStrengthenDataTable.end() ? 0 : std::max(it->second.Exp, 0);
}

int GetCharacterMaxGainableExp(const ServerProto::CharacterInfo& character)
{
    if (static_cast<int>(character.level()) >= GetCharacterMaxLevel(character.advance()))
    {
        return 0;
    }

    int max = 0;
    for (uint32_t level = character.level() + 1; level <= static_cast<uint32_t>(GetCharacterMaxLevel(character.advance())); ++level)
    {
        const auto it = GameData::CharacterUpgradeDataTable.find(level);
        if (it != GameData::CharacterUpgradeDataTable.end())
        {
            max += std::max(it->second.Exp, 0);
        }
    }

    return (std::max)(max - static_cast<int>(character.exp()), 0);
}

int GetDiscMaxGainableExp(const DiscRes& data, const ServerProto::GameDiscInfoBin& disc)
{
    if (disc.level() >= GetDiscMaxLevel(disc.phase()))
    {
        return 0;
    }

    int max = 0;
    for (int level = disc.level() + 1; level <= GetDiscMaxLevel(disc.phase()); ++level)
    {
        const int dataId = data.StrengthenGroupId * 1000 + level;
        const auto it = GameData::DiscStrengthenDataTable.find(dataId);
        if (it != GameData::DiscStrengthenDataTable.end())
        {
            max += std::max(it->second.Exp, 0);
        }
    }

    return (std::max)(max - disc.exp(), 0);
}

ItemParamMap BuildDiscPromoteMaterials(const DiscPromoteRes& data)
{
    ItemParamMap materials;
    materials.Add(data.ItemId1, data.Num1);
    materials.Add(data.ItemId2, data.Num2);
    materials.Add(data.ItemId3, data.Num3);
    materials.Add(GOLD_ITEM_ID, data.ExpenseGold);
    return materials;
}
}

void CharacterStor::OnCreate()
{
    SortCharacters();
    SortDiscs();

    auto* bin = MutableBin();
    for (int index = 0; index < bin->charinfolist_size(); ++index)
    {
        NormalizeCharacter(*bin->mutable_charinfolist(index));
    }

    for (int index = 0; index < bin->gamedisclist_size(); ++index)
    {
        NormalizeDisc(*bin->mutable_gamedisclist(index));
    }
}

void CharacterStor::OnLoad()
{
    auto* bin = MutableBin();

    for (int index = bin->charinfolist_size() - 1; index >= 0; --index)
    {
        auto* character = bin->mutable_charinfolist(index);
        if (GameData::CharacterDataTable.find(character->charid()) == GameData::CharacterDataTable.end())
        {
            bin->mutable_charinfolist()->DeleteSubrange(index, 1);
            continue;
        }

        NormalizeCharacter(*character);
    }

    for (int index = bin->gamedisclist_size() - 1; index >= 0; --index)
    {
        auto* disc = bin->mutable_gamedisclist(index);
        if (GameData::DiscDataTable.find(disc->discid()) == GameData::DiscDataTable.end())
        {
            bin->mutable_gamedisclist()->DeleteSubrange(index, 1);
            continue;
        }

        NormalizeDisc(*disc);
    }

    SortCharacters();
    SortDiscs();
}

ServerProto::CharacterCompBin* CharacterStor::MutableBin()
{
    return GetPlayer()->SaveData().mutable_charcomp();
}

const ServerProto::CharacterCompBin& CharacterStor::Bin() const
{
    return GetPlayer()->SaveData().charcomp();
}

ServerProto::CharacterInfo* CharacterStor::AddCharacterFromId(int charId)
{
    auto it = GameData::CharacterDataTable.find(charId);
    if (it == GameData::CharacterDataTable.end())
    {
        return nullptr;
    }

    return AddCharacter(it->second);
}

ServerProto::CharacterInfo* CharacterStor::AddCharacter(const CharacterRes& data)
{
    if (HasCharacter(data.Id) || !data.Available)
    {
        return nullptr;
    }

    auto* charInfo = MutableBin()->add_charinfolist();
    charInfo->set_charid(data.Id);
    charInfo->set_level(1);
    charInfo->set_exp(0);
    charInfo->set_advance(0);
    charInfo->set_affinitylevel(0);
    charInfo->set_affinityexp(0);
    charInfo->set_skin(data.DefaultSkinId);
    charInfo->set_talents(std::string(8, '\0'));
    charInfo->set_createtime(GameTime::NowSeconds());
    charInfo->set_gempresetindex(0);

    for (int i = 0; i < 5; ++i)
    {
        charInfo->add_skills(1);
    }

    NormalizeCharacter(*charInfo);
    SortCharacters();
    GetPlayer()->Trigger(5, 1, static_cast<uint32_t>(data.Id), 0);
    GetPlayer()->Trigger(20, static_cast<uint32_t>(Bin().charinfolist_size()), 0, 0);
    TriggerCharacterAchievements(*charInfo);
    return GetCharacterById(data.Id);
}

ServerProto::CharacterInfo* CharacterStor::GetCharacterById(int id)
{
    if (id <= 0)
    {
        return nullptr;
    }

    auto* bin = MutableBin();
    for (int i = 0; i < bin->charinfolist_size(); ++i)
    {
        auto* character = bin->mutable_charinfolist(i);
        if (character->charid() == id)
        {
            return character;
        }
    }

    return nullptr;
}

const ServerProto::CharacterInfo* CharacterStor::GetCharacterById(int id) const
{
    if (id <= 0)
    {
        return nullptr;
    }

    for (const auto& character : Bin().charinfolist())
    {
        if (character.charid() == id)
        {
            return &character;
        }
    }

    return nullptr;
}

bool CharacterStor::HasCharacter(int id) const
{
    return GetCharacterById(id) != nullptr;
}

ServerProto::GameDiscInfoBin* CharacterStor::AddDiscFromId(int discId)
{
    auto it = GameData::DiscDataTable.find(discId);
    if (it == GameData::DiscDataTable.end())
    {
        return nullptr;
    }

    return AddDisc(it->second);
}

ServerProto::GameDiscInfoBin* CharacterStor::AddDisc(const DiscRes& data)
{
    if (HasDisc(data.Id) || !data.Available)
    {
        return nullptr;
    }

    auto* disc = MutableBin()->add_gamedisclist();
    disc->set_discid(data.Id);
    disc->set_level(1);
    disc->set_exp(0);
    disc->set_phase(0);
    disc->set_star(0);
    disc->set_read(false);
    disc->set_avg(false);
    disc->set_createtime(GameTime::NowSeconds());
    NormalizeDisc(*disc);
    SortDiscs();
    GetPlayer()->Trigger(28, 1, static_cast<uint32_t>(data.Id), 0);
    GetPlayer()->Trigger(30, static_cast<uint32_t>(Bin().gamedisclist_size()), static_cast<uint32_t>(disc->level()), 0);
    return GetDiscById(data.Id);
}

ServerProto::GameDiscInfoBin* CharacterStor::GetDiscById(int id)
{
    if (id <= 0)
    {
        return nullptr;
    }

    auto* bin = MutableBin();
    for (int i = 0; i < bin->gamedisclist_size(); ++i)
    {
        auto* disc = bin->mutable_gamedisclist(i);
        if (disc->discid() == id)
        {
            return disc;
        }
    }

    return nullptr;
}

const ServerProto::GameDiscInfoBin* CharacterStor::GetDiscById(int id) const
{
    if (id <= 0)
    {
        return nullptr;
    }

    for (const auto& disc : Bin().gamedisclist())
    {
        if (disc.discid() == id)
        {
            return &disc;
        }
    }

    return nullptr;
}

bool CharacterStor::HasDisc(int id) const
{
    return GetDiscById(id) != nullptr;
}

bool CharacterStor::ApplyCharacterCommandProperties(ServerProto::CharacterInfo& character, int level, int advance, int talent, int skill, int affinity)
{
    bool changed = false;

    if (level > 0)
    {
        const uint32_t nextLevel = std::min<uint32_t>(static_cast<uint32_t>(level), 90);
        if (character.level() != nextLevel)
        {
            character.set_level(nextLevel);
            character.set_advance(std::max<uint32_t>(character.advance(), GetMinAdvanceForLevel(nextLevel)));
            changed = true;
        }
    }

    if (advance >= 0)
    {
        const uint32_t nextAdvance = std::min<uint32_t>(static_cast<uint32_t>(advance), 8);
        if (character.advance() != nextAdvance)
        {
            character.set_advance(nextAdvance);
            changed = true;
        }
    }

    if (skill > 0)
    {
        const uint32_t nextSkill = std::min<uint32_t>(static_cast<uint32_t>(skill), 10);
        while (character.skills_size() < 5)
        {
            character.add_skills(1);
        }
        for (int index = 0; index < character.skills_size(); ++index)
        {
            if (character.skills(index) != nextSkill)
            {
                character.set_skills(index, nextSkill);
                changed = true;
            }
        }
    }

    if (talent >= 0)
    {
        const auto talents = BuildTalentBytes(talent);
        if (character.talents() != talents)
        {
            character.set_talents(talents);
            changed = true;
        }
    }

    if (affinity >= 0)
    {
        const uint32_t maxAffinity = AffinityLevelRes::MaxLevel > 0
            ? static_cast<uint32_t>(AffinityLevelRes::MaxLevel)
            : 10;
        const uint32_t nextAffinity = std::min<uint32_t>(static_cast<uint32_t>(affinity), maxAffinity);
        if (character.affinitylevel() != nextAffinity)
        {
            character.set_affinitylevel(nextAffinity);
            changed = true;
        }
    }

    if (changed)
    {
        NormalizeCharacter(character);
        TriggerCharacterAchievements(character);
    }

    return changed;
}

bool CharacterStor::ApplyDiscCommandProperties(ServerProto::GameDiscInfoBin& disc, int level, int phase, int star)
{
    bool changed = false;

    if (level > 0)
    {
        const int32_t nextLevel = static_cast<int32_t>(std::min<uint32_t>(static_cast<uint32_t>(level), 90));
        if (disc.level() != nextLevel)
        {
            disc.set_level(nextLevel);
            disc.set_phase(std::max<int32_t>(disc.phase(), static_cast<int32_t>(GetMinAdvanceForLevel(nextLevel))));
            changed = true;
        }
    }

    if (phase >= 0)
    {
        const int32_t nextPhase = static_cast<int32_t>(std::min<uint32_t>(static_cast<uint32_t>(phase), 8));
        if (disc.phase() != nextPhase)
        {
            disc.set_phase(nextPhase);
            changed = true;
        }
    }

    if (star >= 0)
    {
        const int32_t nextStar = static_cast<int32_t>(std::min<uint32_t>(static_cast<uint32_t>(star), 5));
        if (disc.star() != nextStar)
        {
            disc.set_star(nextStar);
            changed = true;
        }
    }

    if (changed)
    {
        NormalizeDisc(disc);
        GetPlayer()->Trigger(35, 1, static_cast<uint32_t>(disc.discid()), 0);
    }

    return changed;
}

void CharacterStor::AddCharacterChange(proto::ChangeInfo& change, const ServerProto::CharacterInfo& character) const
{
    ChangeInfoUtil::AddProp(change, ToProto(character));
}

void CharacterStor::AddDiscChange(proto::ChangeInfo& change, const ServerProto::GameDiscInfoBin& disc) const
{
    ChangeInfoUtil::AddProp(change, ToProto(disc));
}

void CharacterStor::TriggerCharacterAchievements(const ServerProto::CharacterInfo& character)
{
    int anyCount = 0;
    int sameElementCount = 0;
    int element = 0;

    if (auto it = GameData::CharacterDataTable.find(character.charid()); it != GameData::CharacterDataTable.end())
    {
        element = it->second.ElementType;
    }

    for (const auto& owned : Bin().charinfolist())
    {
        if (owned.level() < character.level())
        {
            continue;
        }

        ++anyCount;
        if (element > 0)
        {
            if (auto it = GameData::CharacterDataTable.find(owned.charid()); it != GameData::CharacterDataTable.end() && it->second.ElementType == element)
            {
                ++sameElementCount;
            }
        }
    }

    GetPlayer()->Trigger(16, static_cast<uint32_t>(anyCount), character.level(), 0);
    if (element > 0)
    {
        GetPlayer()->Trigger(17, static_cast<uint32_t>(sameElementCount), character.level(), static_cast<uint32_t>(element));
    }
}

bool CharacterStor::UpgradeCharacter(uint32_t charId, const ItemParamMap& items, proto::CharUpgradeResp& out)
{
    auto* character = GetCharacterById(static_cast<int>(charId));
    if (!character || items.Empty())
    {
        return false;
    }

    int exp = 0;
    for (const auto& [itemId, count] : items.Items)
    {
        const auto itemIt = GameData::CharItemExpDataTable.find(itemId);
        if (itemIt == GameData::CharItemExpDataTable.end() || count <= 0)
        {
            return false;
        }

        exp += itemIt->second.ExpValue * count;
    }

    exp = (std::min)(exp, GetCharacterMaxGainableExp(*character));
    if (exp <= 0)
    {
        return false;
    }

    ItemParamMap cost = items;
    cost.Add(GOLD_ITEM_ID, static_cast<int>(std::ceil(exp * 0.15)));
    if (!GetPlayer()->Inventory().HasItems(cost))
    {
        return false;
    }

    proto::ChangeInfo change;
    if (!GetPlayer()->Inventory().RemoveItems(cost, &change))
    {
        return false;
    }

    const uint32_t oldLevel = character->level();
    int maxExp = GetCharacterMaxExp(*character);
    character->set_exp(character->exp() + exp);
    while (character->exp() >= static_cast<uint32_t>(maxExp) && maxExp > 0)
    {
        character->set_level(character->level() + 1);
        character->set_exp(character->exp() - maxExp);
        maxExp = GetCharacterMaxExp(*character);
    }
    if (static_cast<int>(character->level()) >= GetCharacterMaxLevel(character->advance()))
    {
        character->set_exp(0);
    }

    if (character->level() > oldLevel)
    {
        GetPlayer()->Trigger(12, character->level() - oldLevel, character->charid(), 0);
        TriggerCharacterAchievements(*character);
    }

    out.set_level(character->level());
    out.set_exp(character->exp());
    out.mutable_change()->CopyFrom(change);
    return true;
}

bool CharacterStor::AdvanceCharacter(uint32_t charId, proto::ChangeInfo& change)
{
    auto* character = GetCharacterById(static_cast<int>(charId));
    if (!character)
    {
        return false;
    }

    const auto charIt = GameData::CharacterDataTable.find(charId);
    if (charIt == GameData::CharacterDataTable.end())
    {
        return false;
    }

    const int advanceId = charIt->second.AdvanceGroup * 100 + static_cast<int>(character->advance()) + 1;
    const auto advanceIt = GameData::CharacterAdvanceDataTable.find(advanceId);
    if (advanceIt == GameData::CharacterAdvanceDataTable.end())
    {
        return false;
    }

    if (!GetPlayer()->Inventory().RemoveItems(advanceIt->second.Materials, &change))
    {
        return false;
    }

    character->set_advance(character->advance() + 1);
    if (character->advance() == static_cast<uint32_t>(charIt->second.AdvanceSkinUnlockLevel) && charIt->second.AdvanceSkinId > 0)
    {
        character->set_skin(static_cast<uint32_t>(charIt->second.AdvanceSkinId));
        GetPlayer()->Inventory().AddSkin(static_cast<uint32_t>(charIt->second.AdvanceSkinId), &change);
    }

    NormalizeCharacter(*character);
    GetPlayer()->Trigger(7, 1, character->charid(), 0);
    return true;
}

bool CharacterStor::UpgradeCharacterSkill(uint32_t charId, uint32_t index, proto::ChangeInfo& change)
{
    auto* character = GetCharacterById(static_cast<int>(charId));
    if (!character || index == 0)
    {
        return false;
    }

    const auto charIt = GameData::CharacterDataTable.find(charId);
    const int skillIndex = static_cast<int>(index) - 1;
    if (charIt == GameData::CharacterDataTable.end() || skillIndex < 0 || skillIndex >= static_cast<int>(charIt->second.SkillsUpgradeGroup.size()))
    {
        return false;
    }

    while (character->skills_size() <= skillIndex)
    {
        character->add_skills(1);
    }

    const int upgradeId = charIt->second.SkillsUpgradeGroup[skillIndex] * 100 + static_cast<int>(character->skills(skillIndex));
    const auto upgradeIt = GameData::CharacterSkillUpgradeDataTable.find(upgradeId);
    if (upgradeIt == GameData::CharacterSkillUpgradeDataTable.end())
    {
        return false;
    }

    if (!GetPlayer()->Inventory().RemoveItems(upgradeIt->second.Materials, &change))
    {
        return false;
    }

    character->set_skills(skillIndex, character->skills(skillIndex) + 1);
    return true;
}

bool CharacterStor::SetCharacterSkin(uint32_t charId, uint32_t skinId)
{
    auto* character = GetCharacterById(static_cast<int>(charId));
    if (!character || character->skin() == skinId)
    {
        return false;
    }

    if (!GetPlayer()->Inventory().HasItem(skinId, 1))
    {
        return false;
    }

    character->set_skin(skinId);
    return true;
}

bool CharacterStor::ToggleCharacterFavorite(uint32_t charId)
{
    auto* character = GetCharacterById(static_cast<int>(charId));
    if (!character)
    {
        return false;
    }

    character->set_favorite(!character->favorite());
    return true;
}

bool CharacterStor::SendAffinityGift(uint32_t charId, const ItemParamMap& items, proto::CharAffinityGiftSendResp& out)
{
    auto* character = GetCharacterById(static_cast<int>(charId));
    if (!character || items.Empty() || !GetPlayer()->Inventory().HasItems(items))
    {
        return false;
    }

    int exp = 0;
    int count = 0;
    for (const auto& [itemId, qty] : items.Items)
    {
        const auto giftIt = GameData::AffinityGiftDataTable.find(itemId);
        if (giftIt == GameData::AffinityGiftDataTable.end() || qty <= 0)
        {
            return false;
        }

        exp += giftIt->second.BaseAffinity * qty;
        count += qty;
    }

    if (exp <= 0)
    {
        return false;
    }

    proto::ChangeInfo change;
    if (!GetPlayer()->Inventory().RemoveItems(items, &change))
    {
        return false;
    }

    int maxExp = 0;
    auto nextIt = GameData::AffinityLevelDataTable.find(character->affinitylevel() + 1);
    maxExp = nextIt == GameData::AffinityLevelDataTable.end() ? 0 : std::max(nextIt->second.NeedExp, 0);
    character->set_affinityexp(character->affinityexp() + exp);
    while (character->affinityexp() >= static_cast<uint32_t>(maxExp) && maxExp > 0)
    {
        character->set_affinitylevel(character->affinitylevel() + 1);
        character->set_affinityexp(character->affinityexp() - maxExp);
        nextIt = GameData::AffinityLevelDataTable.find(character->affinitylevel() + 1);
        maxExp = nextIt == GameData::AffinityLevelDataTable.end() ? 0 : std::max(nextIt->second.NeedExp, 0);
    }
    if (maxExp <= 0)
    {
        character->set_affinityexp(0);
    }

    GetPlayer()->Trigger(45, static_cast<uint32_t>(count), character->charid(), 0);
    out.mutable_change()->CopyFrom(change);
    out.mutable_info()->CopyFrom(ToAffinityProto(*character));
    out.set_sendgiftcnt(static_cast<uint32_t>(count));
    return true;
}

bool CharacterStor::UseGemPreset(uint32_t charId, uint32_t presetId)
{
    auto* character = GetCharacterById(static_cast<int>(charId));
    if (!character || presetId >= 3)
    {
        return false;
    }

    character->set_gempresetindex(presetId);
    EnsureGemPresets(*character);
    return true;
}

bool CharacterStor::EquipGem(uint32_t charId, uint32_t slotId, int32_t gemIndex, uint32_t presetId)
{
    auto* character = GetCharacterById(static_cast<int>(charId));
    if (!character || presetId >= 3 || slotId == 0 || slotId > 3)
    {
        return false;
    }

    EnsureGemPresets(*character);
    auto* preset = character->mutable_gempresets(static_cast<int>(presetId));
    while (preset->gems_size() < 3)
    {
        preset->add_gems(static_cast<uint32_t>(-1));
    }
    preset->set_gems(static_cast<int>(slotId - 1), static_cast<uint32_t>(gemIndex));
    return true;
}

bool CharacterStor::RefreshGem(uint32_t charId, uint32_t slotId, uint32_t gemIndex, const google::protobuf::RepeatedField<uint32_t>& lockAttrs, proto::CharGemRefreshResp& out)
{
    auto* character = GetCharacterById(static_cast<int>(charId));
    auto* gem = character ? GetGem(*character, slotId, gemIndex) : nullptr;
    if (!character || !gem)
    {
        return false;
    }

    gem->clear_alterattributes();
    gem->clear_alteroverlock();
    if (gem->attributes_size() == 0)
    {
        gem->add_attributes(slotId * 1000 + 1);
    }

    for (uint32_t attr : gem->attributes())
    {
        bool locked = false;
        for (uint32_t lockAttr : lockAttrs)
        {
            if (lockAttr == attr)
            {
                locked = true;
                break;
            }
        }
        gem->add_alterattributes(locked ? attr : attr + 1);
    }

    while (gem->alteroverlock_size() < gem->alterattributes_size())
    {
        gem->add_alteroverlock(0);
    }

    for (uint32_t attr : gem->alterattributes())
    {
        out.add_attributes(attr);
    }
    for (uint32_t count : gem->alteroverlock())
    {
        out.add_overlockcount(count);
    }
    return true;
}

bool CharacterStor::ReplaceGemAttribute(uint32_t charId, uint32_t slotId, uint32_t gemIndex)
{
    auto* character = GetCharacterById(static_cast<int>(charId));
    auto* gem = character ? GetGem(*character, slotId, gemIndex) : nullptr;
    if (!character || !gem || gem->alterattributes_size() == 0)
    {
        return false;
    }

    gem->mutable_attributes()->CopyFrom(gem->alterattributes());
    gem->mutable_overlock()->CopyFrom(gem->alteroverlock());
    gem->clear_alterattributes();
    gem->clear_alteroverlock();
    return true;
}

bool CharacterStor::SetGemLock(uint32_t charId, uint32_t slotId, uint32_t gemIndex, bool locked)
{
    auto* character = GetCharacterById(static_cast<int>(charId));
    auto* gem = character ? GetGem(*character, slotId, gemIndex) : nullptr;
    if (!character || !gem)
    {
        return false;
    }

    gem->set_locked(locked);
    return true;
}

bool CharacterStor::OverlockGem(uint32_t charId, uint32_t slotId, uint32_t gemIndex, uint32_t attrIndex, proto::ChangeInfo& change)
{
    auto* character = GetCharacterById(static_cast<int>(charId));
    auto* gem = character ? GetGem(*character, slotId, gemIndex) : nullptr;
    if (!character || !gem || attrIndex >= static_cast<uint32_t>(gem->attributes_size()))
    {
        return false;
    }

    while (gem->overlock_size() < gem->attributes_size())
    {
        gem->add_overlock(0);
    }
    gem->set_overlock(static_cast<int>(attrIndex), gem->overlock(static_cast<int>(attrIndex)) + 1);
    return true;
}

bool CharacterStor::StrengthenDisc(uint32_t discId, const ItemParamMap& items, proto::DiscStrengthenResp& out)
{
    auto* disc = GetDiscById(static_cast<int>(discId));
    const auto dataIt = GameData::DiscDataTable.find(discId);
    if (!disc || dataIt == GameData::DiscDataTable.end() || items.Empty())
    {
        return false;
    }

    int exp = 0;
    for (const auto& [itemId, count] : items.Items)
    {
        const auto itemIt = GameData::DiscItemExpDataTable.find(itemId);
        if (itemIt == GameData::DiscItemExpDataTable.end() || count <= 0)
        {
            return false;
        }

        exp += itemIt->second.Exp * count;
    }

    exp = (std::min)(exp, GetDiscMaxGainableExp(dataIt->second, *disc));
    if (exp <= 0)
    {
        return false;
    }

    ItemParamMap cost = items;
    cost.Add(GOLD_ITEM_ID, static_cast<int>(std::ceil(exp * 0.25)));
    if (!GetPlayer()->Inventory().HasItems(cost))
    {
        return false;
    }

    proto::ChangeInfo change;
    if (!GetPlayer()->Inventory().RemoveItems(cost, &change))
    {
        return false;
    }

    const int32_t oldLevel = disc->level();
    int maxExp = GetDiscMaxExp(dataIt->second, *disc);
    disc->set_exp(disc->exp() + exp);
    while (disc->exp() >= maxExp && maxExp > 0)
    {
        disc->set_level(disc->level() + 1);
        disc->set_exp(disc->exp() - maxExp);
        maxExp = GetDiscMaxExp(dataIt->second, *disc);
    }
    if (disc->level() >= GetDiscMaxLevel(disc->phase()))
    {
        disc->set_exp(0);
    }

    if (disc->level() > oldLevel)
    {
        GetPlayer()->Trigger(35, static_cast<uint32_t>(disc->level() - oldLevel), disc->discid(), 0);
    }

    out.set_level(static_cast<uint32_t>(disc->level()));
    out.set_exp(static_cast<uint32_t>(disc->exp()));
    out.mutable_change()->CopyFrom(change);
    return true;
}

bool CharacterStor::PromoteDisc(uint32_t discId, proto::DiscPromoteResp& out)
{
    auto* disc = GetDiscById(static_cast<int>(discId));
    const auto dataIt = GameData::DiscDataTable.find(discId);
    if (!disc || dataIt == GameData::DiscDataTable.end())
    {
        return false;
    }

    const int phaseId = dataIt->second.PromoteGroupId * 1000 + disc->phase() + 1;
    const auto promoteIt = GameData::DiscPromoteDataTable.find(phaseId);
    if (promoteIt == GameData::DiscPromoteDataTable.end())
    {
        return false;
    }

    proto::ChangeInfo change;
    if (!GetPlayer()->Inventory().RemoveItems(BuildDiscPromoteMaterials(promoteIt->second), &change))
    {
        return false;
    }

    disc->set_phase(disc->phase() + 1);
    GetPlayer()->Trigger(34, 1, disc->discid(), 0);
    out.set_phase(static_cast<uint32_t>(disc->phase()));
    out.mutable_change()->CopyFrom(change);
    return true;
}

bool CharacterStor::LimitBreakDisc(uint32_t discId, uint32_t qty, proto::DiscLimitBreakResp& out)
{
    auto* disc = GetDiscById(static_cast<int>(discId));
    const auto dataIt = GameData::DiscDataTable.find(discId);
    if (!disc || dataIt == GameData::DiscDataTable.end() || qty == 0 || disc->star() >= 5)
    {
        return false;
    }

    const uint32_t actualQty = (std::min)(qty, static_cast<uint32_t>(5 - disc->star()));
    ItemParamMap materials;
    materials.Add(dataIt->second.TransformItemId, static_cast<int>(actualQty));

    proto::ChangeInfo change;
    if (!GetPlayer()->Inventory().RemoveItems(materials, &change))
    {
        return false;
    }

    disc->set_star(disc->star() + static_cast<int32_t>(actualQty));
    out.set_star(static_cast<uint32_t>(disc->star()));
    out.mutable_change()->CopyFrom(change);
    return true;
}

bool CharacterStor::LimitBreakAllDiscs(proto::DiscAllLimitBreakResp& out)
{
    auto* bin = MutableBin();
    for (int index = 0; index < bin->gamedisclist_size(); ++index)
    {
        auto* disc = bin->mutable_gamedisclist(index);
        if (disc->star() >= 5)
        {
            continue;
        }

        const auto dataIt = GameData::DiscDataTable.find(disc->discid());
        if (dataIt == GameData::DiscDataTable.end())
        {
            continue;
        }

        const int64_t owned = GetPlayer()->Inventory().GetItemCount(static_cast<uint32_t>(dataIt->second.TransformItemId));
        const uint32_t qty = static_cast<uint32_t>((std::min)(owned, static_cast<int64_t>(5 - disc->star())));
        if (qty == 0)
        {
            continue;
        }

        ItemParamMap materials;
        materials.Add(dataIt->second.TransformItemId, static_cast<int>(qty));
        if (!GetPlayer()->Inventory().RemoveItems(materials, out.mutable_change()))
        {
            continue;
        }

        disc->set_star(disc->star() + static_cast<int32_t>(qty));
        auto* item = out.add_limitbreaks();
        item->set_id(static_cast<uint32_t>(disc->discid()));
        item->set_star(static_cast<uint32_t>(disc->star()));
    }

    return true;
}

bool CharacterStor::ReceiveDiscReadReward(uint32_t discId, uint32_t readType, proto::ChangeInfo& change)
{
    auto* disc = GetDiscById(static_cast<int>(discId));
    const auto dataIt = GameData::DiscDataTable.find(discId);
    if (!disc || dataIt == GameData::DiscDataTable.end())
    {
        return false;
    }

    if (readType == 1)
    {
        if (disc->read())
        {
            return false;
        }
        disc->set_read(true);
    }
    else if (readType == 2)
    {
        if (disc->avg())
        {
            return false;
        }
        disc->set_avg(true);
    }
    else
    {
        return false;
    }

    if (dataIt->second.ReadReward.size() >= 2)
    {
        GetPlayer()->Inventory().AddItem(static_cast<uint32_t>(dataIt->second.ReadReward[0]), dataIt->second.ReadReward[1], &change);
    }
    return true;
}

void CharacterStor::EncodePlayerInfo(proto::PlayerInfo& out) const
{
    for (const auto& character : Bin().charinfolist())
    {
        out.add_chars()->CopyFrom(ToProto(character));
    }

    for (const auto& disc : Bin().gamedisclist())
    {
        out.add_discs()->CopyFrom(ToProto(disc));
    }
}

int CharacterStor::GetNewPhoneMessageCount() const
{
    int count = 0;
    for (const auto& character : Bin().charinfolist())
    {
        for (const auto& [_, chat] : character.contact().chats())
        {
            if (!chat.end())
            {
                ++count;
                break;
            }
        }
    }
    return count;
}

void CharacterStor::NormalizeCharacter(ServerProto::CharacterInfo& character) const
{
    auto it = GameData::CharacterDataTable.find(character.charid());
    if (it == GameData::CharacterDataTable.end())
    {
        return;
    }

    const CharacterRes& data = it->second;

    if (character.level() <= 0)
    {
        character.set_level(1);
    }

    if (character.skin() <= 0)
    {
        character.set_skin(data.DefaultSkinId);
    }

    if (character.createtime() <= 0)
    {
        character.set_createtime(GameTime::NowSeconds());
    }

    if (character.affinitylevel() < 0)
    {
        character.set_affinitylevel(0);
    }

    while (character.skills_size() < 5)
    {
        character.add_skills(1);
    }

    if (character.skills_size() > 5)
    {
        while (character.skills_size() > 5)
        {
            character.mutable_skills()->RemoveLast();
        }
    }

    if (character.talents().empty())
    {
        character.set_talents(std::string(8, '\0'));
    }

    EnsureGemPresets(character);
    EnsureGemSlots(character);
    EnsureCharacterContact(character);
}

void CharacterStor::NormalizeDisc(ServerProto::GameDiscInfoBin& disc) const
{
    if (disc.level() <= 0)
    {
        disc.set_level(1);
    }

    if (disc.phase() < 0)
    {
        disc.set_phase(0);
    }

    if (disc.star() < 0)
    {
        disc.set_star(0);
    }

    if (disc.createtime() <= 0)
    {
        disc.set_createtime(GameTime::NowSeconds());
    }
}

void CharacterStor::EnsureCharacterContact(ServerProto::CharacterInfo& character) const
{
    auto* contact = character.mutable_contact();
    if (contact->triggertime() <= 0)
    {
        contact->set_triggertime(character.createtime() > 0 ? character.createtime() : GameTime::NowSeconds());
    }

    EnsureInitialChats(character);
}

void CharacterStor::EnsureGemPresets(ServerProto::CharacterInfo& character) const
{
    while (character.gempresets_size() < 3)
    {
        character.add_gempresets();
    }

    while (character.gempresets_size() > 3)
    {
        character.mutable_gempresets()->RemoveLast();
    }

    for (int index = 0; index < character.gempresets_size(); ++index)
    {
        auto* preset = character.mutable_gempresets(index);
        while (preset->gems_size() < 3)
        {
            preset->add_gems(static_cast<uint32_t>(-1));
        }

        while (preset->gems_size() > 3)
        {
            preset->mutable_gems()->RemoveLast();
        }
    }
}

void CharacterStor::EnsureGemSlots(ServerProto::CharacterInfo& character) const
{
    while (character.gemslots_size() < 3)
    {
        auto* slot = character.add_gemslots();
        slot->set_id(character.gemslots_size());
    }

    for (int index = 0; index < character.gemslots_size() && index < 3; ++index)
    {
        auto* slot = character.mutable_gemslots(index);
        if (slot->id() <= 0)
        {
            slot->set_id(index + 1);
        }
    }
}

void CharacterStor::EnsureInitialChats(ServerProto::CharacterInfo& character) const
{
    auto it = GameData::CharacterDataTable.find(character.charid());
    if (it == GameData::CharacterDataTable.end())
    {
        return;
    }

    auto* chats = character.mutable_contact()->mutable_chats();
    if (!chats->empty())
    {
        return;
    }

    for (const auto& [_, chatData] : GameData::ChatDataTable)
    {
        if (chatData.AddressBookId != it->second.Id || chatData.PreChatId != 0)
        {
            continue;
        }

        ServerProto::CharacterChat chat;
        chat.set_id(chatData.Id);
        chat.set_process(0);
        chat.set_end(false);
        (*chats)[chatData.Id] = chat;
    }
}

void CharacterStor::SortCharacters()
{
    auto* list = MutableBin()->mutable_charinfolist();
    std::sort(list->begin(), list->end(), [](const ServerProto::CharacterInfo& left, const ServerProto::CharacterInfo& right) {
        return left.charid() < right.charid();
    });
}

void CharacterStor::SortDiscs()
{
    auto* list = MutableBin()->mutable_gamedisclist();
    std::sort(list->begin(), list->end(), [](const ServerProto::GameDiscInfoBin& left, const ServerProto::GameDiscInfoBin& right) {
        return left.discid() < right.discid();
    });
}

proto::Char CharacterStor::ToProto(const ServerProto::CharacterInfo& characterInfo)
{
    proto::Char cliChar;
    cliChar.set_tid(characterInfo.charid());
    cliChar.set_level(characterInfo.level());
    cliChar.set_exp(characterInfo.exp());
    cliChar.set_skin(characterInfo.skin());
    cliChar.set_advance(characterInfo.advance());
    cliChar.set_isfavorite(characterInfo.favorite());
    cliChar.set_affinitylevel(characterInfo.affinitylevel());
    cliChar.set_affinityexp(characterInfo.affinityexp());
    cliChar.set_talentnodes(characterInfo.talents());
    cliChar.set_createtime(characterInfo.createtime());
    cliChar.mutable_affinityquests();

    for (uint32_t skill : characterInfo.skills())
    {
        cliChar.add_skilllvs(skill);
    }

    auto* presets = cliChar.mutable_chargempresets();
    presets->set_inusepresetindex(characterInfo.gempresetindex());

    for (int i = 0; i < 3; ++i)
    {
        auto* preset = presets->add_chargempresets();
        if (i < characterInfo.gempresets_size())
        {
            const auto& savedPreset = characterInfo.gempresets(i);
            preset->set_name(savedPreset.name());
            for (uint32_t gemId : savedPreset.gems())
            {
                preset->add_slotgem(static_cast<int32_t>(gemId));
            }
        }

        while (preset->slotgem_size() < 3)
        {
            preset->add_slotgem(-1);
        }
    }

    for (int index = 0; index < characterInfo.gemslots_size() && index < 3; ++index)
    {
        const auto& savedSlot = characterInfo.gemslots(index);
        auto* slot = cliChar.add_chargemslots();
        slot->set_id(savedSlot.id());
        for (const auto& savedGem : savedSlot.gems())
        {
            slot->add_altergems()->CopyFrom(ToProto(savedGem));
        }
    }

    for (int slotId = 1; slotId <= 3; ++slotId)
    {
        bool exists = false;
        for (const auto& slot : cliChar.chargemslots())
        {
            if (slot.id() == static_cast<uint32_t>(slotId))
            {
                exists = true;
                break;
            }
        }
        if (!exists)
        {
            cliChar.add_chargemslots()->set_id(slotId);
        }
    }

    return cliChar;
}

proto::Disc CharacterStor::ToProto(const ServerProto::GameDiscInfoBin& discInfo)
{
    proto::Disc disc;
    disc.set_id(discInfo.discid());
    disc.set_level(discInfo.level());
    disc.set_exp(discInfo.exp());
    disc.set_phase(discInfo.phase());
    disc.set_star(discInfo.star());
    disc.set_read(discInfo.read());
    disc.set_avg(discInfo.avg());
    disc.set_createtime(discInfo.createtime());
    return disc;
}

ServerProto::CharacterGemSlot* CharacterStor::GetGemSlot(ServerProto::CharacterInfo& character, uint32_t slotId) const
{
    if (slotId == 0 || slotId > 3)
    {
        return nullptr;
    }

    EnsureGemSlots(character);
    for (int index = 0; index < character.gemslots_size(); ++index)
    {
        auto* slot = character.mutable_gemslots(index);
        if (slot->id() == slotId)
        {
            return slot;
        }
    }
    return nullptr;
}

ServerProto::CharacterGem* CharacterStor::GetGem(ServerProto::CharacterInfo& character, uint32_t slotId, uint32_t gemIndex) const
{
    auto* slot = GetGemSlot(character, slotId);
    if (!slot || gemIndex >= static_cast<uint32_t>(slot->gems_size()))
    {
        return nullptr;
    }

    return slot->mutable_gems(static_cast<int>(gemIndex));
}

proto::AffinityInfo CharacterStor::ToAffinityProto(const ServerProto::CharacterInfo& characterInfo)
{
    proto::AffinityInfo info;
    info.set_charid(characterInfo.charid());
    info.set_affinitylevel(characterInfo.affinitylevel());
    info.set_affinityexp(characterInfo.affinityexp());
    return info;
}

proto::CharGem CharacterStor::ToProto(const ServerProto::CharacterGem& gem)
{
    proto::CharGem out;
    out.set_lock(gem.locked());
    for (uint32_t attr : gem.attributes())
    {
        out.add_attributes(attr);
    }
    for (uint32_t attr : gem.alterattributes())
    {
        out.add_alterattributes(attr);
    }
    for (uint32_t count : gem.overlock())
    {
        out.add_overlockcount(count);
    }
    for (uint32_t count : gem.alteroverlock())
    {
        out.add_alteroverlockcount(count);
    }
    return out;
}
