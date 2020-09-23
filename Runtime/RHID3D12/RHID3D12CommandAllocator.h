#pragma once

#include "RHID3D12Core.h"
#include "RHID3D12Device.h"

namespace XE::RHI::D3D12
{
	class FRHID3D12SwapChain;

	class FRHID3D12CommandAllocator : public FRHICommandAllocator
	{
	public:
		FRHID3D12CommandAllocator(FRHID3D12Device * RHID3D12DeviceIn) : RHID3D12Device(RHID3D12DeviceIn) {}
		virtual ~FRHID3D12CommandAllocator() = default;

		EError Create(ECommandType CommandTypeIn);
		void SetName(FStringView Name) override;

	public:
		ECommandType GetCommandType() const { return CommandType; }
		void Reset() override;

	public:
		ID3D12CommandAllocator * CommandAllocator() const { return D3D12CommandAllocator.Get(); }

	private:
		FRHID3D12Device * RHID3D12Device = nullptr;
		ECommandType CommandType = ECommandType::Direct;
		TReferPtr<ID3D12CommandAllocator> D3D12CommandAllocator;
	};
}
