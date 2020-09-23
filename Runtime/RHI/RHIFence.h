#pragma once

#include "RHIInc.h"

namespace XE::RHI
{
	class RHI_API FRHIFence : public FRHIObject
	{
	public:
		virtual uint64_t GetValue() const = 0;
		
		virtual void Signal(uint64_t SignalValue) const = 0;
		virtual void Wait(uint64_t SignalValue, FSignal * Signal) const = 0;
		virtual void Complete(uint64_t SignalValue, FSignal * Signal) const = 0;
	};

}
