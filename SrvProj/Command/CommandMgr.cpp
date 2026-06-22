#include "CommandMgr.h"
#include "BasicCommands.h"

#include "../Game/Player.h"
#include "../GameServices.h"
#include "../Logger.h"
#include "../Util.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <thread>
#include <utility>

namespace {
bool IsSignedNumberText(const std::string& text)
{
    if (text.empty())
    {
        return false;
    }

    size_t start = 0;
    if (text[0] == '-' || text[0] == '+')
    {
        start = 1;
    }

    if (start >= text.size())
    {
        return false;
    }

    for (size_t i = start; i < text.size(); ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(text[i])))
        {
            return false;
        }
    }

    return true;
}

}

CommandMgr& CommandMgr::Instance()
{
    static CommandMgr instance;
    return instance;
}

bool CommandMgr::HasCommandPrefix(const std::string& input)
{
    if (input.empty())
    {
        return false;
    }

    return input[0] == '!' || input[0] == '/' || input[0] == '\\';
}

std::vector<std::string> CommandMgr::Split(const std::string& input)
{
    std::vector<std::string> out;
    std::istringstream stream(input);
    std::string token;
    while (stream >> token)
    {
        out.push_back(token);
    }
    return out;
}

std::string CommandMgr::ToLower(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return text;
}

int64_t CommandMgr::ParseInt64(const std::string& text, int64_t fallback)
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

CommandArgs CommandMgr::ParseArgs(Player* sender, std::vector<std::string> args, bool requireTargetOnline) const
{
    CommandArgs out;
    out.sender = sender;
    out.amount = 0;

    if (out.sender) {
        out.target = sender;
        out.targetUid = out.target->GetUid();
    }

    for (auto it = args.begin(); it != args.end();)
    {
        std::string arg = ToLower(*it);
        bool consumed = false;

        if (arg.size() > 1 && (arg[0] == '@' || (arg.size() > 2 && arg.rfind("at", 0) == 0 && IsSignedNumberText(arg.substr(2)))))
        {
            const std::string uidText = arg[0] == '@' ? arg.substr(1) : arg.substr(2);
            const int64_t uid = ParseInt64(uidText, 0);
            if (uid > 0)
            {
                out.targetUid = static_cast<uint32_t>(uid);
                out.target = GameServices::Instance().GetPlayerByUid(out.targetUid);
            }
            consumed = true;
        }
        else if (arg.size() > 1 && arg[0] == 'x' && IsSignedNumberText(arg.substr(1)))
        {
            out.amount = ParseInt64(arg.substr(1), 0);
            consumed = true;
        }
        else if (arg.rfind("lvl", 0) == 0 && arg.size() > 3 && IsSignedNumberText(arg.substr(3)))
        {
            out.level = static_cast<int>(ParseInt64(arg.substr(3), -1));
            consumed = true;
        }
        else if (arg.rfind("lv", 0) == 0 && arg.size() > 2 && IsSignedNumberText(arg.substr(2)))
        {
            out.level = static_cast<int>(ParseInt64(arg.substr(2), -1));
            consumed = true;
        }
        else if (arg.size() > 1 && arg[0] == 'a' && IsSignedNumberText(arg.substr(1)))
        {
            out.advance = static_cast<int>(ParseInt64(arg.substr(1), -1));
            consumed = true;
        }
        else if (arg.size() > 1 && (arg[0] == 't' || arg[0] == 'c') && IsSignedNumberText(arg.substr(1)))
        {
            out.talent = static_cast<int>(ParseInt64(arg.substr(1), -1));
            consumed = true;
        }
        else if (arg.size() > 1 && arg[0] == 's' && IsSignedNumberText(arg.substr(1)))
        {
            out.skill = static_cast<int>(ParseInt64(arg.substr(1), -1));
            consumed = true;
        }
        else if (arg.size() > 1 && arg[0] == 'f' && IsSignedNumberText(arg.substr(1)))
        {
            out.affinity = static_cast<int>(ParseInt64(arg.substr(1), -1));
            consumed = true;
        }

        if (consumed)
        {
            it = args.erase(it);
        }
        else
        {
            ++it;
        }
    }

    out.list = std::move(args);
    return out;
}

const std::vector<CommandRegEntry>& CommandMgr::GetCommandRegistry() const
{
    static const std::vector<CommandRegEntry> registry = []() {
        std::vector<CommandRegEntry> out;
        out.push_back(CommandRegEntry{
            "help",
            U8("/help - 显示当前可用命令列表。"),
            {"h", "?"},
            "player.help",
            false,
            true,
            BasicCommands::Help
        });

        out.push_back(CommandRegEntry{
            "level",
            U8("/level <等级> 或 /level @uid <等级> - 设置目标玩家世界等级，别名：setlevel、l。"),
            {"setlevel", "l"},
            "player.level",
            true,
            true,
            BasicCommands::Level
        });

        out.push_back(CommandRegEntry{
            "give",
            U8("/give <物品ID> x数量 或 /give @uid <物品ID> x数量 - 通过邮件发放物品，别名：g、item。"),
            {"g", "item"},
            "player.give",
            true,
            true,
            BasicCommands::Give
        });

        out.push_back(CommandRegEntry{
            "giveall",
            U8("/giveall <materials|characters|discs|skins> [lv等级] [a突破] [t天赋] [s技能] [f好感] - 批量发放或补齐资源，别名：ga。"),
            {"ga"},
            "player.give",
            true,
            true,
            BasicCommands::GiveAll
        });

        out.push_back(CommandRegEntry{
            "kick",
            U8("/kick @uid - 踢出指定在线玩家。"),
            {},
            "admin.kick",
            true,
            true,
            BasicCommands::Kick
        });

        out.push_back(CommandRegEntry{
            "permission",
            U8("/permission <add|remove|clear|display> @uid [权限] - 管理或查看指定玩家的命令权限。"),
            {"perm"},
            "admin.permission",
            true,
            false,
            BasicCommands::Permission
        });

        return out;
    }();
    return registry;
}

bool CommandMgr::CheckPermission(Player* sender, const CommandRegEntry& command) const
{
    if (!sender || command.permission.empty())
    {
        return true;
    }

    return sender->HasPermission(command.permission);
}

bool CommandMgr::CheckTargetPermission(Player* sender, const CommandRegEntry& command) const
{
    if (!sender || command.permission.empty())
    {
        return true;
    }

    return sender->HasPermission("target." + command.permission);
}

const CommandRegEntry* CommandMgr::FindCommand(const std::string& label) const
{
    const auto& registry = GetCommandRegistry();

    for (const auto& entry : registry)
    {
        if (entry.label == label)
        {
            return &entry;
        }

        if (std::find(entry.aliases.begin(), entry.aliases.end(), label) != entry.aliases.end())
        {
            return &entry;
        }
    }

    return nullptr;
}

CommandResult CommandMgr::Invoke(Player* sender, const std::string& input)
{
    auto tokens = Split(input);
    if (tokens.empty())
    {
        return {false, false, U8("请输入命令")};
    }

    std::string label = ToLower(tokens.front());
    tokens.erase(tokens.begin());
    if (!label.empty() && (label[0] == '!' || label[0] == '/' || label[0] == '\\'))
    {
        label.erase(label.begin());
    }

    if (label.empty())
    {
        return {false, false, U8("无效命令")};
    }

    const auto* command = FindCommand(label);
    if (!command || !command->commandFunction)
    {
        return {false, false, U8("无效命令")};
    }

    const auto args = ParseArgs(sender, std::move(tokens), command->requireTargetOnline);

    if (!CheckPermission(sender, *command))
    {
        return {true, false, U8("你没有权限使用这个命令")};
    }

    if (command->requireTarget)
    {
        if (command->requireTargetOnline)
        {
            if (!args.target)
            {
                return {true, false, U8("目标玩家不存在或不在线")};
            }
        }
        else if (args.targetUid == 0)
        {
            return {true, false, U8("必须指定目标玩家")};
        }
    }

    if (sender != args.target && !CheckTargetPermission(sender, *command))
    {
        return {true, false, U8("你没有权限对其他玩家使用这个命令")};
    }

    return command->commandFunction(args);
}

void CommandMgr::StartConsoleThread()
{
    std::thread([]() {
        std::string line;
        while (true)
        {
            std::cout << ">" << std::flush;
            if (!std::getline(std::cin, line))
            {
                break;
            }

            if (line.empty())
            {
                continue;
            }

            auto result = CommandMgr::Instance().Invoke(nullptr, line);
			LOG_INFO("命令结果: {}", Utf8ToAnsi(result.message)); // result.message 为 UTF-8 编码，转换为控制台编码输出
        }
    }).detach();
}
