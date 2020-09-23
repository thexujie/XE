#pragma once

#include "RenderInc.h"

namespace XE::Render
{
	constexpr FPipelineSamplerDesc MakeStaticSampler(EFilter Filter, EAddressMode AddressMode, size_t RegisterIndex)
	{
		FPipelineSamplerDesc PipelineSamplerDesc = {};
		PipelineSamplerDesc.Filter = Filter;
		PipelineSamplerDesc.AddressModeU = AddressMode;
		PipelineSamplerDesc.AddressModeV = AddressMode;
		PipelineSamplerDesc.AddressModeW = AddressMode;
		PipelineSamplerDesc.MipLODBias = 0.0f;
		PipelineSamplerDesc.Anisotropy = 1;
		PipelineSamplerDesc.Comparison = EComparison::None;
		PipelineSamplerDesc.MinLOD = 0.0f;
		PipelineSamplerDesc.MaxLOD = Float32Max;
		PipelineSamplerDesc.RegisterIndex = static_cast<uint32_t>(RegisterIndex);
		PipelineSamplerDesc.RegisterSpace = 0;
		PipelineSamplerDesc.ShaderMask = EShaderMasks::All;
		return PipelineSamplerDesc;
	}

	constexpr FPipelineSamplerDesc StaticPipelineSamplerDescs[] =
	{
		MakeStaticSampler(EFilter::Point, EAddressMode::Wrap, 10),
		MakeStaticSampler(EFilter::Point, EAddressMode::Clamp, 11),
		MakeStaticSampler(EFilter::Bilinear, EAddressMode::Wrap, 12),
		MakeStaticSampler(EFilter::Bilinear, EAddressMode::Clamp, 13),
		MakeStaticSampler(EFilter::Trilinear, EAddressMode::Wrap, 14),
		MakeStaticSampler(EFilter::Trilinear, EAddressMode::Clamp, 15),
	};

	const size_t RootSignatureUnitMax = 64;
	
	constexpr size_t PrivateDescriptorCounts[ShaderTypeCount][DescriptorTypeCount] =
	{
		{ 0, 4, 16, 8, 0 }, // VS
		{}, // HS
		{}, // DS
		{}, // GS
		{ 0, 4, 16, 8, 0 }, // PS
		{}, // CS
		{ 0, 4, 16, 8, 0 }, // All
	};

	constexpr size_t GetPrivateDescriptorRootIndex(size_t ShaderType, size_t DescriptorType)
	{
		size_t PrivateDescriptorRootIndex = 0;
		for (size_t ShaderIndex = 0; ShaderIndex < ShaderType; ++ShaderIndex)
		{
			for (size_t DescriptorTypeIndex = 0; DescriptorTypeIndex < DescriptorTypeCount; ++DescriptorTypeIndex)
			{
				if (PrivateDescriptorCounts[ShaderIndex][DescriptorTypeIndex] > 0)
					++PrivateDescriptorRootIndex;
			}
		}

		for (size_t DescriptorTypeIndex = 0; DescriptorTypeIndex < DescriptorType; ++DescriptorTypeIndex)
		{
			if (PrivateDescriptorCounts[ShaderType][DescriptorTypeIndex] > 0)
				++PrivateDescriptorRootIndex;
		}

		return PrivateDescriptorRootIndex;
	}
	
	constexpr size_t GetPrivateDescriptorRegisterBaseIndex(size_t ShaderType, size_t DescriptorType)
	{
		if (ShaderType < DescriptorTypeCount - 1)
			return 0;

		size_t PrivateRegisterBaseIndex = 0;
		for (size_t ShaderIndex = 0; ShaderIndex < ShaderTypeCount - 1; ++ShaderIndex)
			PrivateRegisterBaseIndex = Max(PrivateRegisterBaseIndex, PrivateDescriptorCounts[ShaderIndex][DescriptorType]);
		return PrivateRegisterBaseIndex;
	}

	constexpr size_t GetPrivateDescriptorCountMax()
	{
		size_t PrivateDescriptorCountMax = 0;
		for (size_t ShaderIndex = 0; ShaderIndex < ShaderTypeCount; ++ShaderIndex)
		{
			for (size_t DescriptorTypeIndex = 0; DescriptorTypeIndex < DescriptorTypeCount; ++DescriptorTypeIndex)
				PrivateDescriptorCountMax = Max(PrivateDescriptorCountMax, PrivateDescriptorCounts[ShaderIndex][DescriptorTypeIndex]);
		}
		return PrivateDescriptorCountMax;
	}
	constexpr size_t PrivateDescriptorCountMax = GetPrivateDescriptorCountMax();

	constexpr size_t PublicDescriptorCounts[ShaderTypeCount][DescriptorTypeCount] =
	{
		{}, {}, {}, {}, {}, {}, {}
	};

	constexpr size_t GetPublicDescriptorRegisterBaseIndex(size_t ShaderType, size_t DescriptorType)
	{
		if (ShaderType < DescriptorTypeCount - 1)
			return GetPrivateDescriptorRegisterBaseIndex(AsIndex(EShaderType::All), DescriptorType) + PrivateDescriptorCounts[AsIndex(EShaderType::All)][DescriptorType];

		size_t PublicRegisterBaseIndex = 0;
		for (size_t ShaderIndex = 0; ShaderIndex < ShaderTypeCount - 1; ++ShaderIndex)
			PublicRegisterBaseIndex = Max(PublicRegisterBaseIndex, PrivateDescriptorCounts[ShaderIndex][DescriptorType]);
		return GetPrivateDescriptorRegisterBaseIndex(AsIndex(EShaderType::All), DescriptorType) + PrivateDescriptorCounts[AsIndex(EShaderType::All)][DescriptorType] + PublicRegisterBaseIndex;
	}

	struct FResourceAddress
	{
		FRHIResource * RHIResource = nullptr;
		byte_t * CPUPointer = nullptr;
		size_t Offset = 0;
		size_t NumBytes = 0;
	};
	
	class RENDER_API FRenderFactory
	{
	public:
		FRenderFactory(FRHIDevice & RHIDeviceIn, FRHICommandQueue & RHICommandQueueIn) : RHIDevice(RHIDeviceIn), RHICommandQueue(RHICommandQueueIn){}

	public:
		void InitializeRHI();
		
		FResourceAddress FetchBufferCPU(size_t NumBytes, size_t AlignmentMask = 0);
		FRHICommandList & AllcoateRHICommandList(ECommandType CommandType);

	public:
		FRHIDescriptorHeap & GetDescriptorHeap(EDescriptorHeapType DescriptorHeapType);
		
	public:
		void BeginFrame();
		void EndFrame();

	public:
		FRHIDevice & RHIDevice;
		FRHICommandQueue & RHICommandQueue;

	private:
		TArray<TSharedPtr<FRHIDescriptorHeap>, DescriptorHeapTypeCount> RHIDescriptorHeaps;
		
		enum
		{
			kGpuAllocatorPageSize = 0x10000,    // 64K
			kCpuAllocatorPageSize = 0x200000    // 2MB
		};
		TVector<TSharedPtr<FRHIResource>> RHIResourcesPage;
		TVector<TSharedPtr<FRHIResource>> RHIResourcesPerFrame;
		TVector<size_t> RHIResourcesOffset = { 0 };
		uint32_t ResourcePageSize = kCpuAllocatorPageSize;
		uint32_t ResourcePageIndex = 0;

		TArray<TVector<TSharedPtr<FRHICommandList>>, ECommandTypeCount> RHICommandLists;
		TArray<size_t, ECommandTypeCount> CommandListIndices = { EInitializeMode::Zero };

	public:
		TSharedPtr<FRHIRootSignature> RHIRootSignature;
		FRHIRootSignature * GetLocalRootSignature();
		FRHIRootSignature * GetRootSignature(const FRootSignatureDesc & RootSignatureArgs) const;

	public:
		const IOutputLayout * GetOutputLayout(TView<EFormat> RenderTargetFormats, EFormat DepthStencilFormat, const FMSAA & MSAA);
		const IInputLayout * GetInputLayout(EVertexElements VertexElements);
		const IInputLayout * GetInputLayout(TView<FVertexElement> VertexElements);
		const FPipelineState * GetPipelineState(const IInputLayout * InputLayout, const IOutputLayout * OutputLayout, const IMeshAssembler * MeshAssembler, const IMaterial * Material);
		
	private:
		struct SOutputLayoutIndex
		{
			auto operator<=>(const SOutputLayoutIndex &) const noexcept = default;
			TVector<EFormat> RenderTargetFormats;
			EFormat DepthStencilFormat;
			FMSAA MSAA;
		};
		mutable TMap<SOutputLayoutIndex, TSharedPtr<IOutputLayout>> OutputLayouts;

		struct SInputLayoutIndex
		{
			auto operator<=>(const SInputLayoutIndex &) const noexcept = default;
			TVector<FVertexElement> VertexElements;
		};
		mutable TMap<SInputLayoutIndex, TSharedPtr<IInputLayout>> InputLayouts;
		
		struct SPipelineStateIndex
		{
			auto operator<=>(const SPipelineStateIndex &) const noexcept = default;
			const IInputLayout * InputLayout = 0;
			const IOutputLayout * OutputLayout = 0;
			const IMeshAssembler * MeshAssembler = 0;
			const IMaterial * Material = 0;
		};
		mutable TMap<SPipelineStateIndex, TSharedPtr<FPipelineState>> PipelineStates;
		mutable THashMap<FRootSignatureDesc, TSharedPtr<FRHIRootSignature>> RHIRootSignatures;
	};
}