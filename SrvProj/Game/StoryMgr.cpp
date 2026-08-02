#include "StoryMgr.h"

#include "Bitset.h"
#include "InventoryMgr.h"
#include "Player.h"
#include "../Config.h"
#include "../Resources/BinClass/MiscRes.h"
#include "../Resources/BinClass/StoryRes.h"
#include "../Resources/GameData.h"

#include <algorithm>
#include <unordered_set>
#include <vector>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

void StoryMgr::OnCreate()
{
    MutableBin();
}

void StoryMgr::OnLoad()
{
    MutableBin();
    mStoryId = 0;
}

ServerProto::StoryCompBin* StoryMgr::MutableBin()
{
    return GetPlayer()->SaveData().mutable_storycomp();
}

const ServerProto::StoryCompBin& StoryMgr::Bin() const
{
    return GetPlayer()->SaveData().storycomp();
}

void StoryMgr::Apply(uint32_t storyId)
{
    mStoryId = storyId;
}

bool StoryMgr::HasChoice(const google::protobuf::RepeatedPtrField<ServerProto::StoryChoiceBin>& choices, uint32_t group, uint32_t value)
{
    for (const auto& choice : choices)
    {
        if (choice.group() == group && choice.value() == value)
        {
            return true;
        }
    }
    return false;
}

bool StoryMgr::SettleChoices(const google::protobuf::RepeatedPtrField<proto::StoryOptions>& options, google::protobuf::RepeatedPtrField<ServerProto::StoryChoiceBin>* choices)
{
    bool changed = false;
    for (const auto& option : options)
    {
        if (choices->size() >= 5)
        {
            break;
        }
        if (HasChoice(*choices, option.group(), option.choice()))
        {
            continue;
        }

        auto* choice = choices->Add();
        choice->set_group(option.group());
        choice->set_value(option.choice());
        changed = true;
    }
    return changed;
}

void StoryMgr::SettleOptions(const proto::StorySettle& settle)
{
    if (settle.major().empty() && settle.personality().empty())
    {
        return;
    }

    auto* log = &(*MutableBin()->mutable_options())[settle.idx()];
    SettleChoices(settle.major(), log->mutable_major());
    SettleChoices(settle.personality(), log->mutable_personality());
}

void StoryMgr::Settle(const proto::StorySettleReq& request, proto::ChangeInfo& change)
{
    auto* bin = MutableBin();
    for (const auto& settle : request.list())
    {
        const auto dataIt = GameData::StoryDataTable.find(static_cast<int>(settle.idx()));
        if (dataIt == GameData::StoryDataTable.end())
        {
            continue;
        }

        SettleOptions(settle);
        if (std::find(bin->completedstories().begin(), bin->completedstories().end(), settle.idx()) != bin->completedstories().end())
        {
            continue;
        }

        bin->add_completedstories(settle.idx());
        GetPlayer()->Inventory().AddItems(dataIt->second.Rewards, &change);
        GetPlayer()->Trigger(63, 1, settle.idx(), 0);
    }

    for (uint32_t evidenceId : request.evidences())
    {
        if (GameData::StoryEvidenceDataTable.find(static_cast<int>(evidenceId)) == GameData::StoryEvidenceDataTable.end())
        {
            continue;
        }
        if (std::find(bin->evidences().begin(), bin->evidences().end(), evidenceId) != bin->evidences().end())
        {
            continue;
        }
        bin->add_evidences(evidenceId);
    }

    mStoryId = 0;
}

void StoryMgr::SettleSet(uint32_t chapterId, uint32_t sectionId, proto::ChangeInfo& change)
{
    const auto dataIt = GameData::StorySetSectionDataTable.find(static_cast<int>(sectionId));
    if (dataIt == GameData::StorySetSectionDataTable.end())
    {
        return;
    }

    const uint32_t sectionIndex = sectionId % 10;
    auto* completedSets = MutableBin()->mutable_completedsets();
    const auto completedIt = completedSets->find(chapterId);
    const uint32_t completedIndex = completedIt == completedSets->end() ? 0 : completedIt->second;
    if (completedIndex >= sectionIndex)
    {
        return;
    }

    (*completedSets)[chapterId] = sectionIndex;
    GetPlayer()->Inventory().AddItems(dataIt->second.Rewards, &change);
}

void StoryMgr::BuildStorySetInfo(proto::StorySetInfoResp& out) const
{
    std::vector<uint32_t> chapterIds;
    std::unordered_set<uint32_t> seen;
    for (const auto& [_, section] : GameData::StorySetSectionDataTable)
    {
        const uint32_t chapterId = static_cast<uint32_t>(std::max(section.ChapterId, 0));
        if (chapterId > 0 && seen.insert(chapterId).second)
        {
            chapterIds.push_back(chapterId);
        }
    }
    std::sort(chapterIds.begin(), chapterIds.end());

    for (uint32_t chapterId : chapterIds)
    {
        uint32_t sectionIndex = 0;
        const auto completedIt = Bin().completedsets().find(chapterId);
        if (completedIt != Bin().completedsets().end())
        {
            sectionIndex = completedIt->second;
        }

        auto* chapter = out.add_chapters();
        chapter->set_chapterid(chapterId);
        chapter->set_sectionindex(sectionIndex);
        for (uint32_t index = 1; index <= sectionIndex; ++index)
        {
            chapter->add_rewardedids((chapterId * 100) + index);
        }
    }
}

proto::HandbookInfo StoryMgr::BuildCgHandbook() const
{
    Bitset bitset;
    if (Config::Get().unlockAllStoryCGs)
    {
        for (const auto& [id, _] : GameData::MainScreenCGDataTable)
        {
            const auto handbookIt = GameData::HandbookDataTable.find(id);
            if (handbookIt == GameData::HandbookDataTable.end() || handbookIt->second.Type != 3)
            {
                continue;
            }
            bitset.SetBit(static_cast<uint32_t>(std::max(handbookIt->second.Index, 0)));
        }
    }

    proto::HandbookInfo handbook;
    handbook.set_type(3);
    handbook.set_data(bitset.ToByteArray());
    return handbook;
}

bool StoryMgr::HasNew() const
{
    return static_cast<size_t>(Bin().completedstories_size()) < GameData::StoryDataTable.size() ||
        static_cast<size_t>(Bin().completedsets_size()) < GameData::StorySetSectionDataTable.size();
}

void StoryMgr::EncodePlayerInfo(proto::PlayerInfo& out) const
{
    auto* storyInfo = out.mutable_story();
    for (uint32_t storyId : Bin().completedstories())
    {
        auto* story = storyInfo->add_stories();
        story->set_idx(storyId);

        const auto optionIt = Bin().options().find(storyId);
        if (optionIt == Bin().options().end())
        {
            continue;
        }

        for (const auto& choice : optionIt->second.major())
        {
            auto* encoded = story->add_major();
            encoded->set_group(choice.group());
            encoded->set_value(choice.value());
        }
        for (const auto& choice : optionIt->second.personality())
        {
            auto* encoded = story->add_major();
            encoded->set_group(choice.group());
            encoded->set_value(choice.value());
        }
    }

    for (uint32_t evidenceId : Bin().evidences())
    {
        storyInfo->add_evidences(evidenceId);
    }

    out.add_handbook()->CopyFrom(BuildCgHandbook());
}
