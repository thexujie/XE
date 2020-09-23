#include "PCH.h"
#include "View.h"

namespace XE::Render
{
	void FView::ResetPrimitiveRenderCommand()
	{
		PrimitiveRenderCommands.Reset();
	}
	
	FPrimitiveRenderCommand & FView::AllocPrimitiveRenderCommand()
	{
		return PrimitiveRenderCommands.Emplace();
	}
}
