#pragma once

#include "RHID3D12Core.h"
#include "RHID3D12Device.h"

namespace XE::RHI::D3D12
{
	class FRHID3D12RenderTarget;
	class FRHID3D12DescriptorHeap;
	class RHID3D12ResourceView;

	class FRHID3D12CommandList : public FRHICommandList
	{
	public:
		FRHID3D12CommandList(FRHID3D12Device * RHID3D12DeviceIn) : RHID3D12Device(RHID3D12DeviceIn) {}
		virtual ~FRHID3D12CommandList() = default;

		EError Create(ECommandType CommandTypeIn);
		void SetName(FStringView Name) override;

	public:
		void Reset() override;
		void Open() override;
		void Close() override;

		void SetRootSignature(FRHIRootSignature * RootSignature) override;
		void SetPipelineState(FRHIPipelineState * PipelineState) override;

		//void SetDescriptorHeaps(FRHIDescriptorHeap * ShaderResourceRHIDescriptorHeap, FRHIDescriptorHeap * SamplerRHIDescriptorHeap) override;
		void SetRootConstBuffer(size_t RootParameterIndex, FRHIResource * RHIResource) override;
		void SetRootShaderResource(size_t RootParameterIndex, FRHIResource * RHIResource) override;
		void SetRootUnorderedResource(size_t RootParameterIndex, FRHIResource * RHIResource) override;
		void SetRootDescriptorTable(size_t RootParameterIndex, FRHIResourceView * ResourceView) override;
		void SetRootDescriptorTable(size_t RootParameterIndex, FGPUAddress GPUAddress) override;
		
		void SetRenderTarget(TView<FRHIResourceView *> RenderTargetViews, FRHIResourceView * DepthStencilView) override;
		void ClearDepthStencil(FRHIResourceView * DepthStencilView, float32_t Depth, uint8_t Stencial) override;
		void ClearRenderTarget(FRHIResourceView * RenderTargetView, FColor Color) override;
		
		void SetViewPort(const FViewPort & ViewPort) override;
		void SetScissorRect(const RectI & ScissorRect) override;
		
		void TransitionBarrier(FRHIResource * RHIResource, EResourceStates ResourceStates) override;
		void UnorderedBarrier(FRHIResource * RHIResource) override;


		// Graphics
		void IASetVertexBuffer(FRHIResource * RHIResource, size_t Stride, size_t VertexCount) override;
		void IASetIndexBuffer(FRHIResource * RHIResource, size_t Stride, size_t IndexCount) override;
		void IASetTopology(ETopology Topology) override;
		void DrawInstanced(size_t NumVertices, size_t NumInstances, size_t VertexIndexBase, size_t InstanceIndexBase) override;
		void DrawIndexedInstanced(size_t NumVertices, size_t NumInstances, size_t IndexIndexBase, size_t VertexIndexBase, size_t InstanceIndexBase) override;

		// Compute
		void Dispatch(uint3 NumGroups) override;
		
		// Copy
		void CopyResource(FRHIResource * RHIResourceDst, FRHIResource * RHIResourceSrc) override;
		void CopyTexture(FRHIResource * RHIResourceDst, FRHIResource * RHIResourceSrc, const FCopyableFootprint & CopyableFootprint) override;
		
		void CopyResourceRegion(FRHIResource * RHIResourceDst, size_t DstOffset, FRHIResource * RHIResourceSrc, size_t SrcOffset, size_t NumBytes) override;
		void CopyTextureRegion(FRHIResource * RHIResourceDst, const uint3 & DstOffset, FRHIResource * RHIResourceSrc, const uint3 & SrcOffset, const uint3 & Size) override;
		
		void CopyTextureRegion(FRHIResource * RHIResourceDst, const uint3 & DstOffset, FRHIResource * RHIResourceSrc, size_t SubResourceIndex, size_t SubResourceCount, FCopyableFootprint * CopyableFootprints);
		
		void ResolveSubresource(FRHIResource * RHIResourceDst, size_t DestSubresourceIndex, FRHIResource * RHIResourceSrc, size_t SourceSubresourceIndex, EFormat Format) override;

	public:
		ID3D12GraphicsCommandList * Ptr() const { return D3D12CommandList.Get(); }

	private:
		FRHID3D12Device * RHID3D12Device = nullptr;

		ECommandType CommandType = ECommandType::Direct;
		TReferPtr<ID3D12CommandAllocator> D3D12CommandAllocator;
		TReferPtr<ID3D12GraphicsCommandList> D3D12CommandList;
	};
}
