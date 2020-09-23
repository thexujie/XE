#pragma once

#include "RHID3D12Inc.h"

namespace XE::RHI::D3D12
{
	class FRHID3D12Device : public FRHIDevice
	{
	public:
		FRHID3D12Device(FRHID3D12Factory * RHID3D12FactoryIn) : RHID3D12Factory(RHID3D12FactoryIn) {}
		virtual ~FRHID3D12Device() = default;

		EError Create(TReferPtr<IDXGIAdapter1> AdapterIn);

		FRHID3D12Factory * GetFactory() const { return RHID3D12Factory; }
		TReferPtr<ID3D12Device> GetDevice() const { return D3D12Device; }
		TReferPtr<IDXGIAdapter1> GetAdapter() const { return DXGIAdapter; }

	public:
		TSharedPtr<FRHIFence> CreateFence(EFenceFlags FenceFlags) const override;
		TSharedPtr<FRHICommandQueue> CreateCommandQueue(ECommandType CommandType, ECommandQueueFlags CommandQueueFlags) const override;
		TSharedPtr<FRHICommandList> CreateCommandList(ECommandType CommandType) const override;
		TSharedPtr<FRHIResource> CreateResource(const FResourceDesc & ResourceArgs) const override;
		TSharedPtr<FRHIDescriptorHeap> CreateDescriptorHeap(const FDescriptorHeapDesc & DescriptorHeapArgs) const override;
		TSharedPtr<FRHIDescriptorTable> CreateDescriptorTable(FRHIDescriptorHeap * RHIDescriptorHeap, uint32_t NumDescriptors) const override;
		TSharedPtr<FRHISwapChain> CreateSwapChain(FRHICommandQueue * RHICommandQueue, const FSwapChainDesc & SwapChainArgs) const override;
		TSharedPtr<FRHIRootSignature> CreateRootSignature(const FRootSignatureDesc & RootSignatureArgs) const override;
		TSharedPtr<FRHIPipelineState> CreateGraphicsPipelineState(FRHIRootSignature * RHIRootSignature, const FGraphicsPipelistStateDesc & GraphicsPipelistStateArgs) const override;
		TSharedPtr<FRHIPipelineState> CreateComputePipelineState(FRHIRootSignature * RHIRootSignature, const FComputePipelineStateDesc & ComputePipelineState) const override;

		void GetCopyableFootprints(FRHIResource * RHIResourceDst, uint32_t SubResourceIndex, uint32_t SubResourceCount, FCopyableFootprint * CopyableFootprints) const override;
		FCopyableFootprint GetCopyableFootprint(FRHIResource * RHIResourceDst) const override;

		FAddress AllocateDescriptor(EDescriptorHeapType DescriptorHeapType, size_t Count) override;
		void BindDescriptor(FRHIResource * RHIResourceIn, FCPUAddress CPUAddress, const FResourceViewArgs & ResourceViewArgs);
		
		TSharedPtr<FRHIResourceView> CreateResourceView(FRHIResource * RHIResource, const FResourceViewArgs & ResourceViewArgs) override;
	public:
		FAddress AllocateDescriptor(EDescriptorHeapType DescriptorHeapType);
		const TArray<TReferPtr<ID3D12DescriptorHeap>, DescriptorHeapTypeCount> & GetD3D12DescriptorHeaps() const { return D3D12DescriptorHeaps; }
		
	private:
		FRHID3D12Factory * RHID3D12Factory;
		TReferPtr<IDXGIAdapter1> DXGIAdapter;
		TReferPtr<ID3D12Device> D3D12Device;

		TArray<size_t, DescriptorHeapTypeCount> NumDescriptors = { 8192, 32, 32, 0};
		TArray<TReferPtr<ID3D12DescriptorHeap>, DescriptorHeapTypeCount> D3D12DescriptorHeaps;
		TArray<FBitSet, DescriptorHeapTypeCount> D3D12DescriptorHeapSRVUseds;
		TArray<size_t, DescriptorHeapTypeCount> D3D12DescriptorUnitSizes;
	};
}
