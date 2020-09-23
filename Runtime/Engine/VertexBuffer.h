#pragma once

#include "EngineInc.h"

namespace XE::Engine
{
	class ENGINE_API FVertexBuffer : public FObject
	{
	public:
		FVertexBuffer(EVertexElements VertexElementsIn) : VertexElements(GetVertexElements(VertexElementsIn)) {}
		FVertexBuffer(TView<FVertexElement> VertexElementsIn) : VertexElements(VertexElementsIn) {}

		void InitializeRHI(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList, const void * Data, size_t Size);

	public:
		FRHIResource * GetRHIResource() const { return RHIResource.Get(); }
	private:
		TVector<FVertexElement> VertexElements;
		TSharedPtr<FRHIResource> RHIResource;
	};
}
