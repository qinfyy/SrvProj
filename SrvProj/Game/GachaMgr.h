#pragma once

#include "ManagerBase.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/gacha_histories.pb.h"
#include "../proto/proto_cpp/gacha_information.pb.h"
#include "../proto/proto_cpp/gacha_newbie_info.pb.h"
#include "../proto/proto_cpp/gacha_newbie_spin.pb.h"
#include "../proto/proto_cpp/gacha_spin.pb.h"
#include "../proto/proto_cpp/public.pb.h"

#include <cstdint>
#include <map>
#include <optional>
#include <vector>

class GachaRes;
class GachaStorageRes;

class GachaMgr : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;

    void OnCreate() override;
    void OnLoad() override;
    void BeforeSave() override;

    ServerProto::GachaCompBin* MutableBin();
    const ServerProto::GachaCompBin& Bin() const;

    bool Spin(uint32_t bannerId, uint32_t amount, proto::GachaSpinResp& out);
    proto::GachaInformationResp BuildInformation();
    bool BuildHistories(uint32_t storageId, proto::GachaHistories& out) const;
    bool ReceiveGuarantee(uint32_t bannerId, proto::ChangeInfo& out);

    proto::GachaNewbieInfoResp BuildNewbieInfo();
    bool SpinNewbie(uint32_t newbieId, proto::GachaNewbieSpinResp& out);
    bool SaveNewbie(uint32_t newbieId, std::optional<uint32_t> index);
    bool ObtainNewbie(uint32_t newbieId, uint32_t index, proto::ChangeInfo& out);

private:
    struct BannerDraft {
        uint32_t Total = 0;
        bool UsedFirstTen = false;
        bool UsedGuarantee = false;
    };

    struct PityDraft {
        uint32_t MissTimesA = 0;
        uint32_t MissTimesUpA = 0;
        uint32_t MissTimesB = 0;
        bool BGuaranteeDebt = false;
    };

    struct CostPlan {
        uint32_t SpecificItemId = 0;
        int64_t ConsumeSpecificQty = 0;
        int64_t ConvertReq = 0;
        int64_t ConsumeDefaultQty = 0;
    };

    struct AcquireEntry {
        int Type = 0;
        uint32_t Begin = 0;
        uint32_t Count = 0;
    };

    struct RewardPlan {
        std::map<uint32_t, AcquireEntry> AcquireItems;
        std::map<uint32_t, int64_t> GrantItems;
        std::map<uint32_t, int64_t> TransformSrc;
        std::map<uint32_t, int64_t> TransformDst;
        uint32_t CharacterCount = 0;
    };

    ServerProto::GachaBannerInfoBin* UpsertBanner(uint32_t bannerId);
    const ServerProto::GachaBannerInfoBin* FindBanner(uint32_t bannerId) const;
    ServerProto::GachaPityStateBin* UpsertPity(uint32_t storageId);
    const ServerProto::GachaPityStateBin* FindPity(uint32_t storageId) const;
    ServerProto::NewbieGachaStateBin* GetOrCreateNewbieState(uint32_t newbieId, uint32_t spinCount, uint32_t saveCount);

    BannerDraft CopyBanner(uint32_t bannerId) const;
    PityDraft CopyPity(uint32_t storageId) const;
    void SaveBanner(uint32_t bannerId, const BannerDraft& draft);
    void SavePity(uint32_t storageId, const PityDraft& draft);

    bool BuildCostPlan(const GachaRes& data, const GachaStorageRes& storage, uint32_t amount, CostPlan& out) const;
    bool ApplyCostPlan(const GachaStorageRes& storage, const CostPlan& plan, proto::ChangeInfo& change);
    bool PullOnce(const GachaRes& data, PityDraft& pity, uint32_t& itemId) const;
    bool BuildRewardPlan(const std::vector<uint32_t>& cards, const std::map<uint32_t, int64_t>& bonusItems, RewardPlan& out) const;
    bool ApplyRewardPlan(const RewardPlan& plan, proto::ChangeInfo& change);
    bool ApplyGrantItem(uint32_t itemId, int64_t qty, proto::ChangeInfo& change);

    proto::GachaInfo BuildInfoProto(const GachaRes& data, const ServerProto::GachaBannerInfoBin& banner) const;
    void AppendHistory(uint32_t storageId, uint32_t gachaId, const std::vector<uint32_t>& cards, int64_t now);
    void TrimHistories(int64_t now);

    bool RollNewbieTen(const GachaRes& data, std::vector<uint32_t>& cards) const;
    bool CopyNewbieCards(const ServerProto::NewbieGachaStateBin& state, uint32_t index, std::vector<uint32_t>& cards) const;
};
