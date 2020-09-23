#include "PCH.h"
#include "RHID3D12Fence.h"
#include "RHID3D12Device.h"

namespace XE::RHI::D3D12
{
	FRHID3D12Fence::~FRHID3D12Fence()
	{
	}
	
	EError FRHID3D12Fence::Create(EFenceFlags FenceFlags)
	{
		if (!RHID3D12Device)
			return EError::State;

		HRESULT HR = S_OK;

		auto Device = RHID3D12Device->GetDevice();
		auto Adapter = RHID3D12Device->GetAdapter();
		assert(Device);
		assert(Adapter);

		TReferPtr<ID3D12Fence> TempD3D12Fence;
		HR = Device->CreateFence(0, FromFenceFlags(FenceFlags), __uuidof(ID3D12Fence), TempD3D12Fence.GetVV());
		if (FAILED(HR))
		{
			LogError(Str( " device->CreateFence failed, {}"), Win32::ErrorStr(HR & 0xFFFF));
			return EError::Inner;
		}
		D3D12Fence = TempD3D12Fence;
		return EError::OK;
	}

	void FRHID3D12Fence::SetName(FStringView Name)
	{
		SetD3D12ObjectName(D3D12Fence.Get(), Name);
	}
	
	uint64_t FRHID3D12Fence::GetValue() const
	{
		return D3D12Fence->GetCompletedValue();
	}

	void FRHID3D12Fence::Signal(uint64_t SignalValue) const
	{
		D3D12Fence->Signal(0);
	}
	
	void FRHID3D12Fence::Wait(uint64_t SignalValue, FSignal * Signal) const
	{
		assert(Signal);
		HRESULT HR = S_OK;
		if (D3D12Fence->GetCompletedValue() < SignalValue)
		{
			HR = D3D12Fence->SetEventOnCompletion(SignalValue, Signal->GetHandle());
			Win32::throw_if_failed(HR);
			WaitForSingleObject(Signal->GetHandle(), INFINITE);
		}
	}
	
	void FRHID3D12Fence::Complete(uint64_t SignalValue, FSignal * Signal) const
	{
		HRESULT HR = D3D12Fence->SetEventOnCompletion(SignalValue, Signal->GetHandle());
	}
}
