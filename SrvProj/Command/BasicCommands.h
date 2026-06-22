#pragma once

#include "CommandMgr.h"

namespace BasicCommands
{
    CommandResult Help(const CommandArgs& args);
    CommandResult Level(const CommandArgs& args);
    CommandResult Give(const CommandArgs& args);
    CommandResult GiveAll(const CommandArgs& args);
    CommandResult Kick(const CommandArgs& args);
    CommandResult Permission(const CommandArgs& args);
}
