#pragma once

#include "../EngineInc.h"
#include "../Texture.h"

namespace XE::Engine
{
	class ENGINE_API FTextureMaterial : public IMaterial
	{
	public:
		FTextureMaterial();
		~FTextureMaterial()
		{

		}

		void SetTexture(TSharedPtr<FTexture> TextureIn) { Texture = TextureIn; }

		void Initialize_rt(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList);

		FShader * GetPixelShader() const override { return PS.Get(); }
		bool IsWireFrame() const override { return false; }
		ECullMode GetCullMode() const override { return ECullMode::None; }
		
		void BindResources(FResourceBindings & ResourceBindings) override;

	private:
		TSharedPtr<FShader> PS;
		TSharedPtr<FTexture> Texture;

		uintx_t TextureRegisterIndex = 0;
	};
}
