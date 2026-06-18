#include "MailMgr.h"

#include "InventoryMgr.h"
#include "Player.h"
#include "../GameConstants.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/mail_recv.pb.h"

#include <algorithm>
#include <chrono>
#include <limits>

namespace {
int32_t ClampQty(int64_t qty)
{
    if (qty > std::numeric_limits<int32_t>::max())
    {
        return std::numeric_limits<int32_t>::max();
    }

    if (qty < std::numeric_limits<int32_t>::min())
    {
        return std::numeric_limits<int32_t>::min();
    }

    return static_cast<int32_t>(qty);
}
}

void MailMgr::OnCreate()
{
    EnsureDefaults();
}

void MailMgr::OnLoad()
{
    EnsureDefaults();
}

void MailMgr::EncodePlayerInfo(proto::PlayerInfo& out) const
{
    out.mutable_state()->mutable_mail()->set_new_(HasNewMail());
}

ServerProto::MailCompBin* MailMgr::MutableBin()
{
    return GetPlayer()->SaveData().mutable_mailcomp();
}

const ServerProto::MailCompBin& MailMgr::Bin() const
{
    return GetPlayer()->SaveData().mailcomp();
}

void MailMgr::EnsureDefaults()
{
    auto* bin = MutableBin();
    if (bin->nextmailid() == 0)
    {
        bin->set_nextmailid(1);
    }

    if (!bin->welcomemailcreated())
    {
        std::vector<std::pair<uint32_t, int64_t>> attachments = {
            {GameConstants::GoldItemId, 1000000},
            {GameConstants::GemItemId, 30000}
        };
        AddSystemMail("Server", "Welcome to SrvProj! Please take these items as a starter gift.", attachments, false);
        bin->set_welcomemailcreated(true);
    }
}

uint32_t MailMgr::AllocateMailId()
{
    auto* bin = MutableBin();
    uint32_t id = bin->nextmailid();
    if (id == 0)
    {
        id = 1;
    }
    bin->set_nextmailid(id + 1);
    return id;
}

uint32_t MailMgr::AddSystemMail(const std::string& subject,
    const std::string& desc,
    const std::vector<std::pair<uint32_t, int64_t>>& attachments,
    bool notify)
{
    auto* mail = MutableBin()->add_mails();
    const int64_t now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    mail->set_id(AllocateMailId());
    mail->set_subject(subject);
    mail->set_desc(desc);
    mail->set_author("System");
    mail->set_time(now);
    mail->set_deadline(now + 30LL * 24LL * 60LL * 60LL);
    mail->set_read(false);
    mail->set_recv(false);
    mail->set_pin(false);
    mail->set_flag(0);

    for (const auto& [tid, qty] : attachments)
    {
        if (tid == 0 || qty <= 0)
        {
            continue;
        }

        auto* attachment = mail->add_attachments();
        attachment->set_tid(tid);
        attachment->set_qty(qty);
    }

    if (notify)
    {
        PushMailState(true);
    }

    return mail->id();
}

bool MailMgr::HasNewMail() const
{
    for (const auto& mail : Bin().mails())
    {
        if (!mail.read() || (!mail.recv() && mail.attachments_size() > 0))
        {
            return true;
        }
    }

    return false;
}

proto::Mails MailMgr::ToProto() const
{
    proto::Mails out;
    for (const auto& mail : Bin().mails())
    {
        out.add_list()->CopyFrom(ToProto(mail));
    }
    return out;
}

ServerProto::MailInfoBin* MailMgr::FindMail(uint32_t id)
{
    auto* bin = MutableBin();
    for (int i = 0; i < bin->mails_size(); ++i)
    {
        auto* mail = bin->mutable_mails(i);
        if (mail->id() == id)
        {
            return mail;
        }
    }

    return nullptr;
}

const ServerProto::MailInfoBin* MailMgr::FindMail(uint32_t id) const
{
    for (const auto& mail : Bin().mails())
    {
        if (mail.id() == id)
        {
            return &mail;
        }
    }

    return nullptr;
}

bool MailMgr::MarkRead(uint32_t id)
{
    auto* mail = FindMail(id);
    if (!mail)
    {
        return false;
    }

    mail->set_read(true);
    return true;
}

bool MailMgr::SetPin(uint32_t id, bool pin, uint64_t flag)
{
    auto* mail = FindMail(id);
    if (!mail)
    {
        return false;
    }

    mail->set_pin(pin);
    mail->set_flag(flag);
    return true;
}

bool MailMgr::Remove(uint32_t id, std::vector<uint32_t>& removedIds)
{
    auto* bin = MutableBin();
    for (int i = 0; i < bin->mails_size(); ++i)
    {
        if (bin->mails(i).id() != id)
        {
            continue;
        }

        bin->mutable_mails()->DeleteSubrange(i, 1);
        removedIds.push_back(id);
        PushMailState(HasNewMail(), true);
        return true;
    }

    return false;
}

bool MailMgr::Receive(uint32_t id, proto::MailRecvResp& rsp)
{
    auto* mail = FindMail(id);
    if (!mail || mail->recv())
    {
        return false;
    }

    std::vector<std::pair<uint32_t, int64_t>> attachments;
    attachments.reserve(mail->attachments_size());
    for (const auto& attachment : mail->attachments())
    {
        if (attachment.tid() == 0 || attachment.qty() <= 0)
        {
            continue;
        }
        attachments.emplace_back(attachment.tid(), attachment.qty());
    }

    if (!attachments.empty())
    {
        GetPlayer()->Inventory().AddItems(attachments, rsp.mutable_items());
    }

    mail->set_recv(true);
    mail->set_read(true);
    rsp.add_ids(id);
    PushMailState(HasNewMail());
    return true;
}

void MailMgr::PushMailState(bool hasNew, bool revoke)
{
    proto::MailState state;
    state.set_new_(hasNew);
    state.set_revoke(revoke);
    GetPlayer()->PushNextPackage(mail_state_notify, state);
}

proto::Mail MailMgr::ToProto(const ServerProto::MailInfoBin& mail) const
{
    proto::Mail out;
    out.set_id(mail.id());
    out.set_subject(mail.subject());
    out.set_desc(mail.desc());
    out.set_author(mail.author());
    out.set_time(mail.time());
    out.set_deadline(mail.deadline());
    out.set_read(mail.read());
    out.set_recv(mail.recv());
    out.set_pin(mail.pin());
    out.set_flag(mail.flag());

    for (const auto& attachment : mail.attachments())
    {
        auto* item = out.add_attachments();
        item->set_tid(attachment.tid());
        item->set_qty(ClampQty(attachment.qty()));
    }

    return out;
}
