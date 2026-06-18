#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

class Player;

struct CommandResult
{
    bool handled = false;
    bool success = false;
    std::string message;
};

struct CommandArgs
{
    Player* sender = nullptr;
    Player* target = nullptr;
    uint32_t targetUid = 0;
    int64_t amount = 0;
    int level = -1;
    int advance = -1;
    int talent = -1;
    int skill = -1;
    int affinity = -1;
    std::vector<std::string> list;
};

struct CommandRegEntry
{
    std::string label;
    std::string description;
    std::vector<std::string> aliases;
    std::string permission;
    bool requireTarget = false;
    std::function<CommandResult(const CommandArgs&)> commandFunction;
};

class CommandRegEntryHashByLabel
{
public:
    std::size_t operator()(const CommandRegEntry& entry) const noexcept
    {
        return std::hash<std::string>{}(entry.label);
    }
};

class CommandRegEntryEqualByLabel
{
public:
    bool operator()(const CommandRegEntry& a, const CommandRegEntry& b) const noexcept
    {
        return a.label == b.label;
    }
};

using CommandRegistry = std::unordered_set<CommandRegEntry, CommandRegEntryHashByLabel, CommandRegEntryEqualByLabel>;

class CommandMgr
{
public:
    static CommandMgr& Instance();

    CommandResult Invoke(Player* sender, const std::string& input);
    static bool HasCommandPrefix(const std::string& input);
    static void StartConsoleThread();
    const CommandRegistry& GetCommandRegistry() const;

private:
    CommandMgr() = default;

    CommandArgs ParseArgs(Player* sender, std::vector<std::string> args) const;
    const CommandRegEntry* FindCommand(const std::string& label) const;

    static std::vector<std::string> Split(const std::string& input);
    static int64_t ParseInt64(const std::string& text, int64_t fallback = 0);
    static std::string ToLower(std::string text);
};
