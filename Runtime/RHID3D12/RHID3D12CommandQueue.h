#pragma once

#include "RHID3D12Core.h"
#include "RHID3D12Device.h"

namespace XE::RHI::D3D12
{
	class FRHID3D12CommandQueue : public FRHICommandQueue
	{
	public:
		FRHID3D12CommandQueue(FRHID3D12Device * RHID3D12DeviceIn) : RHID3D12Device(RHID3D12DeviceIn) {}
		virtual ~FRHID3D12CommandQueue() = default;

		EError Create(ECommandType CommandTypeIn, ECommandQueueFlags CommandQueueFlagsIn);
		void SetName(FStringView Name) override;

	public:
		void ExcuteCommandList(FRHICommandList * RHICommandList) override;
		void Signal(FRHIFence * RHIFence, uint64_t SignalValue) override;
		void Fence(FRHIFence * RHIFence, uint64_t SignalValue) override;

	public:
		ID3D12CommandQueue * CommandQueue() const { return D3D12CommandQueue.Get(); }
		
	private:
		FRHID3D12Device * RHID3D12Device = nullptr;
		TReferPtr<ID3D12CommandQueue> D3D12CommandQueue;
		ECommandType CommandType = ECommandType::Direct;
		ECommandQueueFlags CommandQueueFlags = ECommandQueueFlags::None;
	};
}
