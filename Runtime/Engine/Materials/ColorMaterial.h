#pragma once

#include "../EngineInc.h"

namespace XE::Engine
{
	class ENGINE_API FColorMaterial : public IMaterial
	{
	public:
		FColorMaterial();
		~FColorMaterial();

		void InitializeRHI(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList);

		FShader * GetPixelShader() const override { return PS.Get(); }
		bool IsWireFrame() const override { return false; }
		virtual ECullMode GetCullMode() const override { return ECullMode::None; }

		void BindResources(FResourceBindings & ResourceBindings);


	private:
		struct FShaderParameter
		{
			float3 Color;
		};
		
		TSharedPtr<FRHIResource> RHIShaderParameter;
		TSharedPtr<FShader> PS;

		uint32_t ResgisterIndex = 0;
	};
}
