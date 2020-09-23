#include "PCH.h"
#include "DateTime.h"

#define _CRT_NO_TIME_T
#include <time.h>
#include <sys/timeb.h>

namespace XE
{
    //static_assert(sizeof(FDateTime) == sizeof(int) * 3);
    static int64_t _curr_timezone()
    {
        long timezone = 0;
        _get_timezone(&timezone);
        return static_cast<int64_t>(timezone);
    }

    FDateTime::FDateTime(uint64_t Nanoseconds_)
    {
        __time64_t Seconds = Nanoseconds_ / 1'000'000'000;
        tm tmtime;
        _gmtime64_s(&tmtime, &Seconds);
        Year = tmtime.tm_year + 1900;
        Month = tmtime.tm_mon + 1;
        Day = tmtime.tm_mday;
        Hour = tmtime.tm_hour;
        Minute = tmtime.tm_min;
        Second = tmtime.tm_sec;
        Millisecond = (Nanoseconds_ / 1'000'000) % 1'000;
        Microsecond = (Nanoseconds_ / 1'000) % 1'000;
        Nanosecond = (Nanoseconds_ / 1) % 1'000;
    }
     
    FDateTime::FDateTime(uint32_t Year_, uint32_t Month_, uint32_t Day_, uint32_t Hour_, uint32_t Minute_, uint32_t Second_, uint32_t Millisecond_, uint32_t Microsecond_, uint32_t Nanosecond_) :
        Year(Year_), Month(Month_), Day(Day_), Hour(Hour_), Minute(Minute_), Second(Second_), Millisecond(Millisecond_), Microsecond(Microsecond_), Nanosecond(Nanosecond_)
    {
    }

    uint64_t FDateTime::GetNanoseconds() const
    {
        tm tmtime = {};
        tmtime.tm_year = Year - 1900;
        tmtime.tm_mon = Month - 1;
        tmtime.tm_mday = Day;
        tmtime.tm_hour = Hour;
        tmtime.tm_min = Minute;
        tmtime.tm_sec = Second;
        int64_t Seconds = _mkgmtime64(&tmtime);
        return Seconds * 1'000'000'000 +
            Millisecond * 1'000'000 +
            Microsecond * 1'000 +
            Nanosecond;
    }
	
    FStringView FDateTime::Format(FStringView Formal) const
    {
        size_t Count = 0;
        XE::Char ChLast = 0;

        FString & String = XE::GetThreadLocalStringBuffer<XE::Char>();
        auto FunFlushInner = [&]()
        {
            if (Count == 1)
                return XE::Format(Str("{}"), ChLast);

            switch (ChLast)
            {
            case 'y':
                if (Count == 2)
                    return XE::Format(Str("{:02}"), Year);
                if (Count == 4)
                    return XE::Format(Str("{:04}"), Year);
                break;
            case 'M':
                if (Count == 2)
                    return XE::Format(Str("{:02}"), Month);
                break;
            case 'd':
                if (Count == 2)
                    return XE::Format(Str("{:02}"), Day);
                break;
            case 'h':
                if (Count == 2)
                    return XE::Format(Str("{:02}"), Hour % 12);
                break;
            case 'H':
                if (Count == 2)
                    return XE::Format(Str("{:02}"), Hour);
                break;
            case 'm':
                if (Count == 2)
                    return XE::Format(Str("{:02}"), Minute);
                break;
            case 's':
                if (Count == 2)
                    return XE::Format(Str("{:02}"), Second);
                break;
            case 'f':
                if (Count == 1)
                    return XE::Format(Str("{:1}"), Millisecond / 100);
                if (Count == 2)
                    return XE::Format(Str("{:02}"), Millisecond / 10);
                if (Count == 3)
                    return XE::Format(Str("{:03}"), Millisecond);
                break;
            default:
                break;
            }
            return XE::FStringView::Empty;
        };

        auto FunFlush = [&]()
        {
            String.Append(FunFlushInner());
        };

        for (size_t Index = 0; Index < Formal.Size; ++Index)
        {
            XE::Char Ch = Formal[Index];
            if (!ChLast)
                ChLast = Ch;

            if (Ch != ChLast)
            {
                FunFlush();
                ChLast = Ch;
                Count = 1;
            }
            else if (Count == 3)
            {
                ++Count;
                FunFlush();
                ChLast = 0;
                Count = 0;
            }
            else
                ++Count;
        }

        if (Count > 0)
            FunFlush();
        return String;
    }

    bool FDateTime::IsLeapyear(uint32_t Year)
    {
        return (Year % 4 == 0 && Year % 100 != 0) || (Year % 400 == 0);
    }

    EDayWeek FDateTime::DayWeek(uint32_t Year, uint32_t Month, uint32_t Day)
    {
        if (Month < 1 || 12 < Month)
            return EDayWeek::Monday;

        static uint32_t DAY[] = { 6, 2, 1, 4, 6, 2, 4, 0, 3, 5, 1, 3 };
        Year -= Month < 3;
        return static_cast<EDayWeek>((Year + Year / 4 - Year / 100 + Year / 400 + DAY[Month - 1] + Day) % 7);
    }

    uint32_t FDateTime::DayYear(uint32_t Year, uint32_t Month, uint32_t Day)
    {
        if (Month < 1 || 12 < Month)
            return 0;

        static uint32_t DAY[] = { 1, 0, 2, 0, 3, 0, 4, 5, 0, 6, 0, 7 };
        uint32_t Temp = (Month - 1) * 30 + DAY[Month - 1] + Day;
        if (Month > 2)
            Temp -= IsLeapyear(Year) ? 1 : 2;
        return Temp;
    }

    FDateTime FDateTime::Now()
    {
        return FDateTime((uint64_t)((int64_t)SystemNanoseconds() + _curr_timezone() * 1'000'000'000));
    }

    float64_t FDateTime::System()
    {
        return SystemNanoseconds() / 1'000'000.0;
    }

    uint64_t FDateTime::SystemMilliseconds()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    uint64_t FDateTime::SystemMicroseconds()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    uint64_t FDateTime::SystemNanoseconds()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    float64_t FDateTime::Steady()
    {
        return SteadyNanoseconds() / 1'000'000.0;
    }

    uint64_t FDateTime::SteadyMilliseconds()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    uint64_t FDateTime::SteadyMicroseconds()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    uint64_t FDateTime::SteadyNanoseconds()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    float64_t FDateTime::HighResolution()
    {
        return HighResolutionNanoseconds() / 1'000'000.0;
    }

    uint64_t FDateTime::HighResolutionMilliseconds()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
	
    uint64_t FDateTime::HighResolutionMicroseconds()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
	
    uint64_t FDateTime::HighResolutionNanoseconds()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
}
