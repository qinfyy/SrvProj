#include "BasicCommands.h"

#include "CommandMgr.h"
#include "../DbMgr.h"
#include "../Game/CharacterMgr.h"
#include "../Game/InventoryMgr.h"
#include "../Game/MailMgr.h"
#include "../Game/Player.h"
#include "../GameServices.h"
#include "../GameConstants.h"
#include "../GameSession.h"
#include "../Resources/BinClass/CharacterRes.h"
#include "../Resources/BinClass/DiscRes.h"
#include "../Resources/BinClass/ItemsRes.h"
#include "../Resources/BinClass/MiscRes.h"
#include "../Resources/GameData.h"
#include "../Util.h"

#include <algorithm>
#include <cctype>
#include <initializer_list>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace {
bool IsMaterialItem(const ItemRes& data)
{
    switch (data.Stype)
    {
    case 12: // 光盘经验
    case 13: // 光盘突破
    case 24: // 技能升级
    case 25: // 角色突破
    case 35: // 装备/刻印制作
        return true;
    default:
        return false;
    }
}

bool IsCommandLabel(const std::string& label, std::initializer_list<const char*> names)
{
    for (const char* name : names)
    {
        if (label == name)
        {
            return true;
        }
    }
    return false;
}

std::string ToLower(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return text;
}

int64_t ParseInt64(const std::string& text, int64_t fallback = 0)
{
    try
    {
        size_t parsed = 0;
        const int64_t value = std::stoll(text, &parsed, 10);
        return parsed == text.size() ? value : fallback;
    }
    catch (...)
    {
        return fallback;
    }
}

std::vector<std::string> SplitPermissions(const std::string& text)
{
    std::vector<std::string> permissions;
    size_t start = 0;

    while (start <= text.size())
    {
        const size_t pos = text.find(';', start);
        const std::string part = pos == std::string::npos ? text.substr(start) : text.substr(start, pos - start);
        if (!part.empty())
        {
            permissions.push_back(part);
        }

        if (pos == std::string::npos)
        {
            break;
        }

        start = pos + 1;
    }

    return permissions;
}

std::string JoinPermissions(const std::vector<std::string>& permissions)
{
    std::ostringstream out;
    for (size_t i = 0; i < permissions.size(); ++i)
    {
        if (i > 0)
        {
            out << ';';
        }
        out << permissions[i];
    }
    return out.str();
}

}

CommandResult BasicCommands::Help(const CommandArgs& args)
{
    const auto& registry = CommandMgr::Instance().GetCommandRegistry();
    std::ostringstream message;
    message << U8("当前可用命令：") << '\n';
    for (const CommandRegEntry& entry : registry)
    {
        if (!CommandMgr::Instance().CheckPermission(args.sender, entry))
        {
            continue;
        }

        message << entry.description << '\n';
    }

    message << U8("提示：客户端签名命令可以使用 !、/ 或 \\ 前缀；控制台命令可以不加前缀。以后新增命令时，只需要在命令注册表中添加说明。");
    return { true, true, message.str() };
}

CommandResult BasicCommands::Level(const CommandArgs& args)
{
    int level = args.level;
    if (level <= 0 && !args.list.empty())
    {
        level = static_cast<int>(ParseInt64(args.list.front(), 0));
    }

    if (level <= 0)
    {
        return {true, false, U8("等级参数无效")};
    }

    if (!GameData::WorldClassDataTable.empty())
    {
        int minLevel = std::numeric_limits<int>::max();
        int maxLevel = std::numeric_limits<int>::min();

        for (const auto& entry : GameData::WorldClassDataTable)
        {
            const int supportedLevel = entry.second.Id;
            minLevel = std::min(minLevel, supportedLevel);
            maxLevel = std::max(maxLevel, supportedLevel);
        }

        if (minLevel <= maxLevel)
        {
            level = std::clamp(level, minLevel, maxLevel);
        }
    }

    if (!args.target->SetWorldLevel(static_cast<uint32_t>(level)))
    {
        return {true, false, U8("游戏不支持该等级")};
    }

    if (auto* session = args.target->GetSessionRef())
    {
        session->SavePlayer();
    }

    return {true, true, std::string(U8("设置等级成功: ")) + std::to_string(level)};
}

CommandResult BasicCommands::Give(const CommandArgs& args)
{
    int64_t amount = std::max<int64_t>(args.amount, 1);
    std::vector<std::string> itemArgs = args.list;
    if (args.amount <= 0 && itemArgs.size() >= 2)
    {
        const int64_t maybeAmount = ParseInt64(itemArgs.back(), 0);
        if (maybeAmount > 0)
        {
            amount = maybeAmount;
            itemArgs.pop_back();
        }
    }

    std::vector<std::pair<uint32_t, int64_t>> attachments;
    attachments.reserve(itemArgs.size());

    for (const auto& value : itemArgs)
    {
        const int64_t itemId = ParseInt64(value, 0);
        if (itemId <= 0 || itemId > std::numeric_limits<uint32_t>::max())
        {
            continue;
        }

        const uint32_t tid = static_cast<uint32_t>(itemId);
        if (!GameData::ItemDataTable.empty() && GameData::ItemDataTable.find(std::to_string(tid)) == GameData::ItemDataTable.end())
        {
            continue;
        }

        attachments.emplace_back(tid, amount);
    }

    if (attachments.empty())
    {
        return {true, false, U8("没有可发放的物品")};
    }

    args.target->Mails().AddSystemMail("Give Command Result", "", attachments, true);
    if (auto* session = args.target->GetSessionRef())
    {
        session->SavePlayer();
    }

    return {true, true, U8("发放成功，请到邮件中领取")};
}

CommandResult BasicCommands::GiveAll(const CommandArgs& args)
{
    if (args.list.empty())
    {
        return {true, false, U8("缺少 giveall 类型")};
    }

    const std::string type = ToLower(args.list.front());
    proto::ChangeInfo change;
    int count = 0;

    if (IsCommandLabel(type, {"m", "materials", "mats"}))
    {
        for (const auto& [key, data] : GameData::ItemDataTable)
        {
            if (!IsMaterialItem(data))
            {
                continue;
            }

            if (args.target->Inventory().AddItem(static_cast<uint32_t>(data.Id), 10000, &change))
            {
                ++count;
            }
        }

        const std::vector<std::pair<uint32_t, int64_t>> fallbackItems = {
            {30001, 10000}, {30002, 10000}, {30003, 10000}, {30004, 10000},
            {GameConstants::GoldItemId, 50000000}
        };
        for (const auto& item : fallbackItems)
        {
            if (args.target->Inventory().AddItem(item.first, item.second, &change))
            {
                ++count;
            }
        }
    }
    else if (IsCommandLabel(type, {"c", "char", "characters", "trekkers", "t"}))
    {
        for (const auto& [key, data] : GameData::CharacterDataTable)
        {
            if (!data.Available || !data.Visible || args.target->Characters().HasCharacter(data.Id))
            {
                continue;
            }

            auto* character = args.target->Characters().AddCharacter(data);
            if (!character)
            {
                continue;
            }

            args.target->Characters().ApplyCharacterCommandProperties(*character, args.level, args.advance, args.talent, args.skill, args.affinity);
            args.target->Characters().AddCharacterChange(change, *character);
            ++count;
        }
    }
    else if (IsCommandLabel(type, {"d", "disc", "discs"}))
    {
        for (const auto& [key, data] : GameData::DiscDataTable)
        {
            if (!data.Available || !data.Visible || args.target->Characters().HasDisc(data.Id))
            {
                continue;
            }

            auto* disc = args.target->Characters().AddDisc(data);
            if (!disc)
            {
                continue;
            }

            args.target->Characters().ApplyDiscCommandProperties(*disc, args.level, args.advance, args.talent);
            args.target->Characters().AddDiscChange(change, *disc);
            ++count;
        }
    }
    else if (IsCommandLabel(type, {"skin", "skins"}))
    {
        for (const auto& [key, data] : GameData::ItemDataTable)
        {
            if (data.Type != 10 && data.Stype != 46)
            {
                continue;
            }

            if (args.target->Inventory().AddSkin(static_cast<uint32_t>(data.Id), &change))
            {
                ++count;
            }
        }
    }
    else
    {
        return {true, false, U8("未知 giveall 类型")};
    }

    if (count == 0 || change.props_size() <= 0)
    {
        return {true, false, U8("没有物品或角色发生变化")};
    }

    args.target->Inventory().PushItemsChange(change);
    if (auto* session = args.target->GetSessionRef())
    {
        session->SavePlayer();
    }

    return {true, true, std::string(U8("批量发放成功，变更数量: ")) + std::to_string(count)};
}

CommandResult BasicCommands::Kick(const CommandArgs& args)
{
    if (args.targetUid == 0)
    {
        return {true, false, U8("kick 命令必须使用 /kick @uid")};
    }

    if (!args.target)
    {
        return {true, false, U8("目标玩家不存在或不在线")};
    }

    if (!GameServices::Instance().KickSessionByPlayerUid(args.targetUid))
    {
        return {true, false, U8("踢出玩家失败")};
    }

    return {true, true, std::string(U8("已踢出玩家: ")) + std::to_string(args.targetUid)};
}

CommandResult BasicCommands::Permission(const CommandArgs& args)
{
    if (args.list.empty())
    {
        return {true, false, U8("用法: /permission <add|remove|clear|display> @uid [权限]")};
    }

    const std::string action = ToLower(args.list.front());
    if (args.targetUid == 0)
    {
        return {true, false, U8("permission 命令必须指定目标玩家，例如 /permission display @10001")};
    }

    Player* targetPlayer = args.target;
    std::unique_ptr<Player> offlinePlayer;

    if (!targetPlayer)
    {
        targetPlayer = GameServices::Instance().GetPlayerByUid(args.targetUid);
    }

    if (!targetPlayer)
    {
        offlinePlayer = LoadOfflinePlayer(args.targetUid);
        if (!offlinePlayer)
        {
            return {true, false, U8("目标玩家不存在")};
        }

        targetPlayer = offlinePlayer.get();
    }

    if (action == "clear")
    {
        targetPlayer->ClearPermissions();

        const bool ok = targetPlayer->GetSessionRef() ? targetPlayer->Save() : SaveOfflinePlayer(*targetPlayer);
        if (!ok)
        {
            return {true, false, U8("清除权限失败")};
        }

        return {true, true, std::string(U8("已清除玩家全部权限: ")) + std::to_string(args.targetUid)};
    }

    if (action == "display")
    {
        const auto permissions = targetPlayer->GetPermissions();
        std::ostringstream message;
        message << U8("玩家 ") << args.targetUid << U8(" 的权限: ");
        if (permissions.empty())
        {
            message << U8("（空）");
        }
        else
        {
            for (size_t i = 0; i < permissions.size(); ++i)
            {
                if (i > 0)
                {
                    message << U8(", ");
                }
                message << permissions[i];
            }
        }

        return {true, true, message.str()};
    }

    if (args.list.size() < 2)
    {
        return {true, false, U8("缺少权限参数")};
    }

    const auto permissions = SplitPermissions(args.list[1]);
    if (permissions.empty())
    {
        return {true, false, U8("权限参数无效")};
    }

    std::vector<std::string> changedPermissions;
    std::vector<std::string> failedPermissions;
    if (action == "add")
    {
        for (const auto& permission : permissions)
        {
            if (targetPlayer->AddPermission(permission))
            {
                changedPermissions.push_back(permission);
            }
            else
            {
                failedPermissions.push_back(permission);
            }
        }
    }
    else if (action == "remove")
    {
        for (const auto& permission : permissions)
        {
            if (targetPlayer->RemovePermission(permission))
            {
                changedPermissions.push_back(permission);
            }
            else
            {
                failedPermissions.push_back(permission);
            }
        }
    }
    else
    {
        return {true, false, U8("未知操作，只支持 add、remove、clear、display")};
    }

    if (changedPermissions.empty())
    {
        return {true, false, action == "add" ? U8("没有新增任何权限") : U8("没有移除任何权限")};
    }

    const bool ok = targetPlayer->GetSessionRef() ? targetPlayer->Save() : SaveOfflinePlayer(*targetPlayer);
    if (!ok)
    {
        return {true, false, U8("保存权限失败")};
    }

    if (action == "add")
    {
        std::ostringstream message;
        message << U8("已为玩家 ") << args.targetUid << U8(" 添加权限: ") << JoinPermissions(changedPermissions);
        if (!failedPermissions.empty())
        {
            message << U8("，未生效: ") << JoinPermissions(failedPermissions);
        }
        return {true, true, message.str()};
    }

    std::ostringstream message;
    message << U8("已为玩家 ") << args.targetUid << U8(" 移除权限: ") << JoinPermissions(changedPermissions);
    if (!failedPermissions.empty())
    {
        message << U8("，未生效: ") << JoinPermissions(failedPermissions);
    }
    return {true, true, message.str()};
}
