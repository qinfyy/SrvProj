#pragma once

#include "../proto/proto_cpp/player_data.pb.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>
#include "QuestMgr.h"

class GameSession;

class ActivityMgr;
class CharacterStor;
class InventoryMgr;
class QuestMgr;

class Player
{
public:
    Player(GameSession* sessionRef);
    ~Player();

    bool InitNewPlayer(uint32_t uid, std::string name, bool gender);
    bool LoadFromBlob(uint32_t uid, std::span<const uint8_t> data);
    std::vector<uint8_t> SaveToBlob() const;

    proto::PlayerInfo ToProto();
    void OnLogin();

    ServerProto::PlayerSaveData& SaveData();
    const ServerProto::PlayerSaveData& SaveData() const;

    CharacterStor& Characters();
    const CharacterStor& Characters() const;

    ServerProto::PlayerBasicCompBin* GetMutablePlayerData();
    const ServerProto::PlayerBasicCompBin& GetPlayerData() const;

    uint32_t GetUid() const;

    GameSession* GetSessionRef() const { return mSessionRef; }
    void SetSessionRef(GameSession* sessionRef) { mSessionRef = sessionRef; }

    void PushNextPackage(short msgId, std::unique_ptr<google::protobuf::Message> payload);

private:
    void InitManagers();
    void EncodeBasicInfo(proto::PlayerInfo& info) const;
    void EncodeMinimalSystems(proto::PlayerInfo& info) const;

	GameSession* mSessionRef = nullptr;

    uint32_t mUid = 0;
    ServerProto::PlayerSaveData mPlayerSaveData;

    std::unique_ptr<CharacterStor> mCharacterStor;
    std::unique_ptr<ActivityMgr> mActivityMgr;
    std::unique_ptr<InventoryMgr> mInventoryMgr;
    std::unique_ptr<QuestMgr> mQuestMgr;
};
