#include "CharacterMgr.h"

#include "Player.h"
#include "../Resources/GameData.h"

#include <chrono>
#include <algorithm>
#include <string>

namespace {
int64_t NowSeconds()
{
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
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
    charInfo->set_createtime(NowSeconds());
    charInfo->set_gempresetindex(0);

    for (int i = 0; i < 5; ++i)
    {
        charInfo->add_skills(1);
    }

    NormalizeCharacter(*charInfo);
    SortCharacters();
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
    disc->set_createtime(NowSeconds());
    NormalizeDisc(*disc);
    SortDiscs();
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
        character.set_createtime(NowSeconds());
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
        disc.set_createtime(NowSeconds());
    }
}

void CharacterStor::EnsureCharacterContact(ServerProto::CharacterInfo& character) const
{
    auto* contact = character.mutable_contact();
    if (contact->triggertime() <= 0)
    {
        contact->set_triggertime(character.createtime() > 0 ? character.createtime() : NowSeconds());
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
