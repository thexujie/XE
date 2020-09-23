#pragma once

#include "RHIInc.h"
#include "RHI.h"

namespace XE::RHI
{
	class RHI_API FRHIDescriptorTable: public FRHIObject
	{
	public:
		virtual void SetResource(size_t Index, FRHIResource * RHIResource, const FResourceViewArgs & ResourceViewArgs) = 0;
		virtual FCPUAddress GetCPUAddress(size_t Index) const = 0;
		virtual FGPUAddress GetGPUAddress(size_t Index) const = 0;
		
	};
}
