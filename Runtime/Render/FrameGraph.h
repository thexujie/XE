#pragma once

#include "RenderInc.h"

namespace XE::Render
{
	using FPSOCacheId = size_t;

	class FFrameResource : public IRenderObject
	{
	public:
		FFrameResource() = default;
	};

	class FFramePass: public IRenderObject
	{
	public:
		FFramePass() = default;

		virtual void AddInput(FFrameResource * FrameResource) = 0;
		virtual void SetOutput(FFrameResource * FrameResource) = 0;
	};
	
	class FFrameGraph : public IRenderObject
	{
	public:
		FFrameGraph() = default;
		virtual FFrameResource * RegisterResource(FStringView Name) = 0;
		virtual FFramePass * RegisterPass(FStringView Name) = 0;
		
	};
}
