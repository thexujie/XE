#include "PCH.h"
#include "ColorMaterial.h"

namespace XE::Engine
{
	FColorMaterial::FColorMaterial()
	{
		PS = MakeShared<FShader>(EShaderType::Pixel, Str("../Content/Shaders/BasePixelShader.hlsl"), Str("PSMain"));
	}

	FColorMaterial::~FColorMaterial()
	{

	}

	void FColorMaterial::InitializeRHI(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList)
	{
		FResourceDesc ResourceArgs = {};
		ResourceArgs.Dimension = EResourceDimension::Buffer;
		ResourceArgs.Size = { uint32_t(AlignUp(sizeof(FShaderParameter), 0xff)), 1 };
		RHIShaderParameter = RenderFactory.RHIDevice.CreateResource(ResourceArgs);

		FResourceAddress ResourceAddress = RenderFactory.FetchBufferCPU(sizeof(FShaderParameter));
		FShaderParameter & ShaderParameter = *(FShaderParameter *)ResourceAddress.CPUPointer;
		ShaderParameter.Color = float3(1.f, 0.f, 0.f);
		RHICommandList.TransitionBarrier(RHIShaderParameter.Get(), EResourceStates::CopyDest);
		RHICommandList.CopyResourceRegion(RHIShaderParameter.Get(), 0, ResourceAddress.RHIResource, ResourceAddress.Offset, ResourceAddress.NumBytes);
		RHICommandList.TransitionBarrier(RHIShaderParameter.Get(), EResourceStates::ConstBuffer);


		const FShaderReflection & ShaderReflection = PS->GetShaderReflection();
		const FShaderParameterDesc * ShaderParameterDesc = ShaderReflection.FindShaderResourceByName(Str("MaterialParemeter"));
		if (ShaderParameterDesc)
			ResgisterIndex = ShaderParameterDesc->RegisterIndex;
	}

	void FColorMaterial::BindResources(FResourceBindings & ResourceBindings)
	{
		ResourceBindings.SetConstBuffer(EShaderType::Pixel, ResgisterIndex, RHIShaderParameter.Get());
	}
}
