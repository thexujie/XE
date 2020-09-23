#include "PCH.h"
#include "RHID3D12CommandAllocator.h"

namespace XE::RHI::D3D12
{
	EError FRHID3D12CommandAllocator::Create(ECommandType CommandTypeIn)
	{
		if (!RHID3D12Device)
			return EError::State;

		HRESULT HR = S_OK;

		auto Device = RHID3D12Device->GetDevice();
		auto Adapter = RHID3D12Device->GetAdapter();
		assert(Device);
		assert(Adapter);

		TReferPtr<ID3D12CommandAllocator> TempCommandAllocator;
		HR = Device->CreateCommandAllocator(FromCommandType(CommandTypeIn), __uuidof(ID3D12CommandAllocator), TempCommandAllocator.GetVV());
		if (FAILED(HR))
		{
			LogWarning(Str(" Device->CreateCommandAllocator failed: {1}"), Win32::ErrorStr(HR & 0xFFFF));
			return EError::Inner;
		}

		CommandType = CommandTypeIn;
		D3D12CommandAllocator = TempCommandAllocator;
		return EError::OK;
	}

	void FRHID3D12CommandAllocator::Reset()
	{
		D3D12CommandAllocator->Reset();
	}
	
	void FRHID3D12CommandAllocator::SetName(FStringView Name)
	{
		SetD3D12ObjectName(D3D12CommandAllocator.Get(), Name);
	}
}
