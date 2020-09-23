#pragma once

#include "RHID3D12Core.h"
#include "RHID3D12Device.h"

namespace XE::RHI::D3D12
{
	class FRHID3D12Resource : public FRHIResource
	{
	public:
		FRHID3D12Resource(FRHID3D12Device * RHID3D12DeviceIn) : RHID3D12Device(RHID3D12DeviceIn) {}
		FRHID3D12Resource(FRHID3D12Device * RHID3D12DeviceIn, TReferPtr<ID3D12Resource> D3D12ResourceIn);
		virtual ~FRHID3D12Resource() = default;

		EError Create(const FResourceDesc & ResourceArgsIn);
		void SetName(FStringView Name) override;

	public:
		void * GetData() override;
		const void * GetData() const override;
		SizeU GetSize() const override;
		const FResourceDesc & GetResourceArgs() const override { return ResourceArgs; }
		
		FGPUAddress GetGPUAddress() const override { return D3D12Resource ? FGPUAddress(D3D12Resource->GetGPUVirtualAddress()) : 0; }

	public:
		ID3D12Resource * Resource() const { return D3D12Resource.Get(); }
		D3D12_GPU_VIRTUAL_ADDRESS GetD3D12GPUVirtualAddress() const { return D3D12Resource->GetGPUVirtualAddress(); }
		EResourceStates GetResourceStates() const { return ResourceStates; }
		void SetResourceStates(EResourceStates States) { ResourceStates = States; }
		
	private:
		FRHID3D12Device * RHID3D12Device = nullptr;
		FResourceDesc ResourceArgs;
		TReferPtr<ID3D12Resource> D3D12Resource;
		void * RawPointer = nullptr;
		EResourceStates ResourceStates = EResourceStates::None;
	};
}
