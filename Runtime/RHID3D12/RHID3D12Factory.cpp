#include "PCH.h"
#include "RHID3D12Factory.h"
#include "RHID3D12Device.h"

namespace XE::RHI::D3D12
{
	EError FRHID3D12Factory::Load()
	{
		if (DXGIFactory)
			return EError::OK;

		EError err = DXGILibrary.Load(Strings::FromUtf8(u8"dxgi.dll"));
		if (!!err)
		{
			LogError(Str(" load dxgi.dll failed, {}"), Win32::ErrorStr(GetLastError()));
			return err;
		}

		HRESULT HR = S_OK;
#ifdef _DEBUG
		TReferPtr<ID3D12Debug> D3D12Debug;
		HR = D3D12GetDebugInterface(IID_PPV_ARGS(D3D12Debug.GetPP()));
		if (SUCCEEDED(HR))
			D3D12Debug->EnableDebugLayer();
#endif

		auto PfnCreateDXGIFactory = DXGILibrary.GetProc<decltype(CreateDXGIFactory) *>("CreateDXGIFactory");
		if (!PfnCreateDXGIFactory)
		{
			LogError(Str(" canot find CreateDXGIFactory in dxgi.dll, {}"), Win32::ErrorStr(GetLastError()));
			return EError::NotFound;
		}

		HR = PfnCreateDXGIFactory(__uuidof(IDXGIFactory3), DXGIFactory.GetVV());
		if (FAILED(HR))
		{
			LogError(Str(" CreateDXGIFactory<IDXGIFactory3> faild, {}"), Win32::ErrorStr(GetLastError()));
			return EError::NotFound;
		}

		return EError::OK;
	}


	TVector<FRHIAdapterDesc> FRHID3D12Factory::GetAdapterDescs() const
	{
		if (!DXGIFactory)
			return {};

		TVector<FRHIAdapterDesc> AdapterDesc;
		UINT AdapterIndex = 0;
		TReferPtr<IDXGIAdapter1> DXGIAdapter;
		while(DXGIFactory->EnumAdapters1(AdapterIndex++, DXGIAdapter.GetPP()) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 DXGIAdapterDesc = {};
			DXGIAdapter->GetDesc1(&DXGIAdapterDesc);
			DXGIAdapter.Reset();

			TReferPtr<ID3D12Device> D3D12Device;
			HRESULT hr = D3D12CreateDevice(DXGIAdapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), D3D12Device.GetVV());
			if (FAILED(hr))
			{
				LogWarning(Str( " D3D12CreateDevice failed."));
				continue;
			}

			D3D12_FEATURE_DATA_ARCHITECTURE Architecture = {};
			hr = D3D12Device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &Architecture, sizeof(Architecture));
			if (FAILED(hr))
			{
				LogWarning(Str( " CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE) failed."));
				continue;
			}
			
			//D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAALevels = {};
			//MSAALevels.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			//MSAALevels.SampleCount = 2;
			//hr = device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MSAALevels, sizeof(MSAALevels));
			//if (FAILED(hr))
			//{
			//	Warning() << __FUNCTION__ << " CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS) failed.";
			//	continue;
			//}


			//TArray<FRHIAdapterDesc> Strings;
			//FRHIAdapterDesc StrA{ .name = "ABCD" };
			//Strings.Add(FRHIAdapterDesc());
			//Strings.Emplace(Move(StrA));
			
			FRHIAdapterDesc RHIAdapterDesc;
			RHIAdapterDesc.DeviceId = DXGIAdapterDesc.DeviceId;
			RHIAdapterDesc.Name = Strings::FromWide(DXGIAdapterDesc.Description);
			RHIAdapterDesc.Flags |= FlagSelect(ERHIAdapterFlags::CacheCoherentUMA, Architecture.CacheCoherentUMA);
			RHIAdapterDesc.Flags |= FlagSelect(ERHIAdapterFlags::TileBasedRender, Architecture.TileBasedRenderer);
			RHIAdapterDesc.Flags |= FlagSelect(ERHIAdapterFlags::Software, DXGIAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE);
			AdapterDesc.Emplace(Move(RHIAdapterDesc));
		}
		return AdapterDesc;
	}

	TSharedPtr<FRHIDevice> FRHID3D12Factory::CreateDevice(uint64_t Id) const
	{
		if (!DXGIFactory)
			return nullptr;

		UINT AdapterIndex = 0;
		TReferPtr<IDXGIAdapter1> Adapter;

		while (DXGIFactory->EnumAdapters1(AdapterIndex++, Adapter.GetPP()) != DXGI_ERROR_NOT_FOUND)
		{
			if (!Id)
				break;
			
			DXGI_ADAPTER_DESC1 dxgidesc = {};
			Adapter->GetDesc1(&dxgidesc);
			if (dxgidesc.DeviceId == Id)
				break;

			Adapter.Reset();
		}

		if (!Adapter)
			return nullptr;

		TSharedPtr<FRHID3D12Device> Device = MakeShared<FRHID3D12Device>(const_cast<FRHID3D12Factory *>(this));
		auto err = Device->Create(Adapter);
		if (!!err)
			return nullptr;
		return Device;
	}
}