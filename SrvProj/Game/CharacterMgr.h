#pragma once

#include "ManagerBase.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/player_data.pb.h"
#include "../proto/proto_cpp/public.pb.h"
#include "../Resources/BinClass/CharacterRes.h"
#include "../Resources/BinClass/DiscRes.h"
#include "../proto/proto_cpp/char_affinity_gift_send.pb.h"
#include "../proto/proto_cpp/char_gem_refresh.pb.h"
#include "../proto/proto_cpp/char_upgrade.pb.h"
#include "../proto/proto_cpp/disc_all_limit_break.pb.h"
#include "../proto/proto_cpp/disc_limit_break.pb.h"
#include "../proto/proto_cpp/disc_promote.pb.h"
#include "../proto/proto_cpp/disc_strengthen.pb.h"

namespace proto {
class PhoneContactsInfoResp;
}

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

    bool UpgradeCharacter(uint32_t charId, const ItemParamMap& items, proto::CharUpgradeResp& out);
    bool AdvanceCharacter(uint32_t charId, proto::ChangeInfo& change);
    bool UpgradeCharacterSkill(uint32_t charId, uint32_t index, proto::ChangeInfo& change);
    bool SetCharacterSkin(uint32_t charId, uint32_t skinId);
    bool ToggleCharacterFavorite(uint32_t charId);
    bool SendAffinityGift(uint32_t charId, const ItemParamMap& items, proto::CharAffinityGiftSendResp& out);
    bool ReceivePlotReward(uint32_t plotId, proto::ChangeInfo& change);
    bool UseGemPreset(uint32_t charId, uint32_t presetId);
    bool EquipGem(uint32_t charId, uint32_t slotId, int32_t gemIndex, uint32_t presetId);
    bool RefreshGem(uint32_t charId, uint32_t slotId, uint32_t gemIndex, const google::protobuf::RepeatedField<uint32_t>& lockAttrs, proto::CharGemRefreshResp& out);
    bool ReplaceGemAttribute(uint32_t charId, uint32_t slotId, uint32_t gemIndex);
    bool SetGemLock(uint32_t charId, uint32_t slotId, uint32_t gemIndex, bool locked);
    bool OverlockGem(uint32_t charId, uint32_t slotId, uint32_t gemIndex, uint32_t attrIndex, proto::ChangeInfo& change);

    bool StrengthenDisc(uint32_t discId, const ItemParamMap& items, proto::DiscStrengthenResp& out);
    bool PromoteDisc(uint32_t discId, proto::DiscPromoteResp& out);
    bool LimitBreakDisc(uint32_t discId, uint32_t qty, proto::DiscLimitBreakResp& out);
    bool LimitBreakAllDiscs(proto::DiscAllLimitBreakResp& out);
    bool ReceiveDiscReadReward(uint32_t discId, uint32_t readType, proto::ChangeInfo& change);

    void EncodePlayerInfo(proto::PlayerInfo& out) const override;
    int GetNewPhoneMessageCount() const;
    void BuildPhoneContactsInfo(proto::PhoneContactsInfoResp& out) const;
    bool ReportPhoneContact(uint32_t chatId, uint32_t process, const google::protobuf::RepeatedField<uint32_t>& options, bool end, proto::ChangeInfo& change);
    bool TogglePhoneContactTop(uint32_t charId);

private:
    void NormalizeCharacter(ServerProto::CharacterInfo& character) const;
    void NormalizeDisc(ServerProto::GameDiscInfoBin& disc) const;
    void EnsureCharacterContact(ServerProto::CharacterInfo& character) const;
    void EnsureGemPresets(ServerProto::CharacterInfo& character) const;
    void EnsureGemSlots(ServerProto::CharacterInfo& character) const;
    void EnsureInitialChats(ServerProto::CharacterInfo& character) const;
    void SortCharacters();
    void SortDiscs();
    ServerProto::CharacterGemSlot* GetGemSlot(ServerProto::CharacterInfo& character, uint32_t slotId) const;
    ServerProto::CharacterGem* GetGem(ServerProto::CharacterInfo& character, uint32_t slotId, uint32_t gemIndex) const;

    static proto::Char ToProto(const ServerProto::CharacterInfo& characterInfo);
    static proto::Disc ToProto(const ServerProto::GameDiscInfoBin& discInfo);
    static proto::AffinityInfo ToAffinityProto(const ServerProto::CharacterInfo& characterInfo);
    static proto::CharGem ToProto(const ServerProto::CharacterGem& gem);
};
