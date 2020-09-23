#include "PCH.h"
#include "RHID3D12SwapChain.h"
#include "RHID3D12Factory.h"
#include "RHID3D12CommandList.h"
#include "RHID3D12CommandQueue.h"

namespace XE::RHI::D3D12
{
	EError FRHID3D12SwapChain::Create(FRHICommandQueue * CommandQueue, const FSwapChainDesc & RenderTargetArgsIn)
	{
		if (!RHID3D12Device)
			return EError::State;

		HRESULT HR = S_OK;

		auto Factory = RHID3D12Device->GetFactory()->GetFactory();
		auto Device = RHID3D12Device->GetDevice();
		auto Adapter = RHID3D12Device->GetAdapter();
		assert(Device);
		assert(Adapter);

		//ComPtr<IDXGIFactory3> factory;
		//hr = adapter->GetParent(__uuidof(IDXGIFactory3), factory.getvv());
		//if (FAILED(hr))
		//{
		//	Warning() << __FUNCTION__ " adapter->GetParent failed: " << Win32::ErrorStr(hr & 0xFFFF);
		//	return EError::Inner;
		//}
		//
		
		auto D3D12CommandQueue = static_cast<FRHID3D12CommandQueue *>(CommandQueue)->CommandQueue();

		DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
		SwapChainDesc.BufferCount = RenderTargetArgsIn.NumBuffers;
		SwapChainDesc.Width = RenderTargetArgsIn.Size.Width;
		SwapChainDesc.Height = RenderTargetArgsIn.Size.Height;
		SwapChainDesc.Format = FromFormat(RenderTargetArgsIn.Format);
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.SwapEffect = FromSwapEffect(RenderTargetArgsIn.SwapEffect);
		SwapChainDesc.SampleDesc.Count = RenderTargetArgsIn.MSAA.Level;
		SwapChainDesc.SampleDesc.Quality = RenderTargetArgsIn.MSAA.Quality;

		TReferPtr<IDXGISwapChain1> TempDXGISwapChain;
		HR = Factory->CreateSwapChainForHwnd(
			D3D12CommandQueue,
			(HWND)RenderTargetArgsIn.WindowHandle,
			&SwapChainDesc,
			nullptr,
			nullptr,
			TempDXGISwapChain.GetPP()
		);
		if (FAILED(HR))
		{
			LogError(Str("CreateSwapChainForHwnd failed, {}"), Win32::ErrorStr(HR & 0xFFFF));
			return EError::Inner;
		}
		HR = Factory->MakeWindowAssociation((HWND)RenderTargetArgsIn.WindowHandle, DXGI_MWA_NO_ALT_ENTER);
		if (FAILED(HR))
		{
			LogWarning(Str("MakeWindowAssociation(DXGI_MWA_NO_ALT_ENTER) failed, {}"), Win32::ErrorStr(HR & 0xFFFF));
		}

		TVector<TSharedPtr<FRHID3D12Resource>> TempRHIResources(RenderTargetArgsIn.NumBuffers);
		{
			for (uint32_t BufferIndex = 0; BufferIndex < RenderTargetArgsIn.NumBuffers; BufferIndex++)
			{
				TReferPtr<ID3D12Resource> D3D12Resource;
				HR = TempDXGISwapChain->GetBuffer(BufferIndex, __uuidof(ID3D12Resource), D3D12Resource.GetVV());
				if (FAILED(HR))
				{
					LogError(Str("GetBuffer failed, {}"), Win32::ErrorStr(HR & 0xFFFF));
					return EError::Inner;
				}

				TempRHIResources[BufferIndex] = MakeShared<FRHID3D12Resource>(RHID3D12Device, D3D12Resource);
				TempRHIResources[BufferIndex]->SetName(Strings::Printf(Str("FRHID3D12RenderTargetHWND::RHIResources[%u]"), BufferIndex));
			}
		}

		//---------------------------------------------------------------
		SwapChainArgs = RenderTargetArgsIn;
		DXGISwapChain = TempDXGISwapChain.QueryInterface<IDXGISwapChain3>();
		FameIndex = DXGISwapChain->GetCurrentBackBufferIndex();
		RHIResources = TempRHIResources;

		return EError::OK;
	}

	void FRHID3D12SwapChain::SetName(FStringView Name)
	{
	}

	void FRHID3D12SwapChain::Begin()
	{
		FameIndex = DXGISwapChain->GetCurrentBackBufferIndex();
	}

	void FRHID3D12SwapChain::Present(uint32_t SyncValue)
	{
		if (!DXGISwapChain)
			return ;

		DXGISwapChain->Present(SyncValue, 0);
	}

	uint32_t FRHID3D12SwapChain::FrameIndex() const
	{
		return DXGISwapChain->GetCurrentBackBufferIndex();
	}
}
