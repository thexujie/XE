#include "PCH.h"
#include "ResourceBindings.h"


namespace XE::Render
{
	void FResourceBindings::SetConstBuffer(EShaderType ShaderType, size_t RegisterIndex, FRHIResource * RHIResource)
	{
		if (!RHIResource || RHIResources[AsIndex(ShaderType)][AsIndex(EDescriptorType::ConstBuffer)][RegisterIndex] == RHIResource)
			return;

		RHIResources[AsIndex(ShaderType)][AsIndex(EDescriptorType::ConstBuffer)][RegisterIndex] = RHIResource;
		RHIResourceViews[AsIndex(ShaderType)][AsIndex(EDescriptorType::ConstBuffer)][RegisterIndex] = FResourceViewArgs::CBuffer();
		NeedUpdate = true;
	}

	void FResourceBindings::SetTexture(EShaderType ShaderType, size_t RegisterIndex, FRHIResource * RHIResource)
	{
		if (!RHIResource || RHIResources[AsIndex(ShaderType)][AsIndex(EDescriptorType::ShaderResource)][RegisterIndex] == RHIResource)
			return;

		RHIResources[AsIndex(ShaderType)][AsIndex(EDescriptorType::ShaderResource)][RegisterIndex] = RHIResource;
		RHIResourceViews[AsIndex(ShaderType)][AsIndex(EDescriptorType::ShaderResource)][RegisterIndex] = FResourceViewArgs::Texture2D(RHIResource->GetResourceArgs().Format);
		NeedUpdate = true;
	}

	void FResourceBindings::Update_rt(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList)
	{
		if (!NeedUpdate)
			return;

		TArray<TArray<size_t, DescriptorTypeCount>, ShaderTypeCount> DescriptorTableOffsets;
		
		size_t DescriptorTableOffset = 0;
		for (size_t ShaderIndex = 0; ShaderIndex < ShaderTypeCount; ++ShaderIndex)
		{
			for (size_t DescriptorTypeIndex = 0; DescriptorTypeIndex < DescriptorTypeCount; ++DescriptorTypeIndex)
			{
				auto & TypedResources = RHIResources[ShaderIndex][DescriptorTypeIndex];
				if (AllOf(TypedResources, [](const FRHIResource * RHIResource) { return !RHIResource; }))
				{
					DescriptorTableOffsets[ShaderIndex][DescriptorTypeIndex] = NullIndex;
					continue;
				}

				DescriptorTableOffsets[ShaderIndex][DescriptorTypeIndex] = DescriptorTableOffset;
				DescriptorTableOffset += PrivateDescriptorCounts[ShaderIndex][DescriptorTypeIndex];
			}
		}

		for (size_t ShaderIndex = 0; ShaderIndex < ShaderTypeCount; ++ShaderIndex)
		{
			for (size_t DescriptorTypeIndex = 0; DescriptorTypeIndex < DescriptorTypeCount; ++DescriptorTypeIndex)
			{
				if (PrivateDescriptorCounts[ShaderIndex][DescriptorTypeIndex] == 0 || DescriptorTableOffsets[ShaderIndex][DescriptorTypeIndex] == NullIndex)
					continue;
				
				if (DescriptorTables[ShaderIndex][DescriptorTypeIndex].CPUAddress == NullAddress)
					DescriptorTables[ShaderIndex][DescriptorTypeIndex] = RenderFactory.RHIDevice.AllocateDescriptor(EDescriptorHeapType::ShaderResource, PrivateDescriptorCounts[ShaderIndex][DescriptorTypeIndex]);

				auto & Adress = DescriptorTables[ShaderIndex][DescriptorTypeIndex];
				auto & TypedResources = RHIResources[ShaderIndex][DescriptorTypeIndex];
				auto & TypedResourceViews = RHIResourceViews[ShaderIndex][DescriptorTypeIndex];
				for (size_t ResourceIndex = 0 ; ResourceIndex < TypedResources.Size; ++ResourceIndex)
				{
					RenderFactory.RHIDevice.BindDescriptor(TypedResources[ResourceIndex], Adress.CPUAddress + Adress.AddressUnit * ResourceIndex, TypedResourceViews[ResourceIndex]);
				}
			}
		}
		NeedUpdate = false;
	}

	void FResourceBindings::Bind(FRHICommandList & RHICommandList) const
	{
		for (size_t ShaderIndex = 0; ShaderIndex < ShaderTypeCount; ++ShaderIndex)
		{
			for (size_t DescriptorTypeIndex = 0; DescriptorTypeIndex < DescriptorTypeCount; ++DescriptorTypeIndex)
			{
				if (DescriptorTables[ShaderIndex][DescriptorTypeIndex].CPUAddress != NullAddress)
					RHICommandList.SetRootDescriptorTable(GetPrivateDescriptorRootIndex(ShaderIndex, DescriptorTypeIndex), DescriptorTables[ShaderIndex][DescriptorTypeIndex].GPUAddress);
			}
		}
	}
}
