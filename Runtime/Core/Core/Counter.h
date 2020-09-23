#pragma once

#include "CoreInc.h"
#include "DateTime.h"

namespace XE
{
    template<typename ValT, size_t NumValues>
    class FCounter
    {
    public:
        FCounter()
        {
            for (auto & Value : Values)
                Value = 0;
        }
    	
        FCounter(uint64_t PeriodIn = 1000) : Period(PeriodIn)
        {
            for (auto & Value : Values)
                Value = 0;
        }

        void Accumulate(ValT Value)
        {
            uint64_t Now = FDateTime::SteadyMilliseconds();
            if (Now - Last > Period)
            {
                Values[CurrIndex++ % NumValues] = CurrValue;
                CurrValue = 0;
                Last = Now;
            }
            CurrValue += Value;
        }
    	
        ValT Average() const
        {
            return std::accumulate(Values.begin(), Values.end(), static_cast<ValT>(0)) / Values.size();
        }

    private:
        uint64_t Period;
        uint64_t Last;
        std::array<ValT, NumValues> Values;

        int64_t CurrIndex = 0;
        ValT CurrValue = 0;
    };
}