#include "PCH.h"
#include "RHID3D12CommandQueue.h"
#include "RHID3D12CommandList.h"
#include "RHID3D12Fence.h"

namespace XE::RHI::D3D12
{
	EError FRHID3D12CommandQueue::Create(ECommandType CommandTypeIn, ECommandQueueFlags CommandQueueFlagsIn)
	{
		if (!RHID3D12Device)
			return EError::State;

		D3D12_COMMAND_QUEUE_DESC CommandQueueDesc = {};
		if (CommandQueueFlagsIn * ECommandQueueFlags::DisableGPUTimeout)
			CommandQueueDesc.Flags |= D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;

		switch (CommandTypeIn)
		{
		case ECommandType::Direct:
			CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			break;
		case ECommandType::Bundle:
			CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_BUNDLE;
			break;
		case ECommandType::Compute:
			CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			break;
		case ECommandType::Copy:
			CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
			break;
		default:
			CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			break;
		}

		auto Device = RHID3D12Device->GetDevice();
		assert(Device);
		TReferPtr<ID3D12CommandQueue> TempD3D12CommandQueue;
		HRESULT Result = Device->CreateCommandQueue(&CommandQueueDesc, __uuidof(ID3D12CommandQueue), TempD3D12CommandQueue.GetVV());
		if (FAILED(Result))
		{
			LogWarning(Str(" CreateCommandQueue failed: {}"),Win32::ErrorStr(Result & 0xFFFF));
			return EError::Inner;
		}

		D3D12CommandQueue = TempD3D12CommandQueue;
		CommandType = CommandTypeIn;
		CommandQueueFlags = CommandQueueFlagsIn;
		return EError::OK;
	}

	void FRHID3D12CommandQueue::SetName(FStringView Name)
	{
		SetD3D12ObjectName(D3D12CommandQueue.Get(), Name);
	}
	
	void FRHID3D12CommandQueue::ExcuteCommandList(FRHICommandList * RHICommandList)
	{
		ID3D12CommandList * D3D12CommandLists[] = { reinterpret_cast<FRHID3D12CommandList *>(RHICommandList)->Ptr() };
		D3D12CommandQueue->ExecuteCommandLists(1, D3D12CommandLists);
	}

	void FRHID3D12CommandQueue::Signal(FRHIFence * RHIFence, uint64_t SignalValue)
	{
		auto RHID3D12Fence = static_cast<FRHID3D12Fence *>(RHIFence);
		HRESULT HR = S_OK;
		HR = D3D12CommandQueue->Signal(RHID3D12Fence->Fence(), SignalValue);
		Win32::throw_if_failed(HR);
	}

	void FRHID3D12CommandQueue::Fence(FRHIFence * RHIFence, uint64_t SignalValue)
	{
		auto RHID3D12Fence = static_cast<FRHID3D12Fence *>(RHIFence);
		HRESULT HR = S_OK;
		HR = D3D12CommandQueue->Wait(RHID3D12Fence->Fence(), SignalValue);
		Win32::throw_if_failed(HR);
	}
}
