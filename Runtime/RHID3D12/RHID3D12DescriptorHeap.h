#pragma once

#include "RHID3D12Core.h"
#include "RHID3D12Device.h"

namespace XE::RHI::D3D12
{
	class FRHID3D12DescriptorHeap : public FRHIDescriptorHeap
	{
	public:
		FRHID3D12DescriptorHeap(FRHID3D12Device * RHID3D12DeviceIn) : RHID3D12Device(RHID3D12DeviceIn) {}
		virtual ~FRHID3D12DescriptorHeap() = default;

		EError Create(const FDescriptorHeapDesc & DescriptorHeapArgsIn);

		EDescriptorHeapType GetDescriptorHeapType() const { return DescriptorHeapArgs.DescriptorHeapType; }
		size_t GetCPUAddressUnit() const override { return DescriptorHandleUnit; }
		size_t GetGPUAddressUnit() const override { return DescriptorHandleUnit;}
		FCPUAddress GetCPUAddress(size_t Index) const override
		{
			D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle = D3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			return CPUDescriptorHandle.ptr + DescriptorHandleUnit * Index;
		}
		FGPUAddress GetGPUAddress(size_t Index) const override
		{
			D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle = D3D12DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			return GPUDescriptorHandle.ptr + DescriptorHandleUnit * Index;
		}
		
		FDescriptorIndex AllocDescriptors(size_t Count) override
		{
			size_t Index = ResourceIndex;
			ResourceIndex += Count;
			return Index;
		}

		FCPUAddress SetResource(size_t Index, FRHIResource * RHIResource, const FResourceViewArgs & ResourceViewArgsIn) override;
		
	public:
		ID3D12DescriptorHeap * GetD3D12DescriptorHeap() { return D3D12DescriptorHeap.Get(); }
		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle(uint32_t DescriptorIndex)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle = D3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			CPUDescriptorHandle.ptr += DescriptorHandleUnit * DescriptorIndex;
			return CPUDescriptorHandle;
		}
		D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle(uint32_t DescriptorIndex)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle = D3D12DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			GPUDescriptorHandle.ptr += DescriptorHandleUnit * DescriptorIndex;
			return GPUDescriptorHandle;
		}
		
	private:
		FRHID3D12Device * RHID3D12Device = nullptr;
		TReferPtr<ID3D12DescriptorHeap> D3D12DescriptorHeap;
		FDescriptorHeapDesc DescriptorHeapArgs;
		uint32_t DescriptorHandleUnit = 0;
		size_t ResourceIndex = 0;

		TVector<FResourceViewArgs> ResourceViewArgs;
	};
}
