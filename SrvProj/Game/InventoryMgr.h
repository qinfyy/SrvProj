#pragma once

#include "ManagerBase.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/player_data.pb.h"
#include "../proto/proto_cpp/public.pb.h"
#include "../Resources/ResourceDerivedData.h"

#include <cstdint>
#include <string>
#include <vector>

class MallGemRes;
class MallMonthlyCardRes;
class MallPackageRes;
class MallShopRes;

class InventoryMgr : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;

    void OnCreate() override;
    void OnLoad() override;
    void EncodePlayerInfo(proto::PlayerInfo& out) const override;

    ServerProto::InventoryCompBin* MutableBin();
    const ServerProto::InventoryCompBin& Bin() const;

    bool AddItem(uint32_t tid, int64_t qty, proto::ChangeInfo* change = nullptr);
    bool AddItems(const std::vector<std::pair<uint32_t, int64_t>>& items, proto::ChangeInfo* change = nullptr);
    bool AddItems(const ItemParamMap& items, proto::ChangeInfo* change = nullptr);
    bool ConsumeItem(uint32_t tid, int64_t qty, proto::ChangeInfo* change = nullptr);
    bool RemoveItem(uint32_t tid, int64_t qty, proto::ChangeInfo* change = nullptr);
    bool RemoveItems(const ItemParamMap& items, proto::ChangeInfo* change = nullptr);
    bool HasItem(uint32_t tid, int64_t qty) const;
    bool HasItems(const ItemParamMap& items) const;
    int64_t GetItemCount(uint32_t tid) const;
    int64_t GetResourceCount(uint32_t tid) const;

    void PushItemsChange(const proto::ChangeInfo& change);
    bool AddSkin(uint32_t id, proto::ChangeInfo* change = nullptr);
    bool AddHeadIcon(uint32_t id);
    bool AddTitle(uint32_t id);
    bool AddHonor(uint32_t id);

    bool BuyItem(uint32_t currencyId, int64_t currencyCount, const ItemParamMap& products, uint32_t buyCount, proto::ChangeInfo& change);
    bool UseItem(uint32_t id, uint32_t count, uint32_t selectId, proto::ChangeInfo& change);
    bool Produce(uint32_t id, uint32_t count, proto::ChangeInfo& change);
    bool BuyMallPackage(const MallPackageRes& data, proto::ChangeInfo& change);
    bool BuyMallGem(const MallGemRes& data, proto::ChangeInfo& change);
    bool BuyMallMonthlyCard(const MallMonthlyCardRes& data, proto::ChangeInfo& change);
    bool BuyMallShopItem(const MallShopRes& data, uint32_t buyCount, proto::ChangeInfo& change);

    uint32_t GetShopPurchaseCount(uint32_t id) const;
    void AddShopPurchaseCount(uint32_t id, uint32_t count);
    uint32_t GetMallShopPurchaseCount(const std::string& id) const;
    void AddMallShopPurchaseCount(const std::string& id, uint32_t count);
    uint32_t GetMallPackagePurchaseCount(const std::string& id) const;
    void AddMallPackagePurchaseCount(const std::string& id, uint32_t count);
    uint32_t GetMallMonthlyCardPurchaseCount(const std::string& id) const;
    void AddMallMonthlyCardPurchaseCount(const std::string& id, uint32_t count);
    bool HasMallGemMaidenBonus(const std::string& id) const;
    void SetMallGemMaidenClaimed(const std::string& id, bool claimed);
    bool HasMallPackageCurrency(uint32_t currencyId, int64_t qty) const;
    bool ConsumeMallPackageCurrency(uint32_t currencyId, int64_t qty, proto::ChangeInfo& change);

private:
    void EnsureDefaultResources();
    bool ContainsRepeated(const google::protobuf::RepeatedField<uint32_t>& values, uint32_t value) const;
    uint32_t GetStringCounter(const google::protobuf::Map<std::string, uint32_t>& counters, const std::string& key) const;
    void AddStringCounter(google::protobuf::Map<std::string, uint32_t>* counters, const std::string& key, uint32_t count);
};
