#pragma once

#include "RHIInc.h"

namespace XE::RHI
{
	class RHI_API FRHICommandAllocator: public FRHIDeviceObject
	{
	public:
		FRHICommandAllocator() = default;
		virtual ~FRHICommandAllocator() = default;
		
	public:
		virtual ECommandType GetCommandType() const = 0;
		virtual void Reset() = 0;
	};
}
