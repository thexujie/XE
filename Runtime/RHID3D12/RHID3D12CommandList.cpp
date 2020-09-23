#include "PCH.h"
#include "RHID3D12CommandList.h"
#include "RHID3D12CommandAllocator.h"
#include "RHID3D12Resource.h"
#include "RHID3D12PipelineState.h"
#include "RHID3D12DescriptorHeap.h"
#include "RHID3D12DescriptorTable.h"
#include "RHID3D12ResourceView.h"

namespace XE::RHI::D3D12
{
	EError FRHID3D12CommandList::Create(ECommandType CommandTypeIn)
	{
		if (!RHID3D12Device)
			return EError::State;

		HRESULT HR = S_OK;

		auto Device = RHID3D12Device->GetDevice();
		auto Adapter = RHID3D12Device->GetAdapter();

		TReferPtr<ID3D12CommandAllocator> TempCommandAllocator;
		HR = Device->CreateCommandAllocator(FromCommandType(CommandTypeIn), __uuidof(ID3D12CommandAllocator), TempCommandAllocator.GetVV());
		if (FAILED(HR))
		{
			LogWarning(Str(" device->CreateCommandAllocator failed: {}"), Win32::ErrorStr(HR & 0xFFFF));
			return EError::Inner;
		}
		
		assert(Device);
		assert(Adapter);

		TReferPtr<ID3D12GraphicsCommandList> TempD3D12GraphicsCommandList;
		HR = Device->CreateCommandList(0, FromCommandType(CommandTypeIn), TempCommandAllocator.Get(), nullptr, __uuidof(ID3D12GraphicsCommandList), TempD3D12GraphicsCommandList.GetVV());
		if (FAILED(HR))
		{
			LogWarning(Str(" device->CreateCommandList failed: {}"), Win32::ErrorStr(HR & 0xFFFF));
			return EError::Inner;
		}

		TempD3D12GraphicsCommandList->Close();

		CommandType = CommandTypeIn;
		D3D12CommandAllocator = TempCommandAllocator;
		D3D12CommandList = TempD3D12GraphicsCommandList;
		return EError::OK;
	}

	void FRHID3D12CommandList::SetName(FStringView Name)
	{
		SetD3D12ObjectName(D3D12CommandList.Get(), Name);
	}

	void FRHID3D12CommandList::Reset()
	{
		D3D12CommandAllocator->Reset();
	}
	
	void FRHID3D12CommandList::Open()
	{
		D3D12CommandList->Reset(D3D12CommandAllocator.Get(), nullptr);

		auto & D3D12DescriptorHeapArray = RHID3D12Device->GetD3D12DescriptorHeaps();
		ID3D12DescriptorHeap * D3D12DescriptorHeaps[1] =
		{
			D3D12DescriptorHeapArray[AsIndex(EDescriptorHeapType::ShaderResource)].Get()
		};
		D3D12CommandList->SetDescriptorHeaps(1, D3D12DescriptorHeaps);
	}

	void FRHID3D12CommandList::Close()
	{
		D3D12CommandList->Close();
	}

	void FRHID3D12CommandList::SetRootSignature(FRHIRootSignature * RootSignature)
	{
		FRHID3D12RootSignature * RHID3D12RootSignature = static_cast<FRHID3D12RootSignature *>(RootSignature);
		D3D12CommandList->SetGraphicsRootSignature(RHID3D12RootSignature->RootSignature());
	}

	void FRHID3D12CommandList::SetPipelineState(FRHIPipelineState * PipelineState)
	{
		FRHID3D12PipelineState * RHID3D12PipelineState = static_cast<FRHID3D12PipelineState *>(PipelineState);
		D3D12CommandList->SetPipelineState(RHID3D12PipelineState->PipelineState());
	}

	//void FRHID3D12CommandList::SetDescriptorHeaps(FRHIDescriptorHeap * ShaderResourceRHIDescriptorHeap, FRHIDescriptorHeap * SamplerRHIDescriptorHeap)
	//{
	//	if (ShaderResourceRHIDescriptorHeap && SamplerRHIDescriptorHeap)
	//	{
	//		ID3D12DescriptorHeap * D3D12DescriptorHeaps[2] =
	//		{
	//			static_cast<FRHID3D12DescriptorHeap *>(ShaderResourceRHIDescriptorHeap)->GetD3D12DescriptorHeap(),
	//			static_cast<FRHID3D12DescriptorHeap *>(SamplerRHIDescriptorHeap)->GetD3D12DescriptorHeap()
	//		};
	//		D3D12CommandList->SetDescriptorHeaps(2, D3D12DescriptorHeaps);
	//	}
	//	else if (ShaderResourceRHIDescriptorHeap)
	//	{
	//		ID3D12DescriptorHeap * D3D12DescriptorHeap = static_cast<FRHID3D12DescriptorHeap *>(ShaderResourceRHIDescriptorHeap)->GetD3D12DescriptorHeap();
	//		D3D12CommandList->SetDescriptorHeaps(1, &D3D12DescriptorHeap);
	//	}
	//	else if (SamplerRHIDescriptorHeap)
	//	{
	//		ID3D12DescriptorHeap * D3D12DescriptorHeap = static_cast<FRHID3D12DescriptorHeap *>(SamplerRHIDescriptorHeap)->GetD3D12DescriptorHeap();
	//		D3D12CommandList->SetDescriptorHeaps(1, &D3D12DescriptorHeap);
	//	}
	//	else
	//	{
	//		D3D12CommandList->SetDescriptorHeaps(0, nullptr);
	//	}
	//}
	
	void FRHID3D12CommandList::SetRootConstBuffer(size_t RootParameterIndex, FRHIResource * RHIResource)
	{
		if (!RHIResource)
			return;

		D3D12CommandList->SetGraphicsRootConstantBufferView(UINT(RootParameterIndex), RHIResource->GetGPUAddress());
	}
	
	void FRHID3D12CommandList::SetRootShaderResource(size_t RootParameterIndex, FRHIResource * RHIResource)
	{
		if (!RHIResource)
			return;

		D3D12CommandList->SetGraphicsRootShaderResourceView(UINT(RootParameterIndex), RHIResource->GetGPUAddress());
	}
	
	void FRHID3D12CommandList::SetRootUnorderedResource(size_t RootParameterIndex, FRHIResource * RHIResource)
	{
		if (!RHIResource)
			return;

		D3D12CommandList->SetGraphicsRootUnorderedAccessView(UINT(RootParameterIndex), RHIResource->GetGPUAddress());
	}

	void FRHID3D12CommandList::SetRootDescriptorTable(size_t RootParameterIndex, FRHIResourceView * ResourceView)
	{
		if (!ResourceView)
			return;
		
		D3D12CommandList->SetGraphicsRootDescriptorTable(UINT(RootParameterIndex), D3D12_GPU_DESCRIPTOR_HANDLE{ ResourceView->GetGPUAddress() });
	}

	void FRHID3D12CommandList::SetRootDescriptorTable(size_t RootParameterIndex, FGPUAddress GPUAddress)
	{
		if (!GPUAddress)
			return;

		D3D12CommandList->SetGraphicsRootDescriptorTable(UINT(RootParameterIndex), D3D12_GPU_DESCRIPTOR_HANDLE{ GPUAddress });
	}
	
	void
		FRHID3D12CommandList::SetRenderTarget(TView<FRHIResourceView *> RenderTargetViews, FRHIResourceView * DepthStencilView)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE RTCPUDescriptorHandles[8];
		size_t NumRenderTargets = Min<size_t>(Size(RTCPUDescriptorHandles), RenderTargetViews.Size);
		for (size_t Index = 0; Index < NumRenderTargets; ++Index)
			RTCPUDescriptorHandles[Index].ptr = RenderTargetViews[Index]->GetCPUAddress();

		if (DepthStencilView)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilDescriptorHandle;
			DepthStencilDescriptorHandle.ptr = DepthStencilView->GetCPUAddress();
			D3D12CommandList->OMSetRenderTargets(UINT(NumRenderTargets), RTCPUDescriptorHandles, FALSE, &DepthStencilDescriptorHandle);
		}
		else
			D3D12CommandList->OMSetRenderTargets(UINT(NumRenderTargets), RTCPUDescriptorHandles, FALSE, nullptr);
	}

	void FRHID3D12CommandList::ClearDepthStencil(FRHIResourceView * DepthStencilView, float32_t Depth, uint8_t Stencial)
	{
		D3D12CommandList->ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE{ DepthStencilView->GetCPUAddress() }, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, Depth, Stencial, 0, nullptr);
	}

	void FRHID3D12CommandList::ClearRenderTarget(FRHIResourceView * RenderTargetView, FColor Color)
	{
		D3D12CommandList->ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE{ RenderTargetView->GetCPUAddress() }, Color.Values, 0, nullptr);
	}

	void FRHID3D12CommandList::SetViewPort(const FViewPort & ViewPort)
	{
		D3D12_VIEWPORT D3D12ViewPort = {};
		D3D12ViewPort.TopLeftX = ViewPort.Left;
		D3D12ViewPort.TopLeftY = ViewPort.Top;
		D3D12ViewPort.Width = ViewPort.Width;
		D3D12ViewPort.Height = ViewPort.Height;
		D3D12ViewPort.MinDepth = ViewPort.NearZ;
		D3D12ViewPort.MaxDepth = ViewPort.FarZ;

		D3D12CommandList->RSSetViewports(1, &D3D12ViewPort);
	}

	void FRHID3D12CommandList::SetScissorRect(const RectI & ScissorRect)
	{
		D3D12_RECT d3d12rect = {};
		d3d12rect.left = ScissorRect.X;
		d3d12rect.top = ScissorRect.Y;
		d3d12rect.right = ScissorRect.X + ScissorRect.Width;
		d3d12rect.bottom = ScissorRect.Y + ScissorRect.Height;

		D3D12CommandList->RSSetScissorRects(1, &d3d12rect);
	}

	void FRHID3D12CommandList::TransitionBarrier(FRHIResource * RHIResource, EResourceStates ResourceStates)
	{
		FRHID3D12Resource * RHID3D12Resource = static_cast<FRHID3D12Resource *>(RHIResource);

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		ResourceBarrier.Transition.pResource = RHID3D12Resource->Resource();
		ResourceBarrier.Transition.StateBefore = FromResourceStates(RHID3D12Resource->GetResourceStates());
		ResourceBarrier.Transition.StateAfter = FromResourceStates(ResourceStates);
		ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		if (ResourceBarrier.Transition.StateBefore == ResourceBarrier.Transition.StateAfter)
			return;
		RHID3D12Resource->SetResourceStates(ResourceStates);
		//if ((ResourceBarrier.Transition.StateBefore & ResourceBarrier.Transition.StateAfter) != 0)
		//{
		//	Warning() << __FUNCTION__ " RESOURCE_MANIPULATION";
		//	return;
		//}
		D3D12CommandList->ResourceBarrier(1, &ResourceBarrier);
	}

	void FRHID3D12CommandList::UnorderedBarrier(FRHIResource * RHIResource)
	{
		FRHID3D12Resource * RHID3D12Resource = static_cast<FRHID3D12Resource *>(RHIResource);

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		ResourceBarrier.UAV.pResource = RHID3D12Resource->Resource();
		D3D12CommandList->ResourceBarrier(1, &ResourceBarrier);
	}

	void FRHID3D12CommandList::IASetVertexBuffer(FRHIResource * RHIResource, size_t Stride, size_t VertexCount)
	{
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView = {};
		VertexBufferView.BufferLocation = static_cast<FRHID3D12Resource *>(RHIResource)->GetD3D12GPUVirtualAddress();
		VertexBufferView.StrideInBytes = UINT(Stride);
		VertexBufferView.SizeInBytes = UINT(Stride * VertexCount);
		D3D12CommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
	}
	
	void FRHID3D12CommandList::IASetIndexBuffer(FRHIResource * RHIResource, size_t Stride, size_t IndexCount)
	{
		D3D12_INDEX_BUFFER_VIEW IndexBufferView = {};
		IndexBufferView.BufferLocation = static_cast<FRHID3D12Resource *>(RHIResource)->GetD3D12GPUVirtualAddress();
		IndexBufferView.Format = Stride == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		IndexBufferView.SizeInBytes = UINT(Stride * IndexCount);
		D3D12CommandList->IASetIndexBuffer(&IndexBufferView);
	}

	void FRHID3D12CommandList::IASetTopology(ETopology Topology)
	{
		D3D12CommandList->IASetPrimitiveTopology(FromTopology(Topology));
	}

	void FRHID3D12CommandList::DrawInstanced(size_t NumVertices, size_t NumInstances, size_t VertexIndexBase, size_t InstanceIndexBase)
	{
		D3D12CommandList->DrawInstanced(UINT(NumVertices), UINT(NumInstances), UINT(VertexIndexBase), UINT(InstanceIndexBase));
	}

	void FRHID3D12CommandList::DrawIndexedInstanced(size_t NumVertices, size_t NumInstances, size_t IndexIndexBase, size_t VertexIndexBase, size_t InstanceIndexBase)
	{
		D3D12CommandList->DrawIndexedInstanced(UINT(NumVertices), UINT(NumInstances), UINT(IndexIndexBase), UINT(VertexIndexBase), UINT(InstanceIndexBase));
	}

	void FRHID3D12CommandList::Dispatch(uint3 NumGroups)
	{
		D3D12CommandList->Dispatch(NumGroups.X, NumGroups.Y, NumGroups.Z);
	}
	
	void FRHID3D12CommandList::CopyResource(FRHIResource * RHIResourceDst, FRHIResource * RHIResourceSrc)
	{
		auto Device = RHID3D12Device->GetDevice();
		D3D12CommandList->CopyResource(static_cast<FRHID3D12Resource *>(RHIResourceDst)->Resource(), static_cast<FRHID3D12Resource *>(RHIResourceSrc)->Resource());
	}
	
	void FRHID3D12CommandList::CopyTexture(FRHIResource * RHIResourceDst, FRHIResource * RHIResourceSrc, const FCopyableFootprint & CopyableFootprint)
	{
		ID3D12Resource * RHID3D12ResourceDst = static_cast<FRHID3D12Resource *>(RHIResourceDst)->Resource();
		ID3D12Resource * RHID3D12ResourceSrc = static_cast<FRHID3D12Resource *>(RHIResourceSrc)->Resource();
		
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT SrcSubresourceFootprint = {};
		SrcSubresourceFootprint.Offset = CopyableFootprint.Offset;
		SrcSubresourceFootprint.Footprint.Format = FromFormat(CopyableFootprint.Format);
		SrcSubresourceFootprint.Footprint.Width = CopyableFootprint.Width;
		SrcSubresourceFootprint.Footprint.Height = CopyableFootprint.Height;
		SrcSubresourceFootprint.Footprint.Depth = CopyableFootprint.Depth;
		SrcSubresourceFootprint.Footprint.RowPitch = CopyableFootprint.RowPitch;

		D3D12_TEXTURE_COPY_LOCATION SrcLocation =
		{
			.pResource = RHID3D12ResourceSrc,
			.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
			.PlacedFootprint = SrcSubresourceFootprint,
		};
		D3D12_TEXTURE_COPY_LOCATION DstLocation =
		{
			.pResource = RHID3D12ResourceDst,
			.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			.SubresourceIndex = 0,
		};

		D3D12CommandList->CopyTextureRegion(&DstLocation, 0, 0, 0, &SrcLocation, nullptr);
	}
	
	void FRHID3D12CommandList::CopyResourceRegion(FRHIResource * RHIResourceDst, size_t DstOffset, FRHIResource * RHIResourceSrc, size_t SrcOffset, size_t NumBytes)
	{
		auto Device = RHID3D12Device->GetDevice();
		D3D12CommandList->CopyBufferRegion(static_cast<FRHID3D12Resource *>(RHIResourceDst)->Resource(), DstOffset, static_cast<FRHID3D12Resource *>(RHIResourceSrc)->Resource(), SrcOffset, NumBytes);
	}

	void FRHID3D12CommandList::CopyTextureRegion(FRHIResource * RHIResourceDst, const uint3 & DstOffset, FRHIResource * RHIResourceSrc, const uint3 & SrcOffset, const uint3 & Size)
	{
		D3D12_TEXTURE_COPY_LOCATION DstLocation =
		{
			.pResource = static_cast<FRHID3D12Resource *>(RHIResourceDst)->Resource(),
			.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			.SubresourceIndex = 0,
		};
		D3D12_TEXTURE_COPY_LOCATION SrcLocation =
		{
			.pResource = static_cast<FRHID3D12Resource *>(RHIResourceSrc)->Resource(),
			.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			.SubresourceIndex = 0,
		};

		D3D12_BOX Box =
		{
			.left = SrcOffset.X,
			.top = SrcOffset.Y,
			.front = SrcOffset.Z,
			.right = SrcOffset.X + Size.X,
			.bottom = SrcOffset.Y + Size.Y,
			.back = SrcOffset.Z + Size.Z
		};
		auto Device = RHID3D12Device->GetDevice();
		D3D12CommandList->CopyTextureRegion(&DstLocation, DstOffset.X, DstOffset.Y, DstOffset.Z, &SrcLocation, &Box);
	}

	void FRHID3D12CommandList::CopyTextureRegion(FRHIResource * RHIResourceDst, const uint3 & DstOffset, FRHIResource * RHIResourceSrc,
		size_t SubResourceIndex, size_t SubResourceCount, FCopyableFootprint * CopyableFootprints)
	{
		ID3D12Resource * RHID3D12ResourceDst = static_cast<FRHID3D12Resource *>(RHIResourceDst)->Resource();
		ID3D12Resource * RHID3D12ResourceSrc = static_cast<FRHID3D12Resource *>(RHIResourceSrc)->Resource();
		
		for (size_t Index = 0; Index < SubResourceCount; ++Index)
		{
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT SrcSubresourceFootprint = {};
			SrcSubresourceFootprint.Offset = CopyableFootprints[Index].Offset;
			SrcSubresourceFootprint.Footprint.Format = FromFormat(CopyableFootprints[Index].Format);
			SrcSubresourceFootprint.Footprint.Width = CopyableFootprints[Index].Width;
			SrcSubresourceFootprint.Footprint.Height = CopyableFootprints[Index].Height;
			SrcSubresourceFootprint.Footprint.Depth = CopyableFootprints[Index].Depth;
			SrcSubresourceFootprint.Footprint.RowPitch = CopyableFootprints[Index].RowPitch;
			
			D3D12_TEXTURE_COPY_LOCATION SrcLocation =
			{
				.pResource = RHID3D12ResourceSrc,
				.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
				.PlacedFootprint = SrcSubresourceFootprint,
			};
			D3D12_TEXTURE_COPY_LOCATION DstLocation =
			{
				.pResource = RHID3D12ResourceDst,
				.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
				.SubresourceIndex = UINT(SubResourceIndex + Index),
			};

			D3D12CommandList->CopyTextureRegion(&DstLocation, DstOffset.X, DstOffset.Y, DstOffset.Z, &SrcLocation, nullptr);
		}
	}
	
	void FRHID3D12CommandList::ResolveSubresource(FRHIResource * RHIResourceDst, size_t DestSubresourceIndex, FRHIResource * RHIResourceSrc, size_t SourceSubresourceIndex, EFormat Format)
	{
		D3D12CommandList->ResolveSubresource(
			static_cast<FRHID3D12Resource *>(RHIResourceDst)->Resource(), UINT(DestSubresourceIndex),
			static_cast<FRHID3D12Resource *>(RHIResourceSrc)->Resource(), UINT(SourceSubresourceIndex),
			FromFormat(Format));
	}
}
