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

    void EncodePlayerInfo(proto::PlayerInfo& out) const override;

private:
    static proto::Char ToProto(const ServerProto::CharacterInfo& characterInfo);
    static proto::Disc ToProto(const ServerProto::GameDiscInfoBin& discInfo);
};
