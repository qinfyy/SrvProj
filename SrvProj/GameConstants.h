#pragma once

#include <cstdint>

namespace GameConstants
{
    inline constexpr std::uint32_t GoldItemId = 1;
    inline constexpr std::uint32_t StellaniteDustItemId = 2;
    inline constexpr std::uint32_t GemItemId = StellaniteDustItemId;
    inline constexpr std::uint32_t PaidStellaniteLuminaItemId = 3;
    inline constexpr std::uint32_t FreeStellaniteLuminaItemId = 4;
    inline constexpr std::uint32_t EnergyBuyItemId = StellaniteDustItemId;
    inline constexpr std::uint32_t WeeklyEntryItemId = 28;
    inline constexpr std::uint32_t JointDrillTicketId = 36;
    inline constexpr std::uint32_t StarTowerCoinItemId = 11;
    inline constexpr std::uint32_t StarTowerEventIds[] = { 101, 102, 104, 105, 106, 107, 108, 114, 115, 116, 126, 127, 128 };

    inline constexpr std::int32_t MaxEnergy = 240;
    inline constexpr std::int64_t EnergyRegenTime = 360;
    inline constexpr std::uint32_t MaxBuilds = 100;
    inline constexpr std::uint32_t MaxPresets = 50;
    inline constexpr std::uint32_t MaxShowcaseIds = 5;
    inline constexpr std::uint32_t BattlePassId = 9;
    inline constexpr std::uint32_t BattlePassUnlockLevel = 3;

    inline constexpr std::int32_t GachaProbabilityBase = 10000;
    inline constexpr std::int32_t GachaDefaultATypeProb = 200;
    inline constexpr std::int32_t GachaDefaultBGuaranteeTimes = 10;
    inline constexpr std::uint32_t GachaTravelPermitItemId = 23;
    inline constexpr std::uint32_t GachaExpertPermitItemId = 24;
    inline constexpr std::int32_t GachaExpertPermitPerDuplicateCharacter = 40;
    inline constexpr std::int32_t GachaTravelPermitPerDisc = 100;

    inline constexpr std::int32_t RefreshTypeDaily = 1;
    inline constexpr std::int32_t RefreshTypeWeekly = 2;
    inline constexpr std::int32_t RefreshTypeMonthly = 3;
    inline constexpr std::int32_t CurrencyTypeCash = 1;
    inline constexpr std::int32_t CurrencyTypeItem = 2;
    inline constexpr std::int32_t CurrencyTypeFree = 3;
    inline constexpr std::int32_t TagSkin = 2;
    inline constexpr std::int32_t UnlimitedStock = 2147483647;
    inline constexpr std::int32_t MonthlyCardDurationDays = 30;
    inline constexpr std::uint32_t DefaultHonorId = 111001;
    inline constexpr std::uint32_t AffinityHonorUnlockLevel = 10;
    inline constexpr std::uint32_t IntroGuideId = 1;
    inline constexpr std::uint32_t TraceHuntRequestItemId = 38;
    inline constexpr std::uint32_t TraceHuntPermitItemId = 39;
}
