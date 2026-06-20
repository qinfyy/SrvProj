#include "GameTime.h"

#include "Config.h"
#include "Logger.h"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdint>
#include <mutex>
#include <string>

#include <Windows.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace std::chrono;

namespace
{
    struct DateTimeParts
    {
        int Year = 0;
        int Month = 0;
        int Day = 0;
        int Hour = 0;
        int Minute = 0;
        int Second = 0;
    };

    struct TimeSettingsCache
    {
        std::string TimeZoneKey;
        std::string SpoofDate;
        bool SpoofTime = false;
        int32_t UtcOffsetSeconds = 0;
        int64_t ServerOffsetSeconds = 0;
        bool Valid = false;
    };

    std::mutex gTimeSettingsMutex;
    TimeSettingsCache gTimeSettingsCache;

    std::string TrimAscii(std::string value)
    {
        auto isSpace = [](unsigned char c) {
            return std::isspace(c) != 0;
        };

        const auto begin = std::find_if_not(value.begin(), value.end(), isSpace);
        const auto end = std::find_if_not(value.rbegin(), value.rend(), isSpace).base();
        if (begin >= end)
        {
            return {};
        }

        return std::string(begin, end);
    }

    std::string ToUpperAscii(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
            return static_cast<char>(std::toupper(c));
        });
        return value;
    }

    bool ParseIntSegment(const std::string& text, size_t start, size_t length, int& out)
    {
        if (length == 0 || start + length > text.size())
        {
            return false;
        }

        int value = 0;
        for (size_t i = 0; i < length; ++i)
        {
            const unsigned char ch = static_cast<unsigned char>(text[start + i]);
            if (!std::isdigit(ch))
            {
                return false;
            }
            value = (value * 10) + static_cast<int>(ch - '0');
        }

        out = value;
        return true;
    }

    bool ParseDateTimeParts(const std::string& text, DateTimeParts& out, bool allowT = true)
    {
        const std::string trimmed = TrimAscii(text);
        if (trimmed.size() < 19)
        {
            return false;
        }

        if (trimmed[4] != '-' || trimmed[7] != '-' || (trimmed[10] != ' ' && (!allowT || trimmed[10] != 'T')) ||
            trimmed[13] != ':' || trimmed[16] != ':')
        {
            return false;
        }

        if (!ParseIntSegment(trimmed, 0, 4, out.Year) ||
            !ParseIntSegment(trimmed, 5, 2, out.Month) ||
            !ParseIntSegment(trimmed, 8, 2, out.Day) ||
            !ParseIntSegment(trimmed, 11, 2, out.Hour) ||
            !ParseIntSegment(trimmed, 14, 2, out.Minute) ||
            !ParseIntSegment(trimmed, 17, 2, out.Second))
        {
            return false;
        }

        if (out.Month < 1 || out.Month > 12 ||
            out.Day < 1 || out.Day > 31 ||
            out.Hour < 0 || out.Hour > 23 ||
            out.Minute < 0 || out.Minute > 59 ||
            out.Second < 0 || out.Second > 59)
        {
            return false;
        }

        const year_month_day ymd{
            year{out.Year},
            month{static_cast<unsigned>(out.Month)},
            day{static_cast<unsigned>(out.Day)}
        };
        return ymd.ok();
    }

    bool ParseOffsetText(const std::string& text, int32_t& offsetSeconds)
    {
        const std::string trimmed = TrimAscii(text);
        if (trimmed.empty())
        {
            return false;
        }

        if (trimmed == "Z" || trimmed == "z")
        {
            offsetSeconds = 0;
            return true;
        }

        const char sign = trimmed[0];
        if (sign != '+' && sign != '-')
        {
            return false;
        }

        std::string body = TrimAscii(trimmed.substr(1));
        if (body.empty())
        {
            return false;
        }

        int hours = 0;
        int minutes = 0;
        const size_t colon = body.find(':');
        if (colon != std::string::npos)
        {
            if (!ParseIntSegment(body, 0, colon, hours) ||
                !ParseIntSegment(body, colon + 1, body.size() - colon - 1, minutes))
            {
                return false;
            }
        }
        else if (body.size() <= 2)
        {
            if (!ParseIntSegment(body, 0, body.size(), hours))
            {
                return false;
            }
        }
        else if (body.size() == 4)
        {
            if (!ParseIntSegment(body, 0, 2, hours) ||
                !ParseIntSegment(body, 2, 2, minutes))
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        if (hours < 0 || hours > 14 || minutes < 0 || minutes > 59)
        {
            return false;
        }

        int32_t total = static_cast<int32_t>((hours * 3600) + (minutes * 60));
        if (sign == '-')
        {
            total = -total;
        }

        offsetSeconds = total;
        return true;
    }

    sys_seconds MakeLocalDateTimeSeconds(const DateTimeParts& parts)
    {
        const year_month_day ymd{
            year{parts.Year},
            month{static_cast<unsigned>(parts.Month)},
            day{static_cast<unsigned>(parts.Day)}
        };

        const sys_days dayPoint{ymd};
        const auto local = dayPoint
            + hours{parts.Hour}
            + minutes{parts.Minute}
            + seconds{parts.Second};
        return time_point_cast<seconds>(local);
    }

    sys_seconds MakeUtcSecondsFromLocalDateTime(const DateTimeParts& parts, int32_t utcOffsetSeconds)
    {
        return MakeLocalDateTimeSeconds(parts) - seconds{utcOffsetSeconds};
    }

    bool TryGetSystemUtcOffsetSeconds(int32_t& offsetSeconds)
    {
        TIME_ZONE_INFORMATION info{};
        const DWORD result = GetTimeZoneInformation(&info);
        if (result == TIME_ZONE_ID_INVALID)
        {
            return false;
        }

        offsetSeconds = static_cast<int32_t>(-info.Bias * 60);
        return true;
    }

    TimeSettingsCache BuildTimeSettings()
    {
        const Config& config = Config::Get();
        const std::string timeZoneKey = ToUpperAscii(TrimAscii(config.TimeZone));
        const std::string spoofDate = TrimAscii(config.serverTime.spoofDate);
        const bool spoofTime = config.serverTime.spoofTime;

        TimeSettingsCache next;
        next.TimeZoneKey = timeZoneKey;
        next.SpoofDate = spoofDate;
        next.SpoofTime = spoofTime;

        if (timeZoneKey == "AUTO")
        {
            if (!TryGetSystemUtcOffsetSeconds(next.UtcOffsetSeconds))
            {
                LOG_ERROR("自动时区获取失败，已回退到 UTC");
                next.UtcOffsetSeconds = 0;
            }
        }
        else if (timeZoneKey == "UTC")
        {
            next.UtcOffsetSeconds = 0;
        }
        else if (timeZoneKey.rfind("UTC", 0) == 0)
        {
            if (!ParseOffsetText(timeZoneKey.substr(3), next.UtcOffsetSeconds))
            {
                LOG_ERROR("时区配置无效: {}, 已回退到 UTC", config.TimeZone);
                next.UtcOffsetSeconds = 0;
            }
        }
        else
        {
            LOG_ERROR("时区配置无效: {}, 已回退到 UTC", config.TimeZone);
            next.UtcOffsetSeconds = 0;
        }

        if (next.SpoofTime)
        {
            DateTimeParts parts{};
            if (!ParseDateTimeParts(next.SpoofDate, parts, false))
            {
                LOG_ERROR("伪服务器时间格式无效: {}, 期望格式 yyyy-mm-dd HH:MM:SS", config.serverTime.spoofDate);
                next.SpoofTime = false;
                next.ServerOffsetSeconds = 0;
            }
            else
            {
                const int64_t spoofEpoch = MakeUtcSecondsFromLocalDateTime(parts, next.UtcOffsetSeconds).time_since_epoch().count();
                const int64_t nowEpoch = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
                next.ServerOffsetSeconds = spoofEpoch - nowEpoch;
            }
        }

        next.Valid = true;
        return next;
    }

    TimeSettingsCache SnapshotTimeSettings()
    {
        std::lock_guard<std::mutex> lock(gTimeSettingsMutex);
        const Config& config = Config::Get();
        const std::string timeZoneKey = ToUpperAscii(TrimAscii(config.TimeZone));
        const std::string spoofDate = TrimAscii(config.serverTime.spoofDate);
        const bool spoofTime = config.serverTime.spoofTime;

        if (!gTimeSettingsCache.Valid ||
            gTimeSettingsCache.TimeZoneKey != timeZoneKey ||
            gTimeSettingsCache.SpoofDate != spoofDate ||
            gTimeSettingsCache.SpoofTime != spoofTime)
        {
            gTimeSettingsCache = BuildTimeSettings();
        }

        return gTimeSettingsCache;
    }

    std::chrono::sys_seconds NowSecondsPoint()
    {
        return time_point_cast<seconds>(std::chrono::system_clock::now());
    }
}

namespace GameTime
{
    int64_t NowSeconds()
    {
        return NowSecondsPoint().time_since_epoch().count();
    }

    int64_t NowMilliseconds()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    int64_t ServerNowSeconds()
    {
        const TimeSettingsCache view = SnapshotTimeSettings();
        return NowSeconds() + view.ServerOffsetSeconds;
    }

    int64_t ServerNowMilliseconds()
    {
        const TimeSettingsCache view = SnapshotTimeSettings();
        return NowMilliseconds() + (view.ServerOffsetSeconds * 1000LL);
    }

    int32_t GetUtcOffsetSeconds()
    {
        return SnapshotTimeSettings().UtcOffsetSeconds;
    }

    int32_t GetConfiguredUtcOffsetSeconds()
    {
        return GetUtcOffsetSeconds();
    }

    uint32_t CurrentEpochDay()
    {
        const TimeSettingsCache view = SnapshotTimeSettings();
        const sys_seconds localTime = NowSecondsPoint() + seconds{view.ServerOffsetSeconds + view.UtcOffsetSeconds};
        const sys_days localDay = floor<days>(localTime);
        const auto dayCount = localDay.time_since_epoch().count();
        return dayCount < 0 ? 0u : static_cast<uint32_t>(dayCount);
    }

    int64_t ResetTimeSecondsByEpochDay(uint32_t epochDay, int hour)
    {
        const int safeHour = (std::clamp)(hour, 0, 23);
        const TimeSettingsCache view = SnapshotTimeSettings();
        const sys_days localDay{days{static_cast<int64_t>(epochDay)}};
        const auto localReset = localDay + hours{safeHour};
        const auto utcReset = time_point_cast<seconds>(localReset) - seconds{view.UtcOffsetSeconds};
        return utcReset.time_since_epoch().count();
    }

    int64_t NextDailyReset(int hour)
    {
        const int safeHour = (std::clamp)(hour, 0, 23);
        const TimeSettingsCache view = SnapshotTimeSettings();
        const sys_seconds localNow = NowSecondsPoint() + seconds{view.ServerOffsetSeconds + view.UtcOffsetSeconds};
        const sys_days nextDay = floor<days>(localNow - hours{safeHour}) + days{1};
        const auto nextLocal = nextDay + hours{safeHour};
        const auto nextUtc = time_point_cast<seconds>(nextLocal) - seconds{view.UtcOffsetSeconds};
        return nextUtc.time_since_epoch().count();
    }

    int64_t NextWeeklyReset(int weekDay, int hour)
    {
        const int safeWeekDay = (std::clamp)(weekDay, 1, 7);
        const int safeHour = (std::clamp)(hour, 0, 23);
        const TimeSettingsCache view = SnapshotTimeSettings();
        const sys_seconds localNow = NowSecondsPoint() + seconds{view.ServerOffsetSeconds + view.UtcOffsetSeconds};
        const sys_days shiftedDay = floor<days>(localNow - hours{safeHour});
        const weekday currentWeekDay{shiftedDay};
        const int currentIsoWeekDay = currentWeekDay.iso_encoding();

        int daysUntil = safeWeekDay - currentIsoWeekDay;
        if (daysUntil <= 0)
        {
            daysUntil += 7;
        }

        const sys_days nextDay = shiftedDay + days{daysUntil};
        const auto nextLocal = nextDay + hours{safeHour};
        const auto nextUtc = time_point_cast<seconds>(nextLocal) - seconds{view.UtcOffsetSeconds};
        return nextUtc.time_since_epoch().count();
    }

    int64_t NextMonthlyReset(int hour)
    {
        const int safeHour = (std::clamp)(hour, 0, 23);
        const TimeSettingsCache view = SnapshotTimeSettings();
        const sys_seconds localNow = NowSecondsPoint() + seconds{view.ServerOffsetSeconds + view.UtcOffsetSeconds};
        const sys_days shiftedDay = floor<days>(localNow - hours{safeHour});
        const year_month_day currentDate{shiftedDay};
        int nextYearValue = static_cast<int>(currentDate.year());
        int nextMonthValue = static_cast<unsigned>(currentDate.month()) + 1;
        if (nextMonthValue > 12)
        {
            nextMonthValue = 1;
            ++nextYearValue;
        }

        const year_month_day nextDate{year{nextYearValue}, month{static_cast<unsigned>(nextMonthValue)}, day{1}};
        const sys_days nextDay{nextDate};
        const auto nextLocal = nextDay + hours{safeHour};
        const auto nextUtc = time_point_cast<seconds>(nextLocal) - seconds{view.UtcOffsetSeconds};
        return nextUtc.time_since_epoch().count();
    }

    int64_t DateToSecondsInConfiguredTimeZone(const std::string& text)
    {
        const std::string trimmed = TrimAscii(text);
        if (trimmed.empty())
        {
            return 0;
        }

        DateTimeParts parts{};
        if (!ParseDateTimeParts(trimmed, parts))
        {
            return 0;
        }

        std::string suffix;
        if (trimmed.size() > 19)
        {
            suffix = TrimAscii(trimmed.substr(19));
            if (!suffix.empty())
            {
                int32_t explicitOffset = 0;
                if (!ParseOffsetText(suffix, explicitOffset))
                {
                    return 0;
                }

                return MakeUtcSecondsFromLocalDateTime(parts, explicitOffset).time_since_epoch().count();
            }
        }

        return MakeUtcSecondsFromLocalDateTime(parts, GetUtcOffsetSeconds()).time_since_epoch().count();
    }
}
