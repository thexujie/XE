#include "PCH.h"
#include "VertexBuffer.h"

namespace XE::Engine
{
	void FVertexBuffer::InitializeRHI(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList, const void * Data, size_t Size)
	{
		FResourceDesc ResourceArgs;
		ResourceArgs.Size = SizeU(uint32_t(Size), 1);
		ResourceArgs.Dimension = EResourceDimension::Buffer;
		ResourceArgs.Usages = EResourceUsage::VertexBuffer;
		RHIResource = RenderFactory.RHIDevice.CreateResource(ResourceArgs);

		FResourceAddress ResourceAddress = RenderFactory.FetchBufferCPU(Size);
		Memcpy(ResourceAddress.CPUPointer, Data, Size);
		RHICommandList.TransitionBarrier(RHIResource.Get(), EResourceStates::CopyDest);
		RHICommandList.CopyResourceRegion(RHIResource.Get(), 0, ResourceAddress.RHIResource, ResourceAddress.Offset, ResourceAddress.NumBytes);
		RHICommandList.TransitionBarrier(RHIResource.Get(), EResourceStates::VertexBuffer);
	}
}
