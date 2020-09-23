#pragma once

#include "RenderInc.h"

namespace XE::Render
{
	class RENDER_API IMaterial : public IRenderResource
	{
	public:
		virtual void InitializeRHI(FRenderFactory & RenderFactory) {}
		virtual void ReleaseRHI() {}

		virtual FShader * GetPixelShader() const = 0;
		
		virtual bool IsDepthClip() const { return true; }
		virtual bool IsWireFrame() const { return false; }
		virtual ECullMode GetCullMode() const { return ECullMode::Back; }

		virtual void BindResources(FResourceBindings & ResourceBindings) = 0;
	};

	class RENDER_API FMaterialInstance : public IRenderResource
	{
	public:
		const IMaterial * GetMaterial() const { return nullptr; }
	};

	class RENDER_API FMaterialFactory
	{
	public:

		
	public:
		static FMaterialFactory & Instance();
	};

	inline FMaterialFactory & GMaterialFactory = FMaterialFactory::Instance();
}

