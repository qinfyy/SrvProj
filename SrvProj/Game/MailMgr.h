#pragma once

#include "ManagerBase.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/mail_recv.pb.h"
#include "../proto/proto_cpp/public.pb.h"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

class MailMgr : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;

    void OnCreate() override;
    void OnLoad() override;
    void EncodePlayerInfo(proto::PlayerInfo& out) const override;

    ServerProto::MailCompBin* MutableBin();
    const ServerProto::MailCompBin& Bin() const;

    uint32_t AddSystemMail(const std::string& subject,
        const std::string& desc,
        const std::vector<std::pair<uint32_t, int64_t>>& attachments,
        bool notify = true);

    bool HasNewMail() const;
    proto::Mails ToProto() const;
    bool MarkRead(uint32_t id);
    bool SetPin(uint32_t id, bool pin, uint64_t flag);
    bool Remove(uint32_t id, std::vector<uint32_t>& removedIds);
    bool Receive(uint32_t id, proto::MailRecvResp& rsp);
    bool ReceiveAll(proto::MailRecvResp& rsp);
    void PushMailState(bool hasNew, bool revoke = false);

private:
    void EnsureDefaults();
    uint32_t AllocateMailId();
    ServerProto::MailInfoBin* FindMail(uint32_t id);
    const ServerProto::MailInfoBin* FindMail(uint32_t id) const;
    proto::Mail ToProto(const ServerProto::MailInfoBin& mail) const;
};
