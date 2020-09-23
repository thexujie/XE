#pragma once

#include "RHIInc.h"

namespace XE::RHI
{
	class FRHIFence;
	class FRHICommandQueue;
	class FRHICommandAllocator;
	class FRHICommandList;
	class FRHIResource;
	class FRHISwapChain;
	class FRHIRootSignature;
	class FRHIPipelineState;
	class FRHIDescriptorHeap;
	class FRHIDescriptorTable;
	class FRHIResourceView;
	
	class RHI_API FRHIDevice
	{
	public:
		FRHIDevice() = default;
		virtual ~FRHIDevice() = default;

		const FRHIAdapterDesc & Desc() const { return AdapterDesc; }

	public:
		virtual TSharedPtr<FRHIFence> CreateFence(EFenceFlags Flags) const = 0;
		virtual TSharedPtr<FRHICommandQueue> CreateCommandQueue(ECommandType ECommandType, ECommandQueueFlags Flags) const = 0;
		virtual TSharedPtr<FRHICommandList> CreateCommandList(ECommandType ECommandType) const = 0;
		virtual TSharedPtr<FRHIResource> CreateResource(const FResourceDesc & ResourceDesc) const = 0;
		virtual TSharedPtr<FRHIDescriptorHeap> CreateDescriptorHeap(const FDescriptorHeapDesc & DescriptorHeapDesc) const = 0;
		virtual TSharedPtr<FRHIDescriptorTable> CreateDescriptorTable(FRHIDescriptorHeap * RHIDescriptorHeap, uint32_t NumDescriptors) const = 0;
		virtual TSharedPtr<FRHISwapChain> CreateSwapChain(FRHICommandQueue * CommandQueue, const FSwapChainDesc & SwapChainDesc) const = 0;
		virtual TSharedPtr<FRHIRootSignature> CreateRootSignature(const FRootSignatureDesc & RootSignatureDesc) const = 0;
		virtual TSharedPtr<FRHIPipelineState> CreateGraphicsPipelineState(FRHIRootSignature * RHIRootSignature, const FGraphicsPipelistStateDesc & GraphicsPipelistStateDesc) const = 0;
		virtual TSharedPtr<FRHIPipelineState> CreateComputePipelineState(FRHIRootSignature * RHIRootSignature, const FComputePipelineStateDesc & ComputePipelineStateDesc) const = 0;

		virtual void GetCopyableFootprints(FRHIResource * RHIResourceDst, uint32_t SubResourceIndex, uint32_t SubResourceCount, FCopyableFootprint * CopyableFootprints) const = 0;
		virtual FCopyableFootprint GetCopyableFootprint(FRHIResource * RHIResourceDst) const = 0;

		virtual FAddress AllocateDescriptor(EDescriptorHeapType DescriptorHeapType, size_t Count) = 0;
		virtual void BindDescriptor(FRHIResource * RHIResourceIn, FCPUAddress CPUAddress, const FResourceViewArgs & ResourceViewArgs) = 0;
		virtual TSharedPtr<FRHIResourceView> CreateResourceView(FRHIResource * RHIResource, const FResourceViewArgs & ResourceViewArgs) = 0;
		
	protected:
		FRHIAdapterDesc AdapterDesc;
	};
}
