#include "CharacterMgr.h"

#include "Player.h"
#include "../Resources/GameData.h"

#include <chrono>
#include <string>

namespace {
int64_t NowSeconds()
{
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}
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
    charInfo->set_affinitylevel(1);
    charInfo->set_affinityexp(0);
    charInfo->set_skin(data.DefaultSkinId);
    charInfo->set_talents(std::string(8, '\0'));
    charInfo->set_createtime(NowSeconds());

    for (int i = 0; i < 5; ++i)
    {
        charInfo->add_skills(1);
    }

    auto* contact = charInfo->mutable_contact();
    contact->set_triggertime(charInfo->createtime());

    for (const auto& [_, chatData] : GameData::ChatDataTable)
    {
        if (chatData.AddressBookId != data.Id || chatData.PreChatId != 0)
        {
            continue;
        }

        ServerProto::CharacterChat chat;
        chat.set_id(chatData.Id);
        (*contact->mutable_chats())[chatData.Id] = chat;
    }

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
