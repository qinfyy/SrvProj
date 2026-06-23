#pragma once

#include "ManagerBase.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/public.pb.h"

#include <cstdint>

class FormationMgr : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;

    void OnCreate() override;
    void OnLoad() override;
    void EncodePlayerInfo(proto::PlayerInfo& out) const override;

    ServerProto::FormationCompBin* MutableBin();
    const ServerProto::FormationCompBin& Bin() const;

    bool UpdateFormation(const proto::FormationInfo& info);
    ServerProto::FormationInfoBin* GetFormationById(uint32_t id);
    const ServerProto::FormationInfoBin* GetFormationById(uint32_t id) const;

private:
    void InitializeDefaults();
    void NormalizeFormation(ServerProto::FormationInfoBin& info) const;
    bool HasCharacter(uint32_t charId) const;
    bool HasDisc(uint32_t discId) const;
};
