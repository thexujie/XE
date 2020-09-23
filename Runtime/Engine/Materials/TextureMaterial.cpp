#include "PCH.h"
#include "TextureMaterial.h"

namespace XE::Engine
{
	FTextureMaterial::FTextureMaterial()
	{
		PS = MakeShared<FShader>(EShaderType::Pixel, Str("../Content/Shaders/Texture.hlsl"), Str("PSMain"));
	}

	void FTextureMaterial::Initialize_rt(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList)
	{
		const FShaderReflection & ShaderReflection = PS->GetShaderReflection();
		const FShaderParameterDesc * ShaderParameterDesc = ShaderReflection.FindShaderResourceByName(Str("Image"));
		if (ShaderParameterDesc)
			TextureRegisterIndex = ShaderParameterDesc->RegisterIndex;
	}
	
	void FTextureMaterial::BindResources(FResourceBindings & ResourceBindings)
	{
		ResourceBindings.SetTexture(EShaderType::Pixel, TextureRegisterIndex, Texture->GetRHIResource());
	}
}
