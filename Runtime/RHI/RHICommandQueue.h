#pragma once

#include "RHIInc.h"

namespace XE::RHI
{
	class FRHIFence;
	class FRHICommandList;

	class RHI_API FRHICommandQueue : public FRHIDeviceObject
	{
	public:
		FRHICommandQueue() = default;
		virtual ~FRHICommandQueue() = default;

	public:
		virtual void ExcuteCommandList(FRHICommandList * RHICommandList) = 0;
		virtual void Signal(FRHIFence * RHIFence, uint64_t SignalValue) = 0;
		virtual void Fence(FRHIFence * RHIFence, uint64_t SignalValue) = 0;
	};
}


