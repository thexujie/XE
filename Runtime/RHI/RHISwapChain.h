#pragma once

#include "RHIInc.h"
#include "RHIResource.h"

namespace XE::RHI
{
	class FRHIResource;

	class RHI_API FRHISwapChain : public FRHIObject
	{
	public:
		FRHISwapChain() = default;
		virtual ~FRHISwapChain() = default;

	public:
		virtual SizeU GetSize() const = 0;
		
	public:
		virtual void Begin() = 0;
		virtual void Present(uint32_t SyncValue) = 0;
		virtual uint32_t FrameIndex() const = 0;
		virtual FRHIResource * GetBuffer(uint32_t Index) const = 0;
	};
}
