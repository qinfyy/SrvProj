#pragma once

#include <cstdint>

namespace GameConstants
{
    inline constexpr std::uint32_t GoldItemId = 1;
    inline constexpr std::uint32_t GemItemId = 2;
    inline constexpr std::uint32_t EnergyBuyItemId = GemItemId;
    inline constexpr std::uint32_t WeeklyEntryItemId = 28;
    inline constexpr std::uint32_t JointDrillTicketId = 36;

    inline constexpr std::int32_t MaxEnergy = 240;
    inline constexpr std::int64_t EnergyRegenTime = 360;
}
