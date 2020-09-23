#pragma once

#include "RHIInc.h"
#include "RHIResource.h"
#include "RHIResourceView.h"
#include "RHIDevice.h"
#include "RHIFence.h"
#include "RHICommandAllocator.h"
#include "RHICommandQueue.h"
#include "RHICommandList.h"
#include "RHICommandQueue.h"
#include "RHISwapChain.h"
#include "RHIRootSignature.h"
#include "RHIPipelineState.h"
#include "RHIDescriptorHeap.h"
#include "RHIDescriptorTable.h"

namespace XE::RHI
{
	class RHI_API FRHIFactory
	{
	public:
		FRHIFactory() = default;
		virtual ~FRHIFactory() = default;

		virtual EError Load() = 0;
		virtual TVector<FRHIAdapterDesc> GetAdapterDescs() const = 0;
		virtual TSharedPtr<FRHIDevice> CreateDevice(uint64_t Id) const = 0;
	};
}
