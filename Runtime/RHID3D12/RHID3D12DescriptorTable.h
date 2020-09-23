#pragma once

#include "RHID3D12Core.h"
#include "RHID3D12Device.h"

namespace XE::RHI::D3D12
{
	class FRHID3D12DescriptorTable : public FRHIDescriptorTable
	{
	public:
		FRHID3D12DescriptorTable(FRHID3D12Device * RHID3D12DeviceIn, FRHID3D12DescriptorHeap * RHID3D12DescriptorHeapIn) : RHID3D12Device(RHID3D12DeviceIn), RHID3D12DescriptorHeap(RHID3D12DescriptorHeapIn) {}
		virtual ~FRHID3D12DescriptorTable() = default;

		EError Create(uint32_t NumDescriptors);

		void SetResource(size_t Index, FRHIResource * RHIResource, const FResourceViewArgs & ResourceViewArgsIn) override;
		FGPUAddress GetCPUAddress(size_t Index) const override { return DescriptorTableCPUAddress; }
		FGPUAddress GetGPUAddress(size_t Index) const override { return DescriptorTableGPUAddress; }
		
	public:
		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle()
		{
			D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle = { DescriptorTableCPUAddress };
			return CPUDescriptorHandle;
		}
		D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle()
		{
			D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle = { DescriptorTableGPUAddress };
			return GPUDescriptorHandle;
		}
		
	private:
		FRHID3D12Device * RHID3D12Device = nullptr;
		FRHID3D12DescriptorHeap * RHID3D12DescriptorHeap = nullptr;

		size_t DescriptorsIndexBase = 0;
		addr_t DescriptorTableCPUAddress = 0;
		addr_t DescriptorTableGPUAddress = 0;
		
		TVector<FResourceViewArgs> DescriptorViewArgs;
	};
}
