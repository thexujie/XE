#pragma once

#include "CoreInc.h"

namespace XE
{
    enum class EDayWeek
    {
        Monday = 0,
        Tuesday,
        Wednesday,
        Thursday,
        Friday,
        Saturday,
        Sunday
    };

    class CORE_API FDateTime
    {
    public:
        FDateTime() = default;
        FDateTime(const FDateTime &) = default;
        FDateTime & operator =(const FDateTime & Another) = default;
        friend bool operator == (const FDateTime &, const FDateTime &) = default;
        friend auto operator <=> (const FDateTime &, const FDateTime &) = default;
    	
        FDateTime(uint64_t Nanoseconds_);
        FDateTime(uint32_t Year_, uint32_t Month_, uint32_t Day_, uint32_t Hour_, uint32_t Minute_, uint32_t Second_, 
            uint32_t Millisecond_ = 0, uint32_t Microsecond_ = 0, uint32_t Nanosecond_ = 0);

    	// https://docs.microsoft.com/en-us/dotnet/standard/base-types/custom-date-and-time-format-strings
        FStringView Format(FStringView Formal = FStringView::Empty) const;

        uint64_t GetNanoseconds() const;

    public:
        //! Year [-, -]
        uint32_t Year : 16 = 1990;
        //! Month [1, 12]
        uint32_t Month : 8 = 1;
        //! Day [1, 31]
        uint32_t Day : 8 = 1;
    	
        //! Hour [0, 23]
        uint32_t Hour : 5 = 0;
        //! Minute [0, 59]
        uint32_t Minute : 6 = 0;
        //! Second [0, 59]
        uint32_t Second : 6 = 0;
        //! Millisecond [0, 999]
        uint32_t Millisecond : 10 = 0;
        //! Microsecond [0, 999]
        uint32_t Microsecond : 10 = 0;
        //! Nanosecond [0, 999]
        uint32_t Nanosecond : 10 = 0;
    	
    public:
        static bool IsLeapyear(uint32_t Year);
        static EDayWeek DayWeek(uint32_t Year, uint32_t Month, uint32_t Day);
        static uint32_t DayYear(uint32_t Year, uint32_t Month, uint32_t Day);

        static FDateTime Now();

    public:
        static float64_t System();
        static uint64_t SystemMilliseconds();
        static uint64_t SystemMicroseconds();
        static uint64_t SystemNanoseconds();
    	
        static float64_t Steady();
        static uint64_t SteadyMilliseconds();
        static uint64_t SteadyMicroseconds();
        static uint64_t SteadyNanoseconds();
    	
        static float64_t HighResolution();
        static uint64_t HighResolutionMilliseconds();
        static uint64_t HighResolutionMicroseconds();
    	
        static uint64_t HighResolutionNanoseconds();
    };
}
