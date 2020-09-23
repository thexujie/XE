#pragma once

#include "RenderInc.h"

namespace XE::Render
{
	struct FRenderResourceBindings
	{
		uint32_t RootIndex = 0;
		FGPUAddress GPUAddress = 0;
	};

	struct FPrimitiveRenderCommand
	{
		const IInputLayout * InputLayout = nullptr;
		const IMeshAssembler * MeshAssembler = nullptr;
		const IMaterial * Material = nullptr;

		FResourceBindings * ResourceBindings = nullptr;

		FTransform Transform;
		FBoundingBox BoundingBox;

		ETopology Topology = ETopology::TriangleList;
		RHI::FRHIResource * VertexBuffer = nullptr;
		RHI::FRHIResource * IndexBuffer = nullptr;
		uint32_t NumVertices = 0;
		uint32_t NumIndices = 0;
		uint32_t VertexStride = 0;
		uint32_t IndexStride = 0;
		uint32_t VertexIndexBase = 0;
		uint32_t IndexIndexBase = 0;
		uint32_t NumInstances = 0;
		uint32_t InstanceIndexBase = 0;
	};
	
	struct FRenderCommand
	{
		auto operator <=>(const FRenderCommand &) const noexcept = default;
		
		const FPipelineState * PipelineState = nullptr;
		const FResourceBindings * ResourceBindings = nullptr;

		ETopology Topology = ETopology::TriangleList;
		RHI::FRHIResource * VertexBuffer = nullptr;
		RHI::FRHIResource * IndexBuffer = nullptr;

		uint32_t NumVertices = 0;
		uint32_t NumIndices = 0;
		uint32_t VertexStride = 0;
		uint32_t IndexStride = 0;
		uint32_t VertexIndexBase = 0;
		uint32_t IndexIndexBase = 0;
		uint32_t NumInstances = 0;
		uint32_t InstanceIndexBase = 0;
	};


	struct FCacheAddress
	{
		uint32_t PageIndex = 0;
		uint32_t ByteOffset = 0;
		byte_t * Data = nullptr;
	};
}
