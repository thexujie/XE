#pragma once

#include "RHID3D12Core.h"
#include "RHID3D12Device.h"

namespace XE::RHI::D3D12
{
	class FRHID3D12RootSignature : public FRHIRootSignature
	{
	public:
		FRHID3D12RootSignature(FRHID3D12Device * RHID3D12DeviceIn) : RHID3D12Device(RHID3D12DeviceIn) {}

		EError Create(const FRootSignatureDesc & RootSignatureArgsIn);
		void SetName(FStringView Name) override;

	public:
		const FRootSignatureDesc & Args() const { return RootSignatureArgs; }
		ID3D12RootSignature * RootSignature() const { return D3D12RootSignature.Get(); }

	private:
		FRHID3D12Device * RHID3D12Device = nullptr;

		FRootSignatureDesc RootSignatureArgs;
		TReferPtr<ID3D12RootSignature> D3D12RootSignature;

		ECommandType CommandType = ECommandType::Direct;
		ECommandQueueFlags CommandQueueFlags = ECommandQueueFlags::None;
	};
}
