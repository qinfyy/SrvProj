#include "AgentMgr.h"

#include "CharacterMgr.h"
#include "InventoryMgr.h"
#include "Player.h"
#include "../GameTime.h"
#include "../Resources/BinClass/CharacterRes.h"
#include "../Resources/BinClass/CommissionsRes.h"
#include "../Resources/GameData.h"

#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
constexpr uint32_t kMaxAgents = 4;
constexpr uint32_t kCondAgentApplyTotal = 106;
constexpr uint32_t kCondAgentFinishTotal = 83;
constexpr uint32_t kCondAgentSpecificFinishTotal = 84;

ItemParamMap GenerateRewards(const ItemRewardList& rewards)
{
    ItemParamMap out;
    for (const auto& reward : rewards)
    {
        out.Add(reward.Id, reward.GetRandomCount());
    }
    return out;
}

void AddRewardItems(google::protobuf::RepeatedPtrField<proto::ItemTpl>* out, const ItemParamMap& rewards)
{
    for (const auto& [id, qty] : rewards.Items)
    {
        if (id <= 0 || qty == 0)
        {
            continue;
        }
        auto* item = out->Add();
        item->set_tid(static_cast<uint32_t>(id));
        item->set_qty(qty);
    }
}
}

ServerProto::AgentCompBin* AgentMgr::MutableBin()
{
    return GetPlayer()->SaveData().mutable_agentcomp();
}

const ServerProto::AgentCompBin& AgentMgr::Bin() const
{
    return GetPlayer()->SaveData().agentcomp();
}

void AgentMgr::OnCreate()
{
    MutableBin()->clear_agents();
}

void AgentMgr::OnLoad()
{
    auto* agents = MutableBin()->mutable_agents();
    std::unordered_set<uint32_t> ids;
    for (int index = agents->size() - 1; index >= 0; --index)
    {
        const auto& agent = agents->Get(index);
        if (agent.id() == 0 || agent.processtime() == 0 || agent.starttime() <= 0 ||
            GameData::AgentDataTable.find(static_cast<int>(agent.id())) == GameData::AgentDataTable.end() ||
            !ids.insert(agent.id()).second)
        {
            agents->DeleteSubrange(index, 1);
        }
    }
}

bool AgentMgr::HasRequiredTags(const ServerProto::AgentBin& agent, const std::unordered_map<int, int>& required) const
{
    if (required.empty())
    {
        return true;
    }

    std::unordered_map<int, int> tags;
    for (uint32_t charId : agent.charids())
    {
        const auto dataIt = GameData::CharacterDataTable.find(static_cast<int>(charId));
        if (dataIt == GameData::CharacterDataTable.end() || !dataIt->second.Des)
        {
            return false;
        }
        for (int tag : dataIt->second.Des->Tag)
        {
            ++tags[tag];
        }
    }

    for (const auto& [tag, count] : required)
    {
        const auto it = tags.find(tag);
        if (it == tags.end() || it->second < count)
        {
            return false;
        }
    }
    return true;
}

const ServerProto::AgentBin* AgentMgr::Apply(const proto::AgentApplyInfo& apply)
{
    auto* agents = MutableBin()->mutable_agents();
    if (agents->size() >= static_cast<int>(kMaxAgents))
    {
        return nullptr;
    }

    const auto dataIt = GameData::AgentDataTable.find(static_cast<int>(apply.id()));
    if (dataIt == GameData::AgentDataTable.end() || apply.charids_size() <= 0 ||
        apply.charids_size() > dataIt->second.MemberLimit)
    {
        return nullptr;
    }

    ServerProto::AgentBin candidate;
    candidate.set_id(apply.id());
    candidate.set_processtime(apply.processtime());
    candidate.set_starttime(GameTime::NowSeconds());
    for (uint32_t charId : apply.charids())
    {
        const auto* character = GetPlayer()->Characters().GetCharacterById(static_cast<int>(charId));
        if (!character || character->level() < static_cast<uint32_t>((std::max)(dataIt->second.Level, 0)))
        {
            return nullptr;
        }
        candidate.add_charids(charId);
    }

    if (!HasRequiredTags(candidate, dataIt->second.TagCounts))
    {
        return nullptr;
    }

    for (int index = 0; index < agents->size(); ++index)
    {
        if (agents->Get(index).id() == apply.id())
        {
            agents->DeleteSubrange(index, 1);
            break;
        }
    }

    auto* agent = agents->Add();
    agent->CopyFrom(candidate);
    GetPlayer()->Trigger(kCondAgentApplyTotal, 1);
    return agent;
}

bool AgentMgr::GiveUp(uint32_t id, proto::AgentGiveUpResp& out)
{
    auto* agents = MutableBin()->mutable_agents();
    for (int index = 0; index < agents->size(); ++index)
    {
        const auto& agent = agents->Get(index);
        if (agent.id() != id)
        {
            continue;
        }
        for (uint32_t charId : agent.charids())
        {
            out.add_charids(charId);
        }
        agents->DeleteSubrange(index, 1);
        return true;
    }
    return false;
}

bool AgentMgr::ReceiveReward(uint32_t id, proto::AgentRewardReceiveResp& out)
{
    std::vector<ServerProto::AgentBin> completed;
    const int64_t now = GameTime::NowSeconds();
    for (const auto& agent : Bin().agents())
    {
        if (id > 0 && agent.id() != id)
        {
            continue;
        }
        const int64_t finishTime = agent.starttime() + static_cast<int64_t>(agent.processtime()) * 60;
        if (now >= finishTime)
        {
            completed.push_back(agent);
        }
    }

    if (completed.empty())
    {
        return false;
    }

    auto* agents = MutableBin()->mutable_agents();
    for (const auto& agent : completed)
    {
        for (int index = 0; index < agents->size(); ++index)
        {
            if (agents->Get(index).id() == agent.id())
            {
                agents->DeleteSubrange(index, 1);
                break;
            }
        }

        for (uint32_t charId : agent.charids())
        {
            out.add_charids(charId);
        }

        auto* show = out.add_rewardshows();
        show->set_id(agent.id());

        const auto dataIt = GameData::AgentDataTable.find(static_cast<int>(agent.id()));
        if (dataIt == GameData::AgentDataTable.end())
        {
            continue;
        }
        const auto rewardsIt = dataIt->second.DurationRewards.find(static_cast<int>(agent.processtime()));
        if (rewardsIt == dataIt->second.DurationRewards.end())
        {
            continue;
        }

        const ItemParamMap rewards = GenerateRewards(rewardsIt->second);
        GetPlayer()->Inventory().AddItems(rewards, out.mutable_change());
        AddRewardItems(show->mutable_rewards(), rewards);

        if (!HasRequiredTags(agent, dataIt->second.ExtraTagCounts))
        {
            continue;
        }
        const auto bonusIt = dataIt->second.DurationBonusRewards.find(static_cast<int>(agent.processtime()));
        if (bonusIt == dataIt->second.DurationBonusRewards.end())
        {
            continue;
        }
        const ItemParamMap bonus = GenerateRewards(bonusIt->second);
        GetPlayer()->Inventory().AddItems(bonus, out.mutable_change());
        AddRewardItems(show->mutable_bonus(), bonus);
    }

    const uint32_t count = static_cast<uint32_t>(completed.size());
    GetPlayer()->Trigger(kCondAgentFinishTotal, count);
    GetPlayer()->Trigger(kCondAgentSpecificFinishTotal, count);
    return true;
}

void AgentMgr::EncodePlayerInfo(proto::PlayerInfo& out) const
{
    auto* data = out.mutable_agent();
    for (const auto& agent : Bin().agents())
    {
        auto* info = data->add_infos();
        info->set_id(agent.id());
        info->set_processtime(agent.processtime());
        info->set_starttime(agent.starttime());
        for (uint32_t charId : agent.charids())
        {
            info->add_charids(charId);
        }
    }
}
