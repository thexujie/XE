#pragma once

#include "RenderInc.h"
#include "RenderFactory.h"

namespace XE::Render
{
	class RENDER_API FResourceBindings : IRenderObject
	{
	public:
		FResourceBindings() = default;

		void Update_rt(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList);
		
		void SetConstBuffer(EShaderType ShaderType, size_t RegisterIndex, FRHIResource * RHIResource);
		void SetTexture(EShaderType ShaderType, size_t RegisterIndex, FRHIResource * RHIResource);

		void Bind(FRHICommandList & RHICommandList) const;
		
		FAddress GetAddress() const { return Address; }

	private:
		TArray<TArray<TArray<FRHIResource *, PrivateDescriptorCountMax>, DescriptorTypeCount>, ShaderTypeCount> RHIResources;
		TArray<TArray<TArray<FResourceViewArgs, PrivateDescriptorCountMax>, DescriptorTypeCount>, ShaderTypeCount> RHIResourceViews;
		
		FAddress Address;
		TArray<TArray<FAddress, DescriptorTypeCount>, ShaderTypeCount> DescriptorTables;
		
		bool NeedUpdate = false;
	};
}
