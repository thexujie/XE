#pragma once

#include "RHID3D12Core.h"
#include "RHID3D12Device.h"
#include "RHID3D12Resource.h"

namespace XE::RHI::D3D12
{
	class FRHID3D12SwapChain : public FRHISwapChain
	{
	public:
		FRHID3D12SwapChain(FRHID3D12Device * RHID3D12DeviceIn) : RHID3D12Device(RHID3D12DeviceIn) {}
		virtual ~FRHID3D12SwapChain() = default;

		EError Create(FRHICommandQueue * CommandQueue, const FSwapChainDesc & RenderTargetArgsIn);
		void SetName(FStringView Name) override;

	public:
		SizeU GetSize() const override { return SwapChainArgs.Size; }
		
	public:
		void Begin() override;
		void Present(uint32_t SyncValue) override;
		uint32_t FrameIndex() const override;
		
	public:
		FRHIResource * GetBuffer(uint32_t Index) const { return RHIResources[Index].Get(); }

	private:
		FRHID3D12Device * RHID3D12Device = nullptr;
		FSwapChainDesc SwapChainArgs;

		TReferPtr<IDXGISwapChain3> DXGISwapChain;
		TVector<TSharedPtr<FRHID3D12Resource>> RHIResources;

		uint32_t FameIndex = 0;
	};
}
