#pragma once

#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/public_star_tower.pb.h"
#include "../proto/proto_cpp/star_tower_interact.pb.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class Player;
class TowerRoom;
class TowerCaseBase;
class StarTowerRes;
class TowerMgr;

namespace TowerRuntime
{
class Game;
class Build;
class Preset;

struct PotentialInfo
{
    uint32_t Id = 0;
    uint32_t Level = 0;
};

struct ShopGoods
{
    uint32_t Sid = 0;
    uint32_t Type = 0;
    uint32_t Idx = 0;
    uint32_t GoodsId = 0;
    int32_t Price = 0;
    int32_t Discount = 0;
    uint32_t CharPos = 0;
    bool Sold = false;

    bool HasDiscount() const { return Discount > 0; }
    void ApplyDiscount(double percentage);
    int32_t GetPrice() const;
    int32_t GetDisplayPrice() const;
    int32_t GetCount() const;
    uint32_t GetCharId(const Game& game) const;
};

class Build
{
public:
    uint64_t Uid = 0;
    std::string Name;
    bool Lock = false;
    bool Preference = false;
    uint32_t Score = 0;
    uint32_t TowerId = 0;
    std::vector<uint32_t> CharIds;
    std::vector<uint32_t> DiscIds;
    std::vector<uint32_t> ActiveSecondaryIds;
    std::vector<std::pair<uint32_t, uint32_t>> CharPotentials;
    std::vector<std::pair<uint32_t, uint32_t>> Potentials;
    std::vector<std::pair<uint32_t, int32_t>> SubNoteSkills;

    void LoadFromBin(const ServerProto::TowerBuildBin& bin);
    void SaveToBin(ServerProto::TowerBuildBin& bin) const;
    proto::StarTowerBuildInfo ToProto() const;
    proto::StarTowerBuildBrief ToBriefProto() const;
    proto::StarTowerBuildDetail ToDetailProto() const;
};

class Preset
{
public:
    uint64_t Uid = 0;
    std::string Name;
    bool Preference = false;
    int64_t Timestamp = 0;
    std::vector<std::pair<uint32_t, std::vector<PotentialInfo>>> CharPotentials;

    void LoadFromBin(const ServerProto::TowerPotentialPresetBin& bin);
    void SaveToBin(ServerProto::TowerPotentialPresetBin& bin) const;
    proto::PotentialPreselection ToProto() const;
};

uint64_t GenerateUid();
uint32_t BuildScoreFromPotentialLevel(uint32_t level, const std::vector<int>& buildScores);
uint32_t ClampNameLength(std::string& name);

class Game
{
public:
    ~Game();

    uint32_t TowerId = 0;
    uint32_t FormationId = 0;
    uint64_t BuildId = 0;
    uint32_t FloorCount = 0;
    uint32_t StageNum = 0;
    uint32_t StageFloor = 0;
    uint32_t TeamLevel = 1;
    uint32_t TeamExp = 0;
    uint32_t NextLevelExp = 0;
    int32_t CharHp = -1;
    uint32_t BattleTime = 0;
    std::vector<uint32_t> CharIds;
    std::vector<uint32_t> DiscIds;
    uint32_t PendingPotentialCases = 0;
    uint32_t PendingRarePotentialCases = 0;
    uint32_t ShopRerollTimes = 0;
    uint32_t ShopRerollPrice = 0;
    bool FreeStrengthenAvailable = false;
    bool Completed = false;
    bool Sweep = false;
    std::vector<std::pair<uint32_t, int32_t>> Items;
    std::vector<std::pair<uint32_t, int32_t>> Res;
    std::vector<std::pair<uint32_t, int32_t>> Potentials;
    std::vector<std::pair<uint32_t, int32_t>> NewInfos;
    std::vector<std::pair<uint32_t, int32_t>> RarePotentialCount;
    std::vector<uint32_t> ActiveSecondaryIds;
    std::vector<uint32_t> FateCards;
    std::vector<uint64_t> TotalDamages;
    std::unique_ptr<TowerRoom> Room;

    TowerMgr* GetManager() const { return Manager; }
    void SetManager(TowerMgr* manager) { Manager = manager; }

    proto::StarTowerInfo ToProto() const;
    void SaveToBin(ServerProto::TowerGameBin& bin) const;
    void LoadFromBin(const ServerProto::TowerGameBin& bin);
    void InitModifierState();

    int GetItemCount(uint32_t id) const;
    int GetResCount(uint32_t id) const;
    int GetPotentialLevel(uint32_t id) const;
    int GetRarePotentialCount(uint32_t charId) const;
    void SetHp(int hp);
    void AddBattleTime(uint32_t amount);
    void AddExp(uint32_t amount);
    int LevelUp();
    bool AddRuntimeItem(uint32_t id, int count, proto::ChangeInfo* change = nullptr);
    void FlushNewInfos(proto::TowerChangeData* data);
    void AddPotentialSelectors(uint32_t amount);
    void AddRarePotentialSelectors(uint32_t amount);
    std::unique_ptr<TowerCaseBase> CreatePotentialSelector(uint32_t charId = 0, bool rare = false);
    std::unique_ptr<TowerCaseBase> CreateRarePotentialSelector();
    std::unique_ptr<TowerCaseBase> CreateStrengthenSelector();
    void HandlePendingPotentialSelectors(proto::StarTowerInteractResp& rsp);
    bool IsOnFinalFloor(const StarTowerRes& tower) const;
    uint32_t GetNextStageId(const StarTowerRes& tower) const;
    uint32_t GetDifficulty() const;
    int GetExtraPotentialMaxLevel() const;
    uint32_t GetPotentialRerollCount() const;
    uint32_t GetPotentialRerollPrice() const;
    double GetBonusPotentialChance() const;
    uint32_t GetBonusPotentialLevel() const;
    double GetBonusStrengthenChance() const;
    double GetBattleSubNoteDropChance() const;
    double GetBonusSubNoteChance() const;
    uint32_t GetBonusSubNotes() const;
    uint32_t GetBonusBossSubNotes() const;
    double GetBonusCoinChance() const;
    uint32_t GetBonusCoinCount() const;
    uint32_t GetShopGoodsCount() const;
    uint32_t GetStrengthenDiscount() const;
    double GetBattleNpcEventChance() const;
    bool ConsumeShopReroll();
    void ConsumeFreeStrengthen();
    int GetRandomSubNoteId() const;
    void RefreshSecondarySkills(proto::TowerChangeData* data = nullptr);
    void InitializeSubNotesFromDiscs();
    void AddStartingItems();
    bool EnterNextRoom();
    void Settle(bool victory, proto::StarTowerInteractResp& rsp);
    uint32_t GetTotalPotentialCount() const;
    Build& GetBuild();
    Build BuildSnapshot() const;

private:
    TowerMgr* Manager = nullptr;
    std::unique_ptr<Build> mCachedBuild;
};
}
