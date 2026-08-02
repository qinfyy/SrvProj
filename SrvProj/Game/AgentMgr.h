#pragma once

#include "ManagerBase.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/agent_apply.pb.h"
#include "../proto/proto_cpp/agent_give_up.pb.h"
#include "../proto/proto_cpp/agent_reward_receive.pb.h"
#include "../proto/proto_cpp/player_data.pb.h"

#include <cstdint>
#include <unordered_map>

class AgentMgr : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;

    void OnCreate() override;
    void OnLoad() override;
    void EncodePlayerInfo(proto::PlayerInfo& out) const override;

    const ServerProto::AgentBin* Apply(const proto::AgentApplyInfo& apply);
    bool GiveUp(uint32_t id, proto::AgentGiveUpResp& out);
    bool ReceiveReward(uint32_t id, proto::AgentRewardReceiveResp& out);

private:
    ServerProto::AgentCompBin* MutableBin();
    const ServerProto::AgentCompBin& Bin() const;
    bool HasRequiredTags(const ServerProto::AgentBin& agent, const std::unordered_map<int, int>& required) const;
};
