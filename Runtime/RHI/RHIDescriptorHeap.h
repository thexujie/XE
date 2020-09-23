#pragma once

#include "RHIInc.h"
#include "RHI.h"

namespace XE::RHI
{
	class RHI_API FRHIDescriptorHeap : public FRHIObject
	{
	public:
		virtual EDescriptorHeapType GetDescriptorHeapType() const = 0;
		virtual size_t GetCPUAddressUnit() const = 0;
		virtual size_t GetGPUAddressUnit() const = 0;
		virtual FCPUAddress GetCPUAddress(size_t Index) const = 0;
		virtual FGPUAddress GetGPUAddress(size_t Index) const = 0;
		virtual FDescriptorIndex AllocDescriptors(size_t Count) = 0;
		virtual FCPUAddress SetResource(size_t Index, FRHIResource * RHIResource, const FResourceViewArgs & ResourceViewArgsIn) = 0;
	};
}
