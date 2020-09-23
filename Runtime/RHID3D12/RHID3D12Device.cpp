#include "PCH.h"
#include "RHID3D12Device.h"
#include "RHID3D12Fence.h"
#include "RHID3D12CommandQueue.h"
#include "RHID3D12SwapChain.h"
#include "RHID3D12CommandList.h"
#include "RHID3D12Resource.h"
#include "RHID3D12CommandAllocator.h"
#include "RHID3D12PipelineState.h"
#include "RHID3D12DescriptorHeap.h"
#include "RHID3D12DescriptorTable.h"
#include "RHID3D12ResourceView.h"

namespace XE::RHI::D3D12
{
	static FRHIAdapterDesc RHIAdapterDescFromAdapter(TReferPtr<IDXGIAdapter1> DXGIAdapterIn)
	{
		DXGI_ADAPTER_DESC1 DXGIAdapterDesc = {};
		DXGIAdapterIn->GetDesc1(&DXGIAdapterDesc);
		DXGIAdapterIn.Reset();

		FRHIAdapterDesc rhidesc;
		rhidesc.DeviceId = DXGIAdapterDesc.DeviceId;
		rhidesc.Name = Strings::FromWide(DXGIAdapterDesc.Description);
		return rhidesc;
	}

	EError FRHID3D12Device::Create(TReferPtr<IDXGIAdapter1> AdapterIn)
	{
		if (!AdapterIn)
			return EError::Args;
		
		TReferPtr<ID3D12Device> Device;
		HRESULT Result = D3D12CreateDevice(AdapterIn.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), Device.GetVV());
		if (FAILED(Result))
		{
			LogWarning(Str(" D3D12CreateDevice failed: {}"), Win32::ErrorStr(Result & 0xFFFF));
			return EError::Inner;
		}

	//	ComPtr<ID3D12InfoQueue> D3D12InfoQueue = Device.As<ID3D12InfoQueue>();
	//	if (D3D12InfoQueue)
	//	{
	//		D3D12_MESSAGE_SEVERITY Severities[] =
	//		{
	//			D3D12_MESSAGE_SEVERITY_INFO
	//		};

	//		D3D12_MESSAGE_ID DenyIds[] = {
	//			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
	//			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
	//			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
	//		};

	//		D3D12_INFO_QUEUE_FILTER NewFilter = {};
	//		//NewFilter.DenyList.NumCategories = _countof(Categories);
	//		//NewFilter.DenyList.pCategoryList = Categories;
	//		NewFilter.DenyList.NumSeverities = (UINT)std::size(Severities);
	//		NewFilter.DenyList.pSeverityList = Severities;
	//		NewFilter.DenyList.NumIDs = (UINT)std::size(DenyIds);
	//		NewFilter.DenyList.pIDList = DenyIds;

	//		hr = D3D12InfoQueue->PushStorageFilter(&NewFilter);
	//	}
	//	

		TReferPtr<ID3D12DescriptorHeap> ShaderResourceDescriptorHeap;
		{
			D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
			DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			DescriptorHeapDesc.NumDescriptors = (UINT)NumDescriptors[AsIndex(EDescriptorHeapType::ShaderResource)];
			DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), ShaderResourceDescriptorHeap.GetVV());
		}

		TReferPtr<ID3D12DescriptorHeap> RenderTargetDescriptorHeap;
		{
			D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
			DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			DescriptorHeapDesc.NumDescriptors = (UINT)NumDescriptors[AsIndex(EDescriptorHeapType::RenderTarget)];
			DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), RenderTargetDescriptorHeap.GetVV());
		}

		TReferPtr<ID3D12DescriptorHeap> DepthStencilDescriptorHeap;
		{
			D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
			DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			DescriptorHeapDesc.NumDescriptors = (UINT)NumDescriptors[AsIndex(EDescriptorHeapType::DepthStencil)];
			DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), DepthStencilDescriptorHeap.GetVV());
		}

		AdapterDesc = RHIAdapterDescFromAdapter(AdapterIn);
		DXGIAdapter = AdapterIn;
		D3D12Device = Device;

		D3D12DescriptorHeaps[AsIndex(EDescriptorHeapType::ShaderResource)] = ShaderResourceDescriptorHeap;
		D3D12DescriptorHeaps[AsIndex(EDescriptorHeapType::RenderTarget)] = RenderTargetDescriptorHeap;
		D3D12DescriptorHeaps[AsIndex(EDescriptorHeapType::DepthStencil)] = DepthStencilDescriptorHeap;
		
		D3D12DescriptorHeapSRVUseds[AsIndex(EDescriptorHeapType::ShaderResource)].Resize(NumDescriptors[AsIndex(EDescriptorHeapType::ShaderResource)], NumDescriptors[AsIndex(EDescriptorHeapType::ShaderResource)], false);
		D3D12DescriptorHeapSRVUseds[AsIndex(EDescriptorHeapType::RenderTarget)].Resize(NumDescriptors[AsIndex(EDescriptorHeapType::RenderTarget)], NumDescriptors[AsIndex(EDescriptorHeapType::RenderTarget)], false);
		D3D12DescriptorHeapSRVUseds[AsIndex(EDescriptorHeapType::DepthStencil)].Resize(NumDescriptors[AsIndex(EDescriptorHeapType::DepthStencil)], NumDescriptors[AsIndex(EDescriptorHeapType::DepthStencil)], false);
		
		D3D12DescriptorUnitSizes[AsIndex(EDescriptorHeapType::ShaderResource)] = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		D3D12DescriptorUnitSizes[AsIndex(EDescriptorHeapType::RenderTarget)] = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12DescriptorUnitSizes[AsIndex(EDescriptorHeapType::DepthStencil)] = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		D3D12DescriptorUnitSizes[AsIndex(EDescriptorHeapType::Sampler)] = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		return EError::OK;
	}
	
	TSharedPtr<FRHIFence> FRHID3D12Device::CreateFence(EFenceFlags FenceFlags) const
	{
		TSharedPtr<FRHID3D12Fence> Fence = MakeShared<FRHID3D12Fence>(const_cast<FRHID3D12Device *>(this));
		auto Result = Fence->Create(FenceFlags);
		if (!!Result)
			return nullptr;
		return Fence;
	}
	
	TSharedPtr<FRHICommandQueue> FRHID3D12Device::CreateCommandQueue(ECommandType CommandType, ECommandQueueFlags CommandQueueFlags) const
	{
		auto RHICommandQueue = MakeShared<FRHID3D12CommandQueue>(const_cast<FRHID3D12Device *>(this));
		auto Result = RHICommandQueue->Create(CommandType, CommandQueueFlags);
		if (!!Result)
			return nullptr;
		return RHICommandQueue;
	}

	TSharedPtr<FRHICommandList> FRHID3D12Device::CreateCommandList(ECommandType CommandType) const
	{
		auto RHICommandList = MakeShared<FRHID3D12CommandList>(const_cast<FRHID3D12Device *>(this));
		auto Result = RHICommandList->Create(CommandType);
		if (!!Result)
			return nullptr;
		return RHICommandList;
	}

	TSharedPtr<FRHIResource> FRHID3D12Device::CreateResource(const FResourceDesc & ResourceArgs) const
	{
		auto RHIResource = MakeShared<FRHID3D12Resource>(const_cast<FRHID3D12Device *>(this));
		auto Result = RHIResource->Create(ResourceArgs);
		if (!!Result)
			return nullptr;
		return RHIResource;
	}

	TSharedPtr<FRHIDescriptorHeap> FRHID3D12Device::CreateDescriptorHeap(const FDescriptorHeapDesc & DescriptorHeapArgs) const
	{
		auto RHID3D12DescriptorHeap = MakeShared<FRHID3D12DescriptorHeap>(const_cast<FRHID3D12Device *>(this));
		auto Result = RHID3D12DescriptorHeap->Create(DescriptorHeapArgs);
		if (!!Result)
			return nullptr;
		return RHID3D12DescriptorHeap;
	}
	
	TSharedPtr<FRHIDescriptorTable> FRHID3D12Device::CreateDescriptorTable(FRHIDescriptorHeap * RHIDescriptorHeap, uint32_t NumDescriptors) const
	{
		auto RHID3D12DescriptorTable = MakeShared<FRHID3D12DescriptorTable>(const_cast<FRHID3D12Device *>(this), static_cast<FRHID3D12DescriptorHeap*>(RHIDescriptorHeap));
		auto Result = RHID3D12DescriptorTable->Create(NumDescriptors);
		if (!!Result)
			return nullptr;
		return RHID3D12DescriptorTable;
	}
	
	TSharedPtr<FRHISwapChain> FRHID3D12Device::CreateSwapChain(FRHICommandQueue * RHICommandQueue, const FSwapChainDesc & SwapChainArgs) const
	{
		auto RHIRenderTarget = MakeShared<FRHID3D12SwapChain>(const_cast<FRHID3D12Device *>(this));
		auto Result = RHIRenderTarget->Create(RHICommandQueue, SwapChainArgs);
		if (!!Result)
			return nullptr;

		return RHIRenderTarget;
	}

	TSharedPtr<FRHIRootSignature> FRHID3D12Device::CreateRootSignature(const FRootSignatureDesc & RootSignatureArgs) const
	{
		auto RHIRootSignature = MakeShared<FRHID3D12RootSignature>(const_cast<FRHID3D12Device *>(this));
		auto Result = RHIRootSignature->Create(RootSignatureArgs);
		if (!!Result)
			return nullptr;

		return RHIRootSignature;
	}
	
	TSharedPtr<FRHIPipelineState> FRHID3D12Device::CreateGraphicsPipelineState(FRHIRootSignature * RHIRootSignature, const FGraphicsPipelistStateDesc & GraphicsPipelistStateArgs) const
	{
		auto RHIPipelineState = MakeShared<FRHID3D12GraphicsPipelineState>(const_cast<FRHID3D12Device *>(this));
		auto Result = RHIPipelineState->Create(RHIRootSignature, GraphicsPipelistStateArgs);
		if (!!Result)
			return nullptr;

		return RHIPipelineState;
	}

	TSharedPtr<FRHIPipelineState> FRHID3D12Device::CreateComputePipelineState(FRHIRootSignature * RHIRootSignature, const FComputePipelineStateDesc & ComputePipelineState) const
	{
		auto RHIPipelineState = MakeShared<FRHID3D12ComputePipelineState>(const_cast<FRHID3D12Device *>(this));
		auto Result = RHIPipelineState->Create(RHIRootSignature, ComputePipelineState);
		if (!!Result)
			return nullptr;

		return RHIPipelineState;
	}

	void FRHID3D12Device::GetCopyableFootprints(FRHIResource * RHIResourceDst, uint32_t SubResourceIndex, uint32_t SubResourceCount, FCopyableFootprint * CopyableFootprints) const
	{
		ID3D12Resource * RHID3D12ResourceDst = static_cast<FRHID3D12Resource *>(RHIResourceDst)->Resource();
		if (!RHID3D12ResourceDst)
			return;

		D3D12_RESOURCE_DESC DescResourceDesc = RHID3D12ResourceDst->GetDesc();

		const uint32_t SubResourceMax = 32;

		UINT64 BaseOffset = 0;
		UINT64 RequiredSize = 0;
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT SubResourceFootprints[SubResourceMax];
		UINT NumRows[SubResourceMax];
		UINT64 RowSizesInBytes[SubResourceMax];

		ID3D12Device * pDevice = nullptr;
		RHID3D12ResourceDst->GetDevice(__uuidof(ID3D12Device), reinterpret_cast<void **>(&pDevice));
		pDevice->GetCopyableFootprints(&DescResourceDesc, SubResourceIndex, SubResourceCount, BaseOffset, SubResourceFootprints, NumRows, RowSizesInBytes, &RequiredSize);
		pDevice->Release();

		for (uint32_t Index = 0; Index < SubResourceCount; ++Index)
		{
			CopyableFootprints[Index].NumRows = NumRows[Index];

			CopyableFootprints[Index].Offset = uint32_t(SubResourceFootprints[Index].Offset);
			CopyableFootprints[Index].AlignMask = 0x1ff;
			
			CopyableFootprints[Index].Format = ToFormat(SubResourceFootprints[Index].Footprint.Format);
			CopyableFootprints[Index].Width = SubResourceFootprints[Index].Footprint.Width;
			CopyableFootprints[Index].Height = SubResourceFootprints[Index].Footprint.Height;
			CopyableFootprints[Index].Depth = SubResourceFootprints[Index].Footprint.Depth;
			CopyableFootprints[Index].RowPitch = SubResourceFootprints[Index].Footprint.RowPitch;
		}
	}
	
	FCopyableFootprint FRHID3D12Device::GetCopyableFootprint(FRHIResource * RHIResourceDst) const
	{
		FCopyableFootprint CopyableFootprint = {};
		GetCopyableFootprints(RHIResourceDst, 0, 1, &CopyableFootprint);
		return CopyableFootprint;
	}

	FAddress FRHID3D12Device::AllocateDescriptor(EDescriptorHeapType DescriptorHeapType, size_t Count)
	{
		size_t DescriptorIndex = NullIndex;
		for (size_t Index = 0; Index < D3D12DescriptorHeapSRVUseds[AsIndex(DescriptorHeapType)].Size; ++Index)
		{
			bool Good = true;
			for (size_t SubIndex = 0; SubIndex < Count; ++SubIndex)
			{
				if (D3D12DescriptorHeapSRVUseds[AsIndex(DescriptorHeapType)][Index + SubIndex])
				{
					Good = false;
					break;
				}
			}

			if (Good)
			{
				DescriptorIndex = Index;
				break;
			}
		}
		
		if (DescriptorIndex != NullIndex)
		{
			D3D12DescriptorHeapSRVUseds[AsIndex(DescriptorHeapType)].SetAt(DescriptorIndex, Count, true);
			return
			{
				FCPUAddress(D3D12DescriptorHeaps[AsIndex(DescriptorHeapType)]->GetCPUDescriptorHandleForHeapStart().ptr + DescriptorIndex * D3D12DescriptorUnitSizes[AsIndex(DescriptorHeapType)]),
				FGPUAddress(D3D12DescriptorHeaps[AsIndex(DescriptorHeapType)]->GetGPUDescriptorHandleForHeapStart().ptr + DescriptorIndex * D3D12DescriptorUnitSizes[AsIndex(DescriptorHeapType)]),
				D3D12DescriptorUnitSizes[AsIndex(DescriptorHeapType)],
			};
		}
		
		return {};
	}
	
	void FRHID3D12Device::BindDescriptor(FRHIResource * RHIResourceIn, FCPUAddress CPUAddress, const FResourceViewArgs & ResourceViewArgsIn)
	{
		if (!RHIResourceIn)
			return;
		
		if (ResourceViewArgsIn.Type == EResourceType::ConstBuffer)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferViewDesc = {};
			ConstantBufferViewDesc.BufferLocation = static_cast<FRHID3D12Resource *>(RHIResourceIn)->GetD3D12GPUVirtualAddress();
			ConstantBufferViewDesc.SizeInBytes = (RHIResourceIn->GetSize().Width + 255) & ~255;
			//viewDesc.SizeInBytes = resource->Size().cx;

			D3D12Device->CreateConstantBufferView(&ConstantBufferViewDesc, D3D12_CPU_DESCRIPTOR_HANDLE{ CPUAddress });
		}
		else if (ResourceViewArgsIn.Type == EResourceType::ShaderResource)
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc = {};
			ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			ShaderResourceViewDesc.ViewDimension = FromResourceViewDimension(ResourceViewArgsIn.Resource.ViewDimension, RHIResourceIn->GetResourceArgs().MSAA.Level > 1);
			if (ResourceViewArgsIn.Resource.ViewDimension == EResourceViewDimension::Buffer)
			{
				ShaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
				ShaderResourceViewDesc.Buffer.FirstElement = ResourceViewArgsIn.Resource.Buffer.FirstElement;
				ShaderResourceViewDesc.Buffer.NumElements = ResourceViewArgsIn.Resource.Buffer.NumElements;
				ShaderResourceViewDesc.Buffer.StructureByteStride = ResourceViewArgsIn.Resource.Buffer.Stride;
				ShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			}
			else if (ResourceViewArgsIn.Resource.ViewDimension == EResourceViewDimension::Texture2D)
			{
				ShaderResourceViewDesc.Format = FromFormat(ResourceViewArgsIn.Resource.Format);
				ShaderResourceViewDesc.Texture2D.MipLevels = 1;
			}
			else
			{

			}
			D3D12Device->CreateShaderResourceView(static_cast<FRHID3D12Resource *>(RHIResourceIn)->Resource(), &ShaderResourceViewDesc, D3D12_CPU_DESCRIPTOR_HANDLE{ CPUAddress });
		}
		else if (ResourceViewArgsIn.Type == EResourceType::UnorderedAccess)
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessViewDesc = {};
			UnorderedAccessViewDesc.Format = /*FromFormat(args.resource.format)*/DXGI_FORMAT_UNKNOWN;
			UnorderedAccessViewDesc.ViewDimension = FromResourceViewDimension_UAV(ResourceViewArgsIn.Resource.ViewDimension);
			if (ResourceViewArgsIn.Resource.ViewDimension == EResourceViewDimension::Buffer)
			{
				UnorderedAccessViewDesc.Buffer.FirstElement = ResourceViewArgsIn.Resource.Buffer.FirstElement;
				UnorderedAccessViewDesc.Buffer.NumElements = ResourceViewArgsIn.Resource.Buffer.NumElements;
				UnorderedAccessViewDesc.Buffer.StructureByteStride = ResourceViewArgsIn.Resource.Buffer.Stride;
				UnorderedAccessViewDesc.Buffer.CounterOffsetInBytes = 0;
				UnorderedAccessViewDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
			}
			else if (ResourceViewArgsIn.Resource.ViewDimension == EResourceViewDimension::Texture2D)
			{
				UnorderedAccessViewDesc.Texture2D.MipSlice = 0;
				UnorderedAccessViewDesc.Texture2D.PlaneSlice = 0;
			}
			else
			{

			}

			D3D12Device->CreateUnorderedAccessView(static_cast<FRHID3D12Resource *>(RHIResourceIn)->Resource(), nullptr, &UnorderedAccessViewDesc, D3D12_CPU_DESCRIPTOR_HANDLE{ CPUAddress });
		}
		else if (ResourceViewArgsIn.Type == EResourceType::RenderTarget)
		{
			D3D12_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc = {};
			RenderTargetViewDesc.ViewDimension = FromResourceViewDimension_RenderTargetView(ResourceViewArgsIn.Resource.ViewDimension, RHIResourceIn->GetResourceArgs().MSAA.Level > 1);
			if (ResourceViewArgsIn.Resource.ViewDimension == EResourceViewDimension::Buffer)
			{
				RenderTargetViewDesc.Format = DXGI_FORMAT_UNKNOWN;
				RenderTargetViewDesc.Buffer.FirstElement = ResourceViewArgsIn.Resource.Buffer.FirstElement;
				RenderTargetViewDesc.Buffer.NumElements = ResourceViewArgsIn.Resource.Buffer.NumElements;
			}
			else if (ResourceViewArgsIn.Resource.ViewDimension == EResourceViewDimension::Texture2D)
			{
				RenderTargetViewDesc.Format = FromFormat(ResourceViewArgsIn.Resource.Format);
				RenderTargetViewDesc.Texture2D.MipSlice = 0;
				RenderTargetViewDesc.Texture2D.PlaneSlice = 0;
			}
			else
			{

			}
			D3D12Device->CreateRenderTargetView(static_cast<FRHID3D12Resource *>(RHIResourceIn)->Resource(), &RenderTargetViewDesc, D3D12_CPU_DESCRIPTOR_HANDLE{ CPUAddress });
		}
		else if (ResourceViewArgsIn.Type == EResourceType::DepthStencil)
		{
			D3D12_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
			DepthStencilViewDesc.ViewDimension = FromResourceViewDimension_DepthStencilView(ResourceViewArgsIn.Resource.ViewDimension, RHIResourceIn->GetResourceArgs().MSAA.Level > 1);
			if (ResourceViewArgsIn.Resource.ViewDimension == EResourceViewDimension::Texture2D)
			{
				DepthStencilViewDesc.Format = FromFormat(ResourceViewArgsIn.Resource.Format);
				DepthStencilViewDesc.Texture2D.MipSlice = 0;
			}
			else
			{
				throw EError::BadFormat;
			}
			D3D12Device->CreateDepthStencilView(static_cast<FRHID3D12Resource *>(RHIResourceIn)->Resource(), &DepthStencilViewDesc, D3D12_CPU_DESCRIPTOR_HANDLE{ CPUAddress });
		}
		else
		{
			throw EError::BadFormat;
		}
	}
	
	TSharedPtr<FRHIResourceView> FRHID3D12Device::CreateResourceView(FRHIResource * RHIResource, const FResourceViewArgs & ResourceViewArgs)
	{
		auto RHIPipelineState = MakeShared<FRHID3D12ResourceView>(const_cast<FRHID3D12Device *>(this));
		auto Result = RHIPipelineState->Create(RHIResource, ResourceViewArgs);
		if (!!Result)
			return nullptr;

		return RHIPipelineState;
		
	}
	
	FAddress FRHID3D12Device::AllocateDescriptor(EDescriptorHeapType DescriptorHeapType)
	{
		size_t DescriptorIndex = NullIndex;
		for (size_t Index = 0; Index < D3D12DescriptorHeapSRVUseds[AsIndex(DescriptorHeapType)].Size; ++Index)
		{
			if (!D3D12DescriptorHeapSRVUseds[AsIndex(DescriptorHeapType)][Index])
			{
				DescriptorIndex = Index;
				break;
			}
		}

		if (DescriptorIndex != NullIndex)
		{
			return
			{
				FCPUAddress(D3D12DescriptorHeaps[AsIndex(DescriptorHeapType)]->GetCPUDescriptorHandleForHeapStart().ptr + DescriptorIndex * D3D12DescriptorUnitSizes[AsIndex(DescriptorHeapType)]),
				FGPUAddress(D3D12DescriptorHeaps[AsIndex(DescriptorHeapType)]->GetGPUDescriptorHandleForHeapStart().ptr + DescriptorIndex * D3D12DescriptorUnitSizes[AsIndex(DescriptorHeapType)]),
			};
		}
		return {};
	}
}
