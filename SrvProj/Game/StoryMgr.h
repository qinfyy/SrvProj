#pragma once

#include "ManagerBase.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/player_data.pb.h"
#include "../proto/proto_cpp/public.pb.h"
#include "../proto/proto_cpp/story_set_info.pb.h"
#include "../proto/proto_cpp/story_sett.pb.h"

#include <cstdint>

class StoryMgr : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;

    void OnCreate() override;
    void OnLoad() override;
    void EncodePlayerInfo(proto::PlayerInfo& out) const override;

    ServerProto::StoryCompBin* MutableBin();
    const ServerProto::StoryCompBin& Bin() const;

    void Apply(uint32_t storyId);
    void Settle(const proto::StorySettleReq& request, proto::ChangeInfo& change);
    void SettleSet(uint32_t chapterId, uint32_t sectionId, proto::ChangeInfo& change);
    void BuildStorySetInfo(proto::StorySetInfoResp& out) const;
    proto::HandbookInfo BuildCgHandbook() const;
    bool HasNew() const;

private:
    static bool HasChoice(const google::protobuf::RepeatedPtrField<ServerProto::StoryChoiceBin>& choices, uint32_t group, uint32_t value);
    static bool SettleChoices(const google::protobuf::RepeatedPtrField<proto::StoryOptions>& options, google::protobuf::RepeatedPtrField<ServerProto::StoryChoiceBin>* choices);
    void SettleOptions(const proto::StorySettle& settle);

    uint32_t mStoryId = 0;
};
