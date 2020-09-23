#pragma once

#include "CoreInc.h"

namespace XE
{
	class CORE_API FFileLinePeriod
	{
	public:
		FFileLinePeriod(const char * File, int Line, size_t PeriodIn = 1000, size_t CountIn = 1) : Id(File, Line), Period(PeriodIn), Count(CountIn)
		{

		}

		bool Check() const;

		TTuple<const char *, int> Id;
		/**
		 * in ms
		 */
		size_t Period;
		size_t Count = 1;
	};
}