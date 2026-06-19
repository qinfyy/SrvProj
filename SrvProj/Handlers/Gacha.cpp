#include "Gacha.h"

#include "../Game/Player.h"
#include "../Game/GachaMgr.h"
#include "../Game/InventoryMgr.h"
#include "../GameSession.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/gacha_histories.pb.h"
#include "../proto/proto_cpp/gacha_information.pb.h"
#include "../proto/proto_cpp/gacha_newbie_info.pb.h"
#include "../proto/proto_cpp/gacha_newbie_obtain.pb.h"
#include "../proto/proto_cpp/gacha_newbie_save.pb.h"
#include "../proto/proto_cpp/gacha_newbie_spin.pb.h"
#include "../proto/proto_cpp/gacha_spin.pb.h"
#include "../proto/proto_cpp/public.pb.h"

#include <cstdint>
#include <optional>

namespace {
bool HasPlayer(GameSession* session)
{
    return session && session->HasPlayer();
}

std::optional<uint32_t> ResolveGachaAmount(uint32_t mode)
{
    switch (mode)
    {
    case 0:
    case 2:
        return 10;
    case 1:
        return 1;
    default:
        return std::nullopt;
    }
}

bool ReadVarint(const std::string& data, size_t& offset, uint64_t& value)
{
    value = 0;
    int shift = 0;
    while (offset < data.size() && shift <= 63)
    {
        const uint8_t byte = static_cast<uint8_t>(data[offset++]);
        value |= static_cast<uint64_t>(byte & 0x7F) << shift;
        if ((byte & 0x80) == 0)
        {
            return true;
        }
        shift += 7;
    }
    return false;
}

bool SkipField(const std::string& data, size_t& offset, uint32_t wireType)
{
    uint64_t value = 0;
    switch (wireType)
    {
    case 0:
        return ReadVarint(data, offset, value);
    case 1:
        if (offset + 8 > data.size())
        {
            return false;
        }
        offset += 8;
        return true;
    case 2:
        if (!ReadVarint(data, offset, value) || offset + value > data.size())
        {
            return false;
        }
        offset += static_cast<size_t>(value);
        return true;
    case 5:
        if (offset + 4 > data.size())
        {
            return false;
        }
        offset += 4;
        return true;
    default:
        return false;
    }
}

bool HasProtoField(const std::string& data, uint32_t fieldNumber)
{
    size_t offset = 0;
    while (offset < data.size())
    {
        uint64_t tag = 0;
        if (!ReadVarint(data, offset, tag))
        {
            return false;
        }

        const uint32_t currentField = static_cast<uint32_t>(tag >> 3);
        const uint32_t wireType = static_cast<uint32_t>(tag & 0x07);
        if (currentField == fieldNumber)
        {
            return true;
        }

        if (!SkipField(data, offset, wireType))
        {
            return false;
        }
    }

    return false;
}
}

std::string gacha_spin_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, gacha_spin_failed_ack);
    }

    proto::GachaSpinReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, gacha_spin_failed_ack);
    }

    auto amount = ResolveGachaAmount(request.mode());
    if (!amount)
    {
        return EncodeReply(session, gacha_spin_failed_ack);
    }

    proto::GachaSpinResp response;
    if (!session->GetPlayer()->Gachas().Spin(request.id(), *amount, response))
    {
        return EncodeReply(session, gacha_spin_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, gacha_spin_succeed_ack, &response);
}

std::string gacha_information_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, gacha_information_failed_ack);
    }

    auto response = session->GetPlayer()->Gachas().BuildInformation();
    session->SavePlayer();
    return EncodeReply(session, gacha_information_succeed_ack, &response);
}

std::string gacha_histories_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, gacha_histories_failed_ack);
    }

    proto::UI32 request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, gacha_histories_failed_ack);
    }

    proto::GachaHistories response;
    if (!session->GetPlayer()->Gachas().BuildHistories(request.value(), response))
    {
        return EncodeReply(session, gacha_histories_failed_ack);
    }

    return EncodeReply(session, gacha_histories_succeed_ack, &response);
}

std::string gacha_guarantee_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, gacha_guarantee_reward_receive_failed_ack);
    }

    proto::UI32 request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, gacha_guarantee_reward_receive_failed_ack);
    }

    proto::ChangeInfo response;
    if (!session->GetPlayer()->Gachas().ReceiveGuarantee(request.value(), response))
    {
        return EncodeReply(session, gacha_guarantee_reward_receive_failed_ack);
    }

    session->GetPlayer()->Inventory().PushItemsChange(response);
    session->SavePlayer();
    return EncodeReply(session, gacha_guarantee_reward_receive_succeed_ack, &response);
}

std::string gacha_newbie_spin_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, gacha_newbie_spin_failed_ack);
    }

    proto::GachaSpinReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, gacha_newbie_spin_failed_ack);
    }

    if (!ResolveGachaAmount(request.mode()))
    {
        return EncodeReply(session, gacha_newbie_spin_failed_ack);
    }

    proto::GachaNewbieSpinResp response;
    if (!session->GetPlayer()->Gachas().SpinNewbie(request.id(), response))
    {
        return EncodeReply(session, gacha_newbie_spin_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, gacha_newbie_spin_succeed_ack, &response);
}

std::string gacha_newbie_save_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, gacha_newbie_save_failed_ack);
    }

    proto::GachaNewbieSaveReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, gacha_newbie_save_failed_ack);
    }

    std::optional<uint32_t> index;
    if (HasProtoField(req, 2))
    {
        index = request.idx();
    }

    if (!session->GetPlayer()->Gachas().SaveNewbie(request.id(), index))
    {
        return EncodeReply(session, gacha_newbie_save_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, gacha_newbie_save_succeed_ack);
}

std::string gacha_newbie_obtain_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, gacha_newbie_obtain_failed_ack);
    }

    proto::GachaNewbieObtainReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, gacha_newbie_obtain_failed_ack);
    }

    proto::ChangeInfo response;
    if (!session->GetPlayer()->Gachas().ObtainNewbie(request.id(), request.idx(), response))
    {
        return EncodeReply(session, gacha_newbie_obtain_failed_ack);
    }

    session->GetPlayer()->Inventory().PushItemsChange(response);
    session->SavePlayer();
    return EncodeReply(session, gacha_newbie_obtain_succeed_ack, &response);
}

std::string gacha_newbie_info_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, gacha_newbie_info_failed_ack);
    }

    auto response = session->GetPlayer()->Gachas().BuildNewbieInfo();
    session->SavePlayer();
    return EncodeReply(session, gacha_newbie_info_succeed_ack, &response);
}
