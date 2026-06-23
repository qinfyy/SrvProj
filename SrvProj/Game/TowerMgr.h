#pragma once

#include "ManagerBase.h"
#include "TowerRuntime.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/potential_preselection_list.pb.h"
#include "../proto/proto_cpp/player_data.pb.h"
#include "../proto/proto_cpp/public_star_tower.pb.h"
#include "../proto/proto_cpp/star_tower_apply.pb.h"
#include "../proto/proto_cpp/star_tower_build_brief_list_get.pb.h"
#include "../proto/proto_cpp/star_tower_build_delete.pb.h"
#include "../proto/proto_cpp/star_tower_build_detail_get.pb.h"
#include "../proto/proto_cpp/star_tower_build_whether_save.pb.h"
#include "../proto/proto_cpp/star_tower_give_up.pb.h"
#include "../proto/proto_cpp/star_tower_interact.pb.h"
#include "../proto/proto_cpp/tower_book_fate_card_detail.pb.h"
#include "../proto/proto_cpp/npc_affinity_book_get.pb.h"
#include "../proto/proto_cpp/npc_affinity_plot_reward_receive.pb.h"
#include "../proto/proto_cpp/star_tower_book_char_potential_get.pb.h"
#include "../proto/proto_cpp/star_tower_book_event_reward_receive.pb.h"
#include "../proto/proto_cpp/star_tower_book_potential_brief_list_get.pb.h"
#include "../proto/proto_cpp/star_tower_book_potential_reward_receive.pb.h"
#include "../proto/proto_cpp/tower_growth_detail.pb.h"
#include "../proto/proto_cpp/tower_growth_group_node_unlock.pb.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace TowerRuntime { class Game; }
class TowerRoom;
class TowerCaseBase;
class FormationMgr;

class TowerMgr : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;
    ~TowerMgr() override;

    void OnCreate() override;
    void OnLoad() override;
    void OnLogin() override;
    void EncodePlayerInfo(proto::PlayerInfo& out) const override;

    ServerProto::TowerCompBin* MutableBin();
    const ServerProto::TowerCompBin& Bin() const;

    bool Apply(const proto::StarTowerApplyReq& req, proto::StarTowerApplyResp& rsp);
    bool HandleInfo(proto::StarTowerInfo& out) const;
    bool HandleInteract(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp);
    bool GiveUp(proto::StarTowerGiveUpResp& rsp);

    bool BuildBriefList(proto::StarTowerBuildBriefListGetResp& rsp) const;
    bool BuildDetail(uint64_t buildId, proto::StarTowerBuildDetailGetResp& rsp) const;
    bool DeleteBuilds(const google::protobuf::RepeatedField<uint64_t>& buildIds, proto::StarTowerBuildDeleteResp& rsp);
    bool SaveLastBuild(bool removeBuild, const std::string& name, bool lock, proto::StarTowerBuildWhetherSaveResp& rsp);
    bool SetBuildLock(uint64_t buildId, bool lock);
    bool SetBuildName(uint64_t buildId, const std::string& name);
    bool SetBuildPreference(const google::protobuf::RepeatedField<uint64_t>& checkInIds, const google::protobuf::RepeatedField<uint64_t>& checkOutIds);

    bool BuildPresetList(proto::PotentialPreselectionList& rsp) const;
    bool ImportPreset(const std::string& name, bool preference, const google::protobuf::RepeatedPtrField<proto::StarTowerBookCharPotential>& chars, proto::PotentialPreselection& rsp);
    bool UpdatePreset(uint64_t presetId, const google::protobuf::RepeatedPtrField<proto::StarTowerBookCharPotential>& chars, proto::PotentialPreselection& rsp);
    bool SetPresetName(uint64_t presetId, const std::string& name);
    bool SetPresetPreference(const google::protobuf::RepeatedField<uint64_t>& checkInIds, const google::protobuf::RepeatedField<uint64_t>& checkOutIds);
    bool DeletePresets(const google::protobuf::RepeatedField<uint64_t>& ids);

    bool BuildGrowthDetail(proto::TowerGrowthDetailResp& rsp) const;
    bool UnlockGrowthNode(uint32_t nodeId, proto::ChangeInfo& change);
    bool UnlockGrowthGroup(uint32_t groupId, proto::TowerGrowthGroupNodeUnlockResp& rsp);
    bool BuildFateCardDetail(proto::TowerBookFateCardDetailResp& rsp) const;
    bool BuildNpcAffinityBook(proto::NPCAffinityBookGetResp& rsp) const;
    bool ReceiveNpcAffinityPlotReward(uint32_t plotId, proto::NPCAffinityPlotRewardReceiveResp& rsp);
    bool BuildPotentialBriefList(proto::StarTowerBookPotentialBriefListResp& rsp) const;
    bool BuildCharPotential(uint32_t charId, proto::StarTowerBookPotentialGetResp& rsp) const;
    bool ReceivePotentialBookReward(uint32_t potentialId, proto::StarTowerBookPotentialRewardReceiveResp& rsp);
    bool ReceiveEventBookReward(uint32_t eventId, proto::StarTowerBookEventRewardReceiveResp& rsp);
    bool ReceiveFateCardReward(uint32_t bundleId, uint32_t questId, proto::ChangeInfo& outChange);
    void RecordPotentialCollection(uint32_t potentialId, uint32_t level);
    void RecordEventCollection(uint32_t eventId);
    void RecordFateCardCollection(uint32_t cardId);
    uint32_t AddNpcAffinity(uint32_t npcId, uint32_t increase, proto::NPCAffinityChange* outChange = nullptr);

    bool HasGrowthNode(uint32_t nodeId) const;
    bool IsValidPresetForCharacters(uint64_t presetId, const google::protobuf::RepeatedField<uint32_t>& charIds) const;
    bool IsValidPresetForCharacters(uint64_t presetId, const std::vector<uint32_t>& charIds) const;
    proto::StarTowerState BuildStateProto() const;
    proto::StarTowerBookState BuildBookStateProto() const;

    uint32_t GetTowerTickets() const;
    void ResetWeeklyTickets();

private:
    void InitializeDefaults();
    void LoadCurrentGame();
    void SaveCurrentGame();
    void ClearCurrentGame();

    TowerRuntime::Build* FindBuild(uint64_t buildId);
    const TowerRuntime::Build* FindBuild(uint64_t buildId) const;
    TowerRuntime::Preset* FindPreset(uint64_t presetId);
    const TowerRuntime::Preset* FindPreset(uint64_t presetId) const;

    bool UpdatePresetCharacters(TowerRuntime::Preset& preset, const google::protobuf::RepeatedPtrField<proto::StarTowerBookCharPotential>& chars, bool touchTimestamp);
    uint32_t GetWeeklyTowerTicketLimit() const;
    uint32_t GetMaxEarnableWeeklyTowerTickets() const;
    void AddWeeklyTowerTickets(uint32_t count);
    void RebuildBookState();
    std::vector<uint32_t> GetNpcAffinityPlotIds(uint32_t npcId) const;
    uint32_t GetNpcAffinityValue(uint32_t npcId) const;
    uint32_t GetNpcAffinityLevel(uint32_t npcId) const;
    bool HasReceivedNpcPlot(uint32_t plotId) const;
    void SetReceivedNpcPlot(uint32_t plotId);
    bool HasReceivedPotentialBookReward(uint32_t id) const;
    void SetReceivedPotentialBookReward(uint32_t id);
    bool HasReceivedEventBookReward(uint32_t id) const;
    void SetReceivedEventBookReward(uint32_t id);
    bool HasReceivedFateCardReward(uint32_t id) const;
    void SetReceivedFateCardReward(uint32_t id);
    void PushBookPotentialNotify(uint32_t id) const;
    void PushBookEventNotify(uint32_t id) const;
    void PushNpcAffinityNotify(uint32_t npcId, uint32_t affinity, uint32_t increase) const;
    void PushFateCardCollectNotify(uint32_t cardId) const;
    void PushFateCardRewardNotify(uint32_t id, bool questReward) const;
    void RebuildNpcAffinityBookState();
    int FindNpcAffinityIndex(uint32_t npcId) const;

    std::vector<TowerRuntime::Build> mBuilds;
    std::vector<TowerRuntime::Preset> mPresets;
    std::unique_ptr<TowerRuntime::Game> mCurrentGame;
    std::unique_ptr<TowerRuntime::Build> mLastBuild;
};
