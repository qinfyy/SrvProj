#pragma once

#include "CommandMgr.h"

namespace BasicCommands
{
    CommandResult Help(const CommandArgs& args);
    CommandResult Level(const CommandArgs& args);
    CommandResult Give(const CommandArgs& args);
    CommandResult GiveAll(const CommandArgs& args);
}
