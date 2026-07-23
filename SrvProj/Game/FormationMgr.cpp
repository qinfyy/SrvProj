#include "FormationMgr.h"

#include "CharacterMgr.h"
#include "Player.h"
#include "TowerMgr.h"
#include "../GameConstants.h"

#include <algorithm>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {
constexpr uint32_t kMaxFormations = 10;
}

std::vector<uint32_t> FormationMgr::CollectPositiveIds(const google::protobuf::RepeatedField<uint32_t>& ids)
{
    std::vector<uint32_t> out;
    out.reserve(static_cast<size_t>(ids.size()));

    for (uint32_t id : ids)
    {
        if (id > 0)
        {
            out.push_back(id);
        }
    }

    return out;
}

void FormationMgr::OnCreate()
{
    MutableBin()->clear_infos();
    InitializeDefaults();
}

void FormationMgr::OnLoad()
{
    InitializeDefaults();
}

ServerProto::FormationCompBin* FormationMgr::MutableBin()
{
    return GetPlayer()->SaveData().mutable_formationcomp();
}

const ServerProto::FormationCompBin& FormationMgr::Bin() const
{
    return GetPlayer()->SaveData().formationcomp();
}

void FormationMgr::InitializeDefaults()
{
    auto* bin = MutableBin();

    // 先收集唯一 number 的编队，再写回，避免重复 Number 与 DeleteSubrange 依赖。
    std::vector<ServerProto::FormationInfoBin> uniqueInfos;
    uniqueInfos.reserve(static_cast<size_t>(bin->infos_size()));
    std::unordered_set<uint32_t> seenNumbers;
    for (int i = 0; i < bin->infos_size(); ++i)
    {
        ServerProto::FormationInfoBin info = bin->infos(i);
        NormalizeFormation(info);
        const uint32_t number = info.number();
        if (number == 0 || !seenNumbers.insert(number).second)
        {
            continue;
        }
        uniqueInfos.push_back(std::move(info));
    }

    bin->clear_infos();
    for (auto& info : uniqueInfos)
    {
        bin->add_infos()->Swap(&info);
    }

    if (bin->infos_size() > 0)
    {
        return;
    }

    auto* info = bin->add_infos();
    info->set_number(1);

    if (GetPlayer()->Characters().HasCharacter(103))
    {
        info->add_charids(103);
    }
    if (GetPlayer()->Characters().HasCharacter(112))
    {
        info->add_charids(112);
    }
    if (GetPlayer()->Characters().HasCharacter(113))
    {
        info->add_charids(113);
    }

    if (GetPlayer()->Characters().HasDisc(211001))
    {
        info->add_discids(211001);
    }
    if (GetPlayer()->Characters().HasDisc(211005))
    {
        info->add_discids(211005);
    }
    if (GetPlayer()->Characters().HasDisc(211007))
    {
        info->add_discids(211007);
    }

    NormalizeFormation(*info);
}

void FormationMgr::NormalizeFormation(ServerProto::FormationInfoBin& info) const
{
    if (info.number() == 0)
    {
        info.set_number(1);
    }

    while (info.charids_size() > 3)
    {
        info.mutable_charids()->RemoveLast();
    }
    while (info.discids_size() > 6)
    {
        info.mutable_discids()->RemoveLast();
    }
}

bool FormationMgr::HasCharacter(uint32_t charId) const
{
    return charId > 0 && GetPlayer()->Characters().HasCharacter(static_cast<int>(charId));
}

bool FormationMgr::HasDisc(uint32_t discId) const
{
    return discId > 0 && GetPlayer()->Characters().HasDisc(static_cast<int>(discId));
}

bool FormationMgr::UpdateFormation(const proto::FormationInfo& info)
{
    if (info.number() == 0 || info.number() > kMaxFormations)
    {
        return false;
    }

    if (info.charids_size() < 1 || info.charids_size() > 3)
    {
        return false;
    }

    if (info.discids_size() < 3 || info.discids_size() > 6)
    {
        return false;
    }

    for (uint32_t charId : CollectPositiveIds(info.charids()))
    {
        if (!HasCharacter(charId))
        {
            return false;
        }
    }

    for (uint32_t discId : CollectPositiveIds(info.discids()))
    {
        if (!HasDisc(discId))
        {
            return false;
        }
    }

    auto* saved = GetFormationById(info.number());
    if (!saved)
    {
        saved = MutableBin()->add_infos();
    }

    saved->clear_charids();
    saved->clear_discids();
    saved->set_number(info.number());

    for (uint32_t charId : info.charids())
    {
        saved->add_charids(charId);
    }
    for (uint32_t discId : info.discids())
    {
        saved->add_discids(discId);
    }

    uint64_t presetId = 0;
    if (info.preselectionid() > 0 && GetPlayer()->Towers().IsValidPresetForCharacters(info.preselectionid(), info.charids()))
    {
        presetId = info.preselectionid();
    }
    saved->set_preselectionid(presetId);

    NormalizeFormation(*saved);
    return true;
}

ServerProto::FormationInfoBin* FormationMgr::GetFormationById(uint32_t id)
{
    if (id == 0)
    {
        return nullptr;
    }

    auto* bin = MutableBin();
    for (int i = 0; i < bin->infos_size(); ++i)
    {
        auto* info = bin->mutable_infos(i);
        if (info->number() == id)
        {
            return info;
        }
    }

    return nullptr;
}

const ServerProto::FormationInfoBin* FormationMgr::GetFormationById(uint32_t id) const
{
    if (id == 0)
    {
        return nullptr;
    }

    for (const auto& info : Bin().infos())
    {
        if (info.number() == id)
        {
            return &info;
        }
    }

    return nullptr;
}

void FormationMgr::EncodePlayerInfo(proto::PlayerInfo& out) const
{
    auto* formation = out.mutable_formation();
    std::unordered_set<uint32_t> seenNumbers;
    for (const auto& saved : Bin().infos())
    {
        const uint32_t number = saved.number() == 0 ? 1 : saved.number();
        if (!seenNumbers.insert(number).second)
        {
            continue;
        }

        auto* info = formation->add_info();
        info->set_number(number);
        info->set_preselectionid(saved.preselectionid());
        for (uint32_t charId : saved.charids())
        {
            info->add_charids(charId);
        }
        for (uint32_t discId : saved.discids())
        {
            info->add_discids(discId);
        }
    }
}
