#pragma once

#include "RHID3D12Core.h"

namespace XE::RHI::D3D12
{
	class FRHID3D12ResourceView : public FRHIResourceView
	{
	public:
		FRHID3D12ResourceView(FRHID3D12Device * RHID3D12DeviceIn) : RHID3D12Device(RHID3D12DeviceIn) {}

		EError Create(FRHIResource * RHIResourceIn, const FResourceViewArgs & ResourceViewArgsIn);

	public:
		FCPUAddress GetCPUAddress() const override { return D3D12CPUDescriptorHandle.ptr; }
		FGPUAddress GetGPUAddress() const override { return D3D12GPUDescriptorHandle.ptr; }
		FRHIResource * GetRHIResource() const override { return RHIResource.Get(); }
		
		const FResourceViewArgs & GetResourceViewArgs() const override { return ResourceViewArgs; }
	public:
		D3D12_GPU_DESCRIPTOR_HANDLE GetD3D12GPUDescriptorHandle() const { return D3D12GPUDescriptorHandle ; }
		
	private:
		FRHID3D12Device * RHID3D12Device = nullptr;

		TReferPtr<ID3D12Resource> D3D12Resource;
		ECommandType CommandType = ECommandType::Direct;

		TPointer<FRHIResource> RHIResource;
		FResourceViewArgs ResourceViewArgs;
		D3D12_CPU_DESCRIPTOR_HANDLE D3D12CPUDescriptorHandle = {};
		D3D12_GPU_DESCRIPTOR_HANDLE D3D12GPUDescriptorHandle = {};
	};
}
