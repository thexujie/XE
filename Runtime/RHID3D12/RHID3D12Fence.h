#pragma once

#include "RHID3D12Core.h"

namespace XE::RHI::D3D12
{
	class FRHID3D12Fence: public FRHIFence
	{
	public:
		FRHID3D12Fence(FRHID3D12Device * RHID3D12DeviceIn) : RHID3D12Device(RHID3D12DeviceIn) {}
		~FRHID3D12Fence();

		EError Create(EFenceFlags FenceFlags);
		void SetName(FStringView Name) override;

public:
	uint64_t GetValue() const override;
	void Signal(uint64_t SignalValue) const override;
	void Wait(uint64_t SignalValue, FSignal * Signal) const override;
	void Complete(uint64_t SignalValue, FSignal * Signal) const override;
		
public:
	ID3D12Fence * Fence() const { return D3D12Fence.Get(); }
		
	private:
		FRHID3D12Device * RHID3D12Device = nullptr;
		FResourceDesc ResourceArgs;
		TReferPtr<ID3D12Fence> D3D12Fence;
	};
}
