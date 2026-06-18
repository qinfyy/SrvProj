#include "CharacterMgr.h"

#include "Bitset.h"
#include "Player.h"
#include "../Resources/GameData.h"
#include "../proto/proto_cpp/public.pb.h"

#include <chrono>
#include <algorithm>
#include <string>

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
        if (GameData::CharacterDataTable.find(std::to_string(character->charid())) == GameData::CharacterDataTable.end())
        {
            bin->mutable_charinfolist()->DeleteSubrange(index, 1);
            continue;
        }

        NormalizeCharacter(*character);
    }

    for (int index = bin->gamedisclist_size() - 1; index >= 0; --index)
    {
        auto* disc = bin->mutable_gamedisclist(index);
        if (GameData::DiscDataTable.find(std::to_string(disc->discid())) == GameData::DiscDataTable.end())
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
    auto it = GameData::CharacterDataTable.find(std::to_string(charId));
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
    charInfo->set_createtime(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
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
    return charInfo;
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
    auto it = GameData::DiscDataTable.find(std::to_string(discId));
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
    disc->set_createtime(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    NormalizeDisc(*disc);
    SortDiscs();
    GetPlayer()->Trigger(28, 1, static_cast<uint32_t>(data.Id), 0);
    GetPlayer()->Trigger(30, static_cast<uint32_t>(Bin().gamedisclist_size()), static_cast<uint32_t>(disc->level()), 0);
    return disc;
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
    change.add_props()->PackFrom(ToProto(character));
}

void CharacterStor::AddDiscChange(proto::ChangeInfo& change, const ServerProto::GameDiscInfoBin& disc) const
{
    change.add_props()->PackFrom(ToProto(disc));
}

void CharacterStor::TriggerCharacterAchievements(const ServerProto::CharacterInfo& character)
{
    int anyCount = 0;
    int sameElementCount = 0;
    int element = 0;

    if (auto it = GameData::CharacterDataTable.find(std::to_string(character.charid())); it != GameData::CharacterDataTable.end())
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
            if (auto it = GameData::CharacterDataTable.find(std::to_string(owned.charid())); it != GameData::CharacterDataTable.end() && it->second.ElementType == element)
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
    auto it = GameData::CharacterDataTable.find(std::to_string(character.charid()));
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
        character.set_createtime(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
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
        disc.set_createtime(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    }
}

void CharacterStor::EnsureCharacterContact(ServerProto::CharacterInfo& character) const
{
    auto* contact = character.mutable_contact();
    if (contact->triggertime() <= 0)
    {
        contact->set_triggertime(character.createtime() > 0 ? character.createtime() : std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
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
    auto it = GameData::CharacterDataTable.find(std::to_string(character.charid()));
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

    for (int slotId = 1; slotId <= 3; ++slotId)
    {
        auto* slot = cliChar.add_chargemslots();
        slot->set_id(slotId);
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
