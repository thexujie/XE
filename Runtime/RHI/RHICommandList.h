#pragma once

#include "RHIInc.h"

namespace XE::RHI
{
	class FRHIRootSignature;
	class FRHIPipelineState;
	class FRHIResource;
	class FRHICommandAllocator;
	class RHIResourceView;
	class FRHISwapChain;
	class FRHIDescriptorHeap;
	class FRHIDescriptorTable;
	class FRHIResourceView;

	class RHI_API FRHICommandList : public FRHIDeviceObject
	{
	public:
		virtual void Reset() = 0;
		virtual void Open() = 0;
		virtual void Close() = 0;

		virtual void SetRootSignature(FRHIRootSignature * RootSignature) = 0;
		virtual void SetPipelineState(FRHIPipelineState * PipelineState) = 0;

		virtual void ClearDepthStencil(FRHIResourceView * DepthStencilView, float32_t Depth, uint8_t Stencial) = 0;
		virtual void ClearRenderTarget(FRHIResourceView * RenderTargetView, FColor Color) = 0;
		virtual void SetRenderTarget(TView<FRHIResourceView *> RenderTargetViews, FRHIResourceView * DepthStencilView) = 0;

		virtual void SetViewPort(const FViewPort & ViewPort) = 0;
		virtual void SetScissorRect(const RectI & ScissorRect) = 0;
		
		virtual void TransitionBarrier(FRHIResource * RHIResource, EResourceStates ResourceStates) = 0;
		virtual void UnorderedBarrier(FRHIResource * RHIResource) = 0;

		//virtual void SetDescriptorHeaps(FRHIDescriptorHeap * ShaderResourceRHIDescriptorHeap, FRHIDescriptorHeap * SamplerRHIDescriptorHeap) = 0;
		virtual void SetRootConstBuffer(size_t RootParameterIndex, FRHIResource * RHIResource) = 0;
		virtual void SetRootShaderResource(size_t RootParameterIndex, FRHIResource * RHIResource) = 0;
		virtual void SetRootUnorderedResource(size_t RootParameterIndex, FRHIResource * RHIResource) = 0;
		virtual void SetRootDescriptorTable(size_t DescriptorTableIndex, FRHIResourceView * ResourceView) = 0;
		virtual void SetRootDescriptorTable(size_t RootParameterIndex, FGPUAddress GPUAddress) = 0;
		
		// Graphics
		virtual void IASetVertexBuffer(FRHIResource * RHIResource, size_t Stride, size_t VertexCount) = 0;
		virtual void IASetIndexBuffer(FRHIResource * RHIResource, size_t Stride, size_t IndexCount) = 0;
		virtual void IASetTopology(ETopology Topology) = 0;
		virtual void DrawInstanced(size_t NumVertices, size_t NumInstances, size_t VertexIndexBase, size_t InstanceIndexBase) = 0;
		virtual void DrawIndexedInstanced(size_t NumIndicies, size_t NumInstances, size_t IndexBase, size_t VertexIndexBase, size_t InstanceIndexBase) = 0;

		// Compute
		virtual void Dispatch(uint3 ngroups) = 0;

		// Copy
		virtual void CopyResource(FRHIResource * RHIResourceDst, FRHIResource * RHIResourceSrc) = 0;
		virtual void CopyTexture(FRHIResource * RHIResourceDst, FRHIResource * RHIResourceSrc, const FCopyableFootprint & CopyableFootprint) = 0;
		
		virtual void CopyResourceRegion(FRHIResource * RHIResourceDst, size_t DstOffset, FRHIResource * RHIResourceSrc, size_t SrcOffset, size_t NumBytes) = 0;
		virtual void CopyTextureRegion(FRHIResource * RHIResourceDst, const uint3 & DstOffset, FRHIResource * RHIResourceSrc, const uint3 & SrcOffset, const uint3 & Size) = 0;
		//virtual void CopyTextureRegion(FRHIResource * RHIResourceDst, const uint3 & DstOffset, FRHIResource * RHIResourceSrc, size_t SrcOffset, size_t NumBytes) = 0;

		virtual void ResolveSubresource(FRHIResource * RHIResourceDst, size_t DestSubresourceIndex, FRHIResource * RHIResourceSrc, size_t SourceSubresourceIndex, EFormat Format) = 0;
	};
}
