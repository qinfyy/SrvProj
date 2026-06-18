#pragma once

#include "ManagerBase.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/player_data.pb.h"
#include "../proto/proto_cpp/public.pb.h"

#include <cstdint>
#include <vector>

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
    bool ConsumeItem(uint32_t tid, int64_t qty, proto::ChangeInfo* change = nullptr);
    int64_t GetItemCount(uint32_t tid) const;

    void PushItemsChange(const proto::ChangeInfo& change);
    bool AddSkin(uint32_t id, proto::ChangeInfo* change = nullptr);
    bool AddHeadIcon(uint32_t id);
    bool AddTitle(uint32_t id);
    bool AddHonor(uint32_t id);

private:
    bool IsResourceItem(uint32_t tid) const;
    void AddItemChange(proto::ChangeInfo* change, uint32_t tid, int64_t qty) const;
    void EnsureDefaultResources();
    bool ContainsRepeated(const google::protobuf::RepeatedField<uint32_t>& values, uint32_t value) const;
};
