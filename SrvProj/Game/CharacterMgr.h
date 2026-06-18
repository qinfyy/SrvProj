#pragma once

#include "ManagerBase.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/player_data.pb.h"
#include "../proto/proto_cpp/public.pb.h"
#include "../Resources/BinClass/CharacterRes.h"
#include "../Resources/BinClass/DiscRes.h"

class CharacterStor : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;

    void OnCreate() override;
    void OnLoad() override;

    ServerProto::CharacterCompBin* MutableBin();
    const ServerProto::CharacterCompBin& Bin() const;

    ServerProto::CharacterInfo* AddCharacterFromId(int charId);
    ServerProto::CharacterInfo* AddCharacter(const CharacterRes& data);
    ServerProto::CharacterInfo* GetCharacterById(int id);
    const ServerProto::CharacterInfo* GetCharacterById(int id) const;
    bool HasCharacter(int id) const;

    ServerProto::GameDiscInfoBin* AddDiscFromId(int discId);
    ServerProto::GameDiscInfoBin* AddDisc(const DiscRes& data);
    ServerProto::GameDiscInfoBin* GetDiscById(int id);
    const ServerProto::GameDiscInfoBin* GetDiscById(int id) const;
    bool HasDisc(int id) const;

    bool ApplyCharacterCommandProperties(ServerProto::CharacterInfo& character, int level, int advance, int talent, int skill, int affinity);
    bool ApplyDiscCommandProperties(ServerProto::GameDiscInfoBin& disc, int level, int phase, int star);
    void AddCharacterChange(proto::ChangeInfo& change, const ServerProto::CharacterInfo& character) const;
    void AddDiscChange(proto::ChangeInfo& change, const ServerProto::GameDiscInfoBin& disc) const;
    void TriggerCharacterAchievements(const ServerProto::CharacterInfo& character);

    void EncodePlayerInfo(proto::PlayerInfo& out) const override;
    int GetNewPhoneMessageCount() const;

private:
    void NormalizeCharacter(ServerProto::CharacterInfo& character) const;
    void NormalizeDisc(ServerProto::GameDiscInfoBin& disc) const;
    void EnsureCharacterContact(ServerProto::CharacterInfo& character) const;
    void EnsureGemPresets(ServerProto::CharacterInfo& character) const;
    void EnsureGemSlots(ServerProto::CharacterInfo& character) const;
    void EnsureInitialChats(ServerProto::CharacterInfo& character) const;
    void SortCharacters();
    void SortDiscs();

    static proto::Char ToProto(const ServerProto::CharacterInfo& characterInfo);
    static proto::Disc ToProto(const ServerProto::GameDiscInfoBin& discInfo);
};
