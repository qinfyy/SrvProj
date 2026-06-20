#pragma once

#include <cstdint>
#include <string>

namespace GameTime
{
    int64_t NowSeconds();
    int64_t NowMilliseconds();
    int64_t ServerNowSeconds();
    int64_t ServerNowMilliseconds();
    int32_t GetUtcOffsetSeconds();
    int32_t GetConfiguredUtcOffsetSeconds();
    uint32_t CurrentEpochDay();
    int64_t ResetTimeSecondsByEpochDay(uint32_t epochDay, int hour = 0);
    int64_t NextDailyReset(int hour = 0);
    int64_t NextWeeklyReset(int weekDay = 1, int hour = 0);
    int64_t NextMonthlyReset(int hour = 0);
    int64_t DateToSecondsInConfiguredTimeZone(const std::string& text);
}
