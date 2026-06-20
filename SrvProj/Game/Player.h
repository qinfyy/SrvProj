#pragma once

#include "../proto/proto_cpp/player_data.pb.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>
#include <type_traits>
#include <utility>

class GameSession;

class ActivityMgr;
class AchievementMgr;
class CharacterStor;
class GachaMgr;
class InventoryMgr;
class MailMgr;
class QuestMgr;

class Player
{
public:
    Player(GameSession* sessionRef);
    ~Player();

    bool InitNewPlayer(uint32_t uid, std::string name, bool gender);
    bool LoadFromBlob(uint32_t uid, std::span<const uint8_t> data);
    std::vector<uint8_t> SaveToBlob() const;

    void OnCreate();
    proto::PlayerInfo ToProto();
    void OnLogin();
    int32_t GetEnergy();
    int64_t GetEnergyLastUpdate();
    proto::Energy GetEnergyProto();
    bool AddEnergy(int32_t amount);
    bool ConsumeEnergy(int32_t amount);

    ServerProto::PlayerSaveData& SaveData();
    const ServerProto::PlayerSaveData& SaveData() const;

    CharacterStor& Characters();
    const CharacterStor& Characters() const;

    QuestMgr& Quests();
    const QuestMgr& Quests() const;

    AchievementMgr& Achievements();
    const AchievementMgr& Achievements() const;

    InventoryMgr& Inventory();
    const InventoryMgr& Inventory() const;

    GachaMgr& Gachas();
    const GachaMgr& Gachas() const;

    MailMgr& Mails();
    const MailMgr& Mails() const;

    void Trigger(uint32_t condition, uint32_t progress, uint32_t param1 = 0, uint32_t param2 = 0);

    ServerProto::PlayerBasicCompBin* GetMutablePlayerData();
    const ServerProto::PlayerBasicCompBin& GetPlayerData() const;

    uint32_t GetUid() const;
    void SetUid(uint32_t uid);
    bool SetWorldLevel(uint32_t level);
    void SetSignature(const std::string& signature);
    bool HasAvailableFreeMallPackage() const;
    void QueueMallPackageStateNotify();
    uint32_t GetMonthlyCardRemainingDays(const std::string& cardId) const;
    bool ReceivedMonthlyCardRewardToday(const std::string& cardId) const;
    int64_t GetMonthlyCardEndTime(const std::string& cardId) const;
    void ActivateMonthlyCard(const std::string& cardId, uint32_t durationDays);
    bool CreateMonthlyCardRewardChange(const std::string& cardId, proto::ChangeInfo& out);
    bool GrantMonthlyCardReward(const std::string& cardId, bool notifyOnly);

    GameSession* GetSessionRef() const { return mSessionRef; }
    void SetSessionRef(GameSession* sessionRef) { mSessionRef = sessionRef; }

    template<typename T>
    void PushNextPackage(short msgId, T&& payload) {
        using DecayedT = std::decay_t<T>;
        static_assert(std::is_base_of_v<google::protobuf::Message, DecayedT>, "T must derive from Message");
        auto ptr = std::make_unique<DecayedT>(std::forward<T>(payload));
        PushNextPackage(msgId, std::unique_ptr<google::protobuf::Message>(ptr.release()));
    }

    void PushNextPackage(short msgId, std::unique_ptr<google::protobuf::Message> payload);
    bool Save();

private:
    void InitManagers();
    void EncodeBasicInfo(proto::PlayerInfo& info);
    void EncodeMinimalSystems(proto::PlayerInfo& info) const;
    bool CanClaimMonthlyCardReward(const std::string& cardId) const;
    void RefreshMonthlyCardRewards(bool notifyOnly);

    GameSession* mSessionRef = nullptr;

    uint32_t mUid = 0;
    ServerProto::PlayerSaveData mPlayerSaveData;

    std::unique_ptr<CharacterStor> mCharacterStor;
    std::unique_ptr<ActivityMgr> mActivityMgr;
    std::unique_ptr<AchievementMgr> mAchievementMgr;
    std::unique_ptr<InventoryMgr> mInventoryMgr;
    std::unique_ptr<GachaMgr> mGachaMgr;
    std::unique_ptr<MailMgr> mMailMgr;
    std::unique_ptr<QuestMgr> mQuestMgr;
};
