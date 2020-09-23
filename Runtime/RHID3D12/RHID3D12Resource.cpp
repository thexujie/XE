#include "PCH.h"
#include "RHID3D12Resource.h"
#include "RHID3D12CommandList.h"

namespace XE::RHI::D3D12
{
	FRHID3D12Resource::FRHID3D12Resource(FRHID3D12Device * RHID3D12DeviceIn, TReferPtr<ID3D12Resource> D3D12ResourceIn) : RHID3D12Device(RHID3D12DeviceIn), D3D12Resource(D3D12ResourceIn)
	{
		if (D3D12ResourceIn)
		{
			D3D12_RESOURCE_DESC D3D12ResourceDesc = D3D12ResourceIn->GetDesc();
			ResourceArgs.Dimension = ToResourceDimension(D3D12ResourceDesc.Dimension);
			ResourceArgs.Alignment = uint32_t(D3D12ResourceDesc.Alignment);
			ResourceArgs.Size.Width = uint32_t(D3D12ResourceDesc.Width);
			ResourceArgs.Size.Height = uint32_t(D3D12ResourceDesc.Height);
			ResourceArgs.Depth = D3D12ResourceDesc.DepthOrArraySize;
			ResourceArgs.MipLevels = D3D12ResourceDesc.MipLevels;
			ResourceArgs.Format = ToFormat(D3D12ResourceDesc.Format);
			ResourceArgs.MSAA.Level = D3D12ResourceDesc.SampleDesc.Count;
			ResourceArgs.MSAA.Quality = D3D12ResourceDesc.SampleDesc.Quality;

			D3D12_HEAP_PROPERTIES HeapProperties;
			D3D12_HEAP_FLAGS HeapFlags;
			D3D12ResourceIn->GetHeapProperties(&HeapProperties, &HeapFlags);
			
			ResourceArgs.Heap.HeapType = ToHeapType(HeapProperties.Type);

			void * TempRawPointer = nullptr;
			if (ResourceArgs.Heap.HeapType == EHeapType::Upload)
			{
				D3D12_RANGE range = {};
				D3D12ResourceIn->Map(0, &range, &TempRawPointer);
			}
		}
	}
	
	EError FRHID3D12Resource::Create(const FResourceDesc & ResourceArgsIn)
	{
		if (!RHID3D12Device)
			return EError::State;

		HRESULT HR = S_OK;

		auto Device = RHID3D12Device->GetDevice();
		auto Adapter = RHID3D12Device->GetAdapter();
		assert(Device);
		assert(Adapter);

		D3D12_RESOURCE_DESC D3D12ResourceDesc = {};
		D3D12ResourceDesc.Dimension = FromResourceDimension(ResourceArgsIn.Dimension);
		D3D12ResourceDesc.Alignment = ResourceArgsIn.Alignment;
		D3D12ResourceDesc.Width = ResourceArgsIn.Size.Width;
		D3D12ResourceDesc.Height = ResourceArgsIn.Size.Height;
		D3D12ResourceDesc.DepthOrArraySize = ResourceArgsIn.Depth;
		D3D12ResourceDesc.MipLevels = ResourceArgsIn.MipLevels;
		D3D12ResourceDesc.Format = FromFormat(ResourceArgsIn.Format);
		D3D12ResourceDesc.SampleDesc.Count = ResourceArgsIn.MSAA.Level;
		D3D12ResourceDesc.SampleDesc.Quality = ResourceArgsIn.MSAA.Quality;
		D3D12ResourceDesc.Layout = ResourceArgsIn.Dimension == EResourceDimension::Buffer ? D3D12_TEXTURE_LAYOUT_ROW_MAJOR : D3D12_TEXTURE_LAYOUT_UNKNOWN;
		D3D12ResourceDesc.Flags = FromResourceUsages(ResourceArgsIn.Usages);
		
		D3D12_HEAP_PROPERTIES D3D12HeapProperties = {};
		D3D12HeapProperties.Type = FromHeapType(ResourceArgsIn.Heap.HeapType);
		D3D12HeapProperties.CPUPageProperty = FromCPUPageProperty(ResourceArgsIn.Heap.CPUPageProperty);
		D3D12HeapProperties.MemoryPoolPreference = FromMemoryPool(ResourceArgsIn.Heap.MemoryPool);
		D3D12HeapProperties.CreationNodeMask = 1;
		D3D12HeapProperties.VisibleNodeMask = 1;

		D3D12_CLEAR_VALUE D3D12ClearValue = {};
		D3D12_CLEAR_VALUE * D3D12ClearValuePointer = nullptr;
		if (ResourceArgsIn.Dimension == EResourceDimension::Texture2D)
		{
			D3D12ClearValue.Format = D3D12ResourceDesc.Format;
			if (ResourceArgsIn.Usages.Any(EResourceUsage::RenderTarget))
			{
				D3D12ClearValue.Color[0] = ResourceArgsIn.ClearColor.R;
				D3D12ClearValue.Color[1] = ResourceArgsIn.ClearColor.G;
				D3D12ClearValue.Color[2] = ResourceArgsIn.ClearColor.B;
				D3D12ClearValue.Color[3] = ResourceArgsIn.ClearColor.A;
				D3D12ClearValuePointer = &D3D12ClearValue;
			}
			else if (ResourceArgsIn.Usages == EResourceUsage::DepthStencial)
			{
				D3D12ClearValue.DepthStencil.Depth = ResourceArgsIn.ClearDepth;
				D3D12ClearValue.DepthStencil.Stencil = ResourceArgsIn.CleanStencial;
				D3D12ClearValuePointer = &D3D12ClearValue;
			}
			else {}
		}
		TReferPtr<ID3D12Resource> TempD3D12Resource;
		HR = Device->CreateCommittedResource(&D3D12HeapProperties, 
			FromHeapFlags(ResourceArgsIn.Heap.HeapFlags), 
			&D3D12ResourceDesc, 
			FromResourceStates(ResourceArgsIn.States), 
			D3D12ClearValuePointer,
			__uuidof(ID3D12Resource), TempD3D12Resource.GetVV());
		if (FAILED(HR))
		{
			LogError(Str( " device->CreateCommittedResource failed, {}"), Win32::ErrorStr(HR & 0xFFFF));
			return EError::Inner;
		}

		void * TempRawPointer = nullptr;
		if (ResourceArgsIn.Heap.HeapType == EHeapType::Upload)
		{
			D3D12_RANGE range = {};
			TempD3D12Resource->Map(0, &range, &TempRawPointer);
		}

		static uint64_t ResourceIndex = 0;
		SetD3D12ObjectName(TempD3D12Resource.Get(), Strings::Printf(Str("FRHID3D12Resource %d"), ResourceIndex++));

		ResourceArgs = ResourceArgsIn;
		D3D12Resource = TempD3D12Resource;
		RawPointer = TempRawPointer;
		ResourceStates = ResourceArgsIn.States;
		return EError::OK;
	}

	void FRHID3D12Resource::SetName(FStringView Name)
	{
		SetD3D12ObjectName(D3D12Resource.Get(), Name);
	}
	
	void * FRHID3D12Resource::GetData()
	{
		return RawPointer;
	}
	
	const void * FRHID3D12Resource::GetData() const
	{
		return RawPointer;
	}
	
	SizeU FRHID3D12Resource::GetSize() const
	{
		return ResourceArgs.Size;
	}
}
