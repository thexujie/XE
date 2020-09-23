#include "PCH.h"
#include "RenderFactory.h"


#include "InputLayout.h"
#include "Material.h"
#include "RenderPass.h"
#include "Shader.h"
#include "MeshAssembler.h"
#include "RenderTarget.h"
#include "PipelineState.h"

namespace XE::Render
{

	void FRenderFactory::InitializeRHI()
	{
		RHI::FRootSignatureDesc RootSignatureDesc;
		
		for (size_t StaticSamplerIndex = 0; StaticSamplerIndex < Size(StaticPipelineSamplerDescs); ++StaticSamplerIndex)
			RootSignatureDesc.Samplers.Add(StaticPipelineSamplerDescs[StaticSamplerIndex]);
	}
	
	FResourceAddress FRenderFactory::FetchBufferCPU(size_t NumBytes, size_t AlignmentMask)
	{
		if (NumBytes > ResourcePageSize)
		{
			RHI::FResourceDesc ResourceArgs = {};
			ResourceArgs.Heap.HeapType = EHeapType::Upload;
			ResourceArgs.Dimension = EResourceDimension::Buffer;
			ResourceArgs.Usages = EResourceUsage::None;
			ResourceArgs.Size = { (uint32_t)AlignUp(NumBytes, AlignmentMask), 1 };
			ResourceArgs.States = EResourceStates::GenericRead;
			TSharedPtr<RHI::FRHIResource> RHIResource = RHIDevice.CreateResource(ResourceArgs);
			RHIResource->SetName(Strings::Printf(Str("CPUResource %d"), RHIResourcesPage.GetSize()));
			RHIResourcesPerFrame.Add(RHIResource);

			FResourceAddress ResourceAddress;
			ResourceAddress.RHIResource = RHIResource.Get();
			ResourceAddress.CPUPointer = static_cast<byte_t *>(RHIResource->GetData());
			ResourceAddress.Offset = 0;
			ResourceAddress.NumBytes = NumBytes;
			return ResourceAddress;
		}
		
		size_t ResourcesOffsetAligned = AlignUp(RHIResourcesOffset[ResourcePageIndex], AlignmentMask);
		if (ResourcesOffsetAligned + NumBytes > ResourcePageSize)
		{
			++ResourcePageIndex;
			ResourcesOffsetAligned = 0;
		}

		if (ResourcePageIndex >= RHIResourcesPage.GetSize())
		{
			RHI::FResourceDesc ResourceArgs = {};
			ResourceArgs.Heap.HeapType = EHeapType::Upload;
			ResourceArgs.Dimension = EResourceDimension::Buffer;
			ResourceArgs.Usages = EResourceUsage::None;
			ResourceArgs.Size = { ResourcePageSize, 1 };
			ResourceArgs.States = EResourceStates::GenericRead;
			TSharedPtr<RHI::FRHIResource> RHIResource = RHIDevice.CreateResource(ResourceArgs);
			RHIResource->SetName(Strings::Printf(Str("CPUResource %d"), RHIResourcesPage.GetSize()));
			RHIResourcesPage.Add(RHIResource);
			RHIResourcesOffset.Add(0);
		}

		{
			TSharedPtr<RHI::FRHIResource> RHIResource = RHIResourcesPage[ResourcePageIndex];

			FResourceAddress ResourceAddress;
			ResourceAddress.RHIResource = RHIResource.Get();
			ResourceAddress.CPUPointer = static_cast<byte_t *>(RHIResource->GetData()) + ResourcesOffsetAligned;
			ResourceAddress.Offset = ResourcesOffsetAligned;
			ResourceAddress.NumBytes = NumBytes;
			RHIResourcesOffset[ResourcePageIndex] = ResourcesOffsetAligned + NumBytes;
			return ResourceAddress;
		}
	}

	RHI::FRHICommandList & FRenderFactory::AllcoateRHICommandList(ECommandType CommandType)
	{
		size_t CommandTypeIndex = size_t(CommandType);
		TVector<TSharedPtr<RHI::FRHICommandList>> & CommandLists = RHICommandLists[CommandTypeIndex];
		if (CommandListIndices[CommandTypeIndex] >= CommandLists.GetSize())
		{
			TSharedPtr<RHI::FRHICommandList> RHICommandList = RHIDevice.CreateCommandList(CommandType);
			RHICommandLists[size_t(CommandType)].Add(RHICommandList);
		}

		TSharedPtr<RHI::FRHICommandList> RHICommandList = RHICommandLists[CommandTypeIndex][CommandListIndices[CommandTypeIndex]];
		return *CommandLists[CommandListIndices[CommandTypeIndex]++];
	}

	RHI::FRHIDescriptorHeap & FRenderFactory::GetDescriptorHeap(EDescriptorHeapType DescriptorHeapType)
	{
		if (!RHIDescriptorHeaps[size_t(DescriptorHeapType)])
		{
			RHI::FDescriptorHeapDesc DescriptorHeapArgs = { DescriptorHeapType, 1024 };
			RHIDescriptorHeaps[size_t(DescriptorHeapType)] = RHIDevice.CreateDescriptorHeap(DescriptorHeapArgs);
		}
		return *RHIDescriptorHeaps[size_t(DescriptorHeapType)];
	}
	
	void FRenderFactory::BeginFrame()
	{
	}
	
	void FRenderFactory::EndFrame()
	{
		ResourcePageIndex = 0;
		RHIResourcesOffset.Resize(1, 0);
		Fill(CommandListIndices, size_t(0));
		RHIResourcesPerFrame.Clear();
	}

	RHI::FRHIRootSignature * FRenderFactory::GetLocalRootSignature()
	{
		if (RHIRootSignature)
			return RHIRootSignature.Get();

		RHI::FRootSignatureDesc RootSignatureDesc = {};
		
		// Private For Mesh Assembler
		for (size_t ShaderIndex = 0; ShaderIndex < ShaderTypeCount; ++ShaderIndex)
		{
			for (size_t DescriptorTypeIndex = 0; DescriptorTypeIndex < DescriptorTypeCount; ++DescriptorTypeIndex)
			{
				if (PrivateDescriptorCounts[ShaderIndex][DescriptorTypeIndex] == 0)
					continue;

				RHI::FDescriptorTableDesc DescriptorTableDesc = {};
				DescriptorTableDesc.Shader = EShaderType(ShaderIndex);
				{
					RHI::FDescriptorRangeDesc DescriptorRangeDesc = {};
					DescriptorRangeDesc.DescriptorType = EDescriptorType(DescriptorTypeIndex);
					DescriptorRangeDesc.RegisterIndex = (uint32_t)GetPrivateDescriptorRegisterBaseIndex(ShaderIndex, DescriptorTypeIndex);
					DescriptorRangeDesc.NumDescriptors = (uint32_t)PrivateDescriptorCounts[ShaderIndex][DescriptorTypeIndex];
					DescriptorRangeDesc.DescriptorFlags = EDescriptorFlags::DescriptorVolatile;
					DescriptorTableDesc.DescriptorRanges.Add(DescriptorRangeDesc);
				}

				RootSignatureDesc.DescriptorTables.Add(DescriptorTableDesc);
			}
		}
		
		// Public
		for (size_t ShaderIndex = 0; ShaderIndex < ShaderTypeCount; ++ShaderIndex)
		{
			for (size_t DescriptorTypeIndex = 0; DescriptorTypeIndex < DescriptorTypeCount; ++DescriptorTypeIndex)
			{
				for (size_t DescriptorIndex = 0; DescriptorIndex < PublicDescriptorCounts[ShaderIndex][DescriptorTypeIndex]; ++DescriptorIndex)
				{
					RHI::FDescriptorDesc DescriptorDesc = {};
					DescriptorDesc.Shader = EShaderType(ShaderIndex);
					DescriptorDesc.DescriptorType = EDescriptorType::ConstBuffer;
					DescriptorDesc.RegisterIndex = uint32_t(GetPublicDescriptorRegisterBaseIndex(ShaderIndex, DescriptorTypeIndex) + DescriptorIndex);
					DescriptorDesc.DescriptorFlags = EDescriptorFlags::None;
					RootSignatureDesc.Descriptors.Add(DescriptorDesc);
				}
			}
		}

		// Static Samplers
		for (size_t StaticSamplerIndex = 0; StaticSamplerIndex < Size(StaticPipelineSamplerDescs); ++StaticSamplerIndex)
			RootSignatureDesc.Samplers.Add(StaticPipelineSamplerDescs[StaticSamplerIndex]);

		uintx_t UnitCount = RootSignatureDesc.GetUnitCount();
		if (UnitCount > RootSignatureUnitMax)
		{
			LogError(Str("Root Signature size exceeds maximum of 64 32-bit units, {} > {}"), UnitCount, RootSignatureUnitMax);
		}

		RHIRootSignature = RHIDevice.CreateRootSignature(RootSignatureDesc);
		RHIRootSignature->SetName(Str("LocalRootSignature"));
		return RHIRootSignature.Get();
	}
	
	RHI::FRHIRootSignature * FRenderFactory::GetRootSignature(const RHI::FRootSignatureDesc & RootSignatureArgs) const
	{
		auto RootSignatureIter = RHIRootSignatures.Find(RootSignatureArgs);
		if (RootSignatureIter == RHIRootSignatures.End())
		{
			TSharedPtr<RHI::FRHIRootSignature> RHIRootSignature = RHIDevice.CreateRootSignature(RootSignatureArgs);
			RootSignatureIter = RHIRootSignatures.InsertOrAssign(RootSignatureArgs, RHIRootSignature);
		}
		return RootSignatureIter->Value.Get();
	}

	const IOutputLayout * FRenderFactory::GetOutputLayout(TView<EFormat> RenderTargetFormats, EFormat DepthStencilFormat, const FMSAA & MSAA)
	{
		SOutputLayoutIndex OutputLayoutIndex = { TVector<EFormat>(RenderTargetFormats), DepthStencilFormat, MSAA };
		auto OutputLayoutIter = OutputLayouts.Find(OutputLayoutIndex);
		if (OutputLayoutIter == OutputLayouts.End())
		{
			TSharedPtr<FOutputLayout> OutputLayout = MakeShared<FOutputLayout>(RenderTargetFormats, DepthStencilFormat, MSAA);
			OutputLayoutIter = OutputLayouts.InsertOrAssign(OutputLayoutIndex, OutputLayout).Key;
		}
		return OutputLayoutIter->Value.Get();
	}

	const IInputLayout * FRenderFactory::GetInputLayout(EVertexElements VertexElements)
	{
		return GetInputLayout(GetVertexElements(VertexElements));
	}

	const IInputLayout * FRenderFactory::GetInputLayout(TView<FVertexElement> VertexElementsView)
	{
		TVector<FVertexElement> VertexElementsA;
		TVector<FVertexElement> VertexElementsB;
		auto bccc = VertexElementsA < VertexElementsB;
		
		TVector<FVertexElement> VertexElements(VertexElementsView);
		SInputLayoutIndex InputLayoutIndex = { TVector<FVertexElement>(VertexElementsView )};
		auto aaa = InputLayoutIndex <=> InputLayoutIndex;
		
		auto InputLayoutIter = InputLayouts.Find(InputLayoutIndex);
		if (InputLayoutIter == InputLayouts.End())
		{
			TSharedPtr<FInputLayout> InputLayout = MakeShared<FInputLayout>(VertexElementsView);
			InputLayoutIter = InputLayouts.InsertOrAssign(InputLayoutIndex, InputLayout).Key;
		}
		return InputLayoutIter->Value.Get();
	}
	
	const FPipelineState * FRenderFactory::GetPipelineState(const IInputLayout * InputLayout, const IOutputLayout * OutputLayout, const IMeshAssembler * MeshAssembler, const IMaterial * Material)
	{
		SPipelineStateIndex PipelineStateIndex = { InputLayout, OutputLayout, MeshAssembler, Material };
		auto PipelineStateIter = PipelineStates.Find(PipelineStateIndex);
		if (PipelineStateIter == PipelineStates.End())
		{
			TSharedPtr<FPipelineState> PipelineState = MakeShared<FPipelineState>(*InputLayout, *OutputLayout, *MeshAssembler, *Material);
			PipelineState->InitializeRHI(RHIDevice, GetLocalRootSignature());
			PipelineStateIter = PipelineStates.InsertOrAssign(PipelineStateIndex, PipelineState).Key;
		}
		return PipelineStateIter->Value.Get();
	}
}
