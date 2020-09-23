#pragma once

#include "RHIInc.h"
#include "RHIResource.h"

namespace XE::RHI
{
	class RHI_API FRHIResourceView : public FRHIObject
	{
	public:
		virtual FCPUAddress GetCPUAddress() const = 0;
		virtual FGPUAddress GetGPUAddress() const = 0;
		virtual FRHIResource * GetRHIResource() const = 0;
		virtual const FResourceViewArgs & GetResourceViewArgs() const = 0;
	};
}
