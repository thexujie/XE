#pragma once

#include "RHIInc.h"

namespace XE::RHI
{
	class RHI_API FRHIResource : public FRHIObject
	{
	public:
		FRHIResource() = default;

	public:
		virtual void * GetData() = 0;
		virtual const void * GetData() const = 0;
		virtual SizeU GetSize() const = 0;
		virtual const FResourceDesc & GetResourceArgs() const = 0;
		virtual FGPUAddress GetGPUAddress() const = 0;
	};
}
