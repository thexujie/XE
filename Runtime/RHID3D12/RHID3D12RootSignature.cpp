#include "PCH.h"
#include "RHID3D12RootSignature.h"

namespace XE::RHI::D3D12
{
	EError FRHID3D12RootSignature::Create(const FRootSignatureDesc & RootSignatureArgsIn)
	{
		if (!RHID3D12Device)
			return EError::State;

		HRESULT HR = S_OK;

		auto Device = RHID3D12Device->GetDevice();
		assert(Device);

		TVector<D3D12_ROOT_PARAMETER1> D3D12RootParamters( RootSignatureArgsIn.DescriptorTables.GetSize() + RootSignatureArgsIn.UInt32Constants.GetSize() + RootSignatureArgsIn.Descriptors.GetSize());

		size_t ParamIndexBase = 0;
		size_t NumRanges = 0;
		for (size_t TableIndex = 0; TableIndex < RootSignatureArgsIn.DescriptorTables.GetSize(); ++TableIndex)
			NumRanges += RootSignatureArgsIn.DescriptorTables[TableIndex].DescriptorRanges.GetSize();

		TVector<D3D12_DESCRIPTOR_RANGE1> D3D12DescriptorRanges(NumRanges);
		for (size_t DescriptorTableIndex = 0, RangeBaseIndex = 0; DescriptorTableIndex < RootSignatureArgsIn.DescriptorTables.GetSize(); ++DescriptorTableIndex)
		{
			const FDescriptorTableDesc & DescriptorTableDescs = RootSignatureArgsIn.DescriptorTables[DescriptorTableIndex];
			D3D12RootParamters[DescriptorTableIndex].ParameterType = FromRootPipelineParameterType(EDescriptorType::DescriptorTable);
			D3D12RootParamters[DescriptorTableIndex].ShaderVisibility = FromShaderType(DescriptorTableDescs.Shader);

			D3D12RootParamters[DescriptorTableIndex].DescriptorTable.NumDescriptorRanges = UINT(DescriptorTableDescs.DescriptorRanges.GetSize());
			D3D12RootParamters[DescriptorTableIndex].DescriptorTable.pDescriptorRanges = &(D3D12DescriptorRanges[RangeBaseIndex]);

			for (size_t RangeIndex = 0; RangeIndex < DescriptorTableDescs.DescriptorRanges.GetSize(); ++RangeIndex)
			{
				const FDescriptorRangeDesc & DescriptorRangeDesc = DescriptorTableDescs.DescriptorRanges[RangeIndex];
				D3D12DescriptorRanges[RangeBaseIndex + RangeIndex].RangeType = FromDescriptorRangeType(DescriptorRangeDesc.DescriptorType);
				D3D12DescriptorRanges[RangeBaseIndex + RangeIndex].NumDescriptors = DescriptorRangeDesc.NumDescriptors;
				D3D12DescriptorRanges[RangeBaseIndex + RangeIndex].BaseShaderRegister = DescriptorRangeDesc.RegisterIndex;
				D3D12DescriptorRanges[RangeBaseIndex + RangeIndex].RegisterSpace = DescriptorRangeDesc.RegisterSpace;
				D3D12DescriptorRanges[RangeBaseIndex + RangeIndex].Flags = FromDescriptorFlags(DescriptorRangeDesc.DescriptorFlags);
				D3D12DescriptorRanges[RangeBaseIndex + RangeIndex].OffsetInDescriptorsFromTableStart = DescriptorRangeDesc.PacketOffset == UInt32Max ? D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND : DescriptorRangeDesc.PacketOffset;

				//if (range.type == DescriptorRangeType::ConstBuffer)
				//	ranges[irangebase + irange].Flags |= D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
			}
			RangeBaseIndex += DescriptorTableDescs.DescriptorRanges.GetSize();
		}
		ParamIndexBase += RootSignatureArgsIn.DescriptorTables.GetSize();

		for (size_t UInt32ConstantIndex = 0; UInt32ConstantIndex < RootSignatureArgsIn.UInt32Constants.GetSize(); ++UInt32ConstantIndex)
		{
			const FUInt32ConstantDesc & UInt32ConstantArgs = RootSignatureArgsIn.UInt32Constants[UInt32ConstantIndex];
			D3D12RootParamters[ParamIndexBase + UInt32ConstantIndex].ParameterType = FromRootPipelineParameterType(EDescriptorType::UInt32Constant);
			D3D12RootParamters[ParamIndexBase + UInt32ConstantIndex].ShaderVisibility = FromShaderType(UInt32ConstantArgs.Shader);
			D3D12RootParamters[ParamIndexBase + UInt32ConstantIndex].Constants.ShaderRegister = UInt32ConstantArgs.RegisterIndex;
			D3D12RootParamters[ParamIndexBase + UInt32ConstantIndex].Constants.RegisterSpace = UInt32ConstantArgs.RegisterSpace;
			D3D12RootParamters[ParamIndexBase + UInt32ConstantIndex].Constants.Num32BitValues = UInt32ConstantArgs.NumValues;
		}
		ParamIndexBase += RootSignatureArgsIn.UInt32Constants.GetSize();

		for (size_t DescriptorIndex = 0; DescriptorIndex < RootSignatureArgsIn.Descriptors.GetSize(); ++DescriptorIndex)
		{
			const FDescriptorDesc & DescriptorArgs = RootSignatureArgsIn.Descriptors[DescriptorIndex];
			D3D12RootParamters[ParamIndexBase + DescriptorIndex].ParameterType = FromRootPipelineParameterType(DescriptorArgs.DescriptorType);
			D3D12RootParamters[ParamIndexBase + DescriptorIndex].ShaderVisibility = FromShaderType(DescriptorArgs.Shader);
			D3D12RootParamters[ParamIndexBase + DescriptorIndex].Descriptor.ShaderRegister = DescriptorArgs.RegisterIndex;
			D3D12RootParamters[ParamIndexBase + DescriptorIndex].Descriptor.RegisterSpace = DescriptorArgs.RegisterSpace;
			D3D12RootParamters[ParamIndexBase + DescriptorIndex].Descriptor.Flags = FromRootDescriptorFlags(DescriptorArgs.DescriptorFlags);
		}

		TVector<D3D12_STATIC_SAMPLER_DESC> D3D12StaticSamplerDescs(RootSignatureArgsIn.Samplers.GetSize());
		for (size_t SamplerIndex = 0; SamplerIndex < RootSignatureArgsIn.Samplers.GetSize(); ++SamplerIndex)
		{
			D3D12StaticSamplerDescs[SamplerIndex].Filter = FromFilter(RootSignatureArgsIn.Samplers[SamplerIndex].Filter);
			D3D12StaticSamplerDescs[SamplerIndex].AddressU = FromAddressMode(RootSignatureArgsIn.Samplers[SamplerIndex].AddressModeU);
			D3D12StaticSamplerDescs[SamplerIndex].AddressV = FromAddressMode(RootSignatureArgsIn.Samplers[SamplerIndex].AddressModeV);
			D3D12StaticSamplerDescs[SamplerIndex].AddressW = FromAddressMode(RootSignatureArgsIn.Samplers[SamplerIndex].AddressModeW);
			D3D12StaticSamplerDescs[SamplerIndex].MipLODBias = RootSignatureArgsIn.Samplers[SamplerIndex].MipLODBias;
			D3D12StaticSamplerDescs[SamplerIndex].MaxAnisotropy = RootSignatureArgsIn.Samplers[SamplerIndex].Anisotropy;
			D3D12StaticSamplerDescs[SamplerIndex].ComparisonFunc = FromComparison(RootSignatureArgsIn.Samplers[SamplerIndex].Comparison);
			D3D12StaticSamplerDescs[SamplerIndex].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			D3D12StaticSamplerDescs[SamplerIndex].MinLOD = RootSignatureArgsIn.Samplers[SamplerIndex].MinLOD;
			D3D12StaticSamplerDescs[SamplerIndex].MaxLOD = RootSignatureArgsIn.Samplers[SamplerIndex].MaxLOD;
			D3D12StaticSamplerDescs[SamplerIndex].ShaderRegister = RootSignatureArgsIn.Samplers[SamplerIndex].RegisterIndex;
			D3D12StaticSamplerDescs[SamplerIndex].RegisterSpace = RootSignatureArgsIn.Samplers[SamplerIndex].RegisterSpace;
			D3D12StaticSamplerDescs[SamplerIndex].ShaderVisibility = FromShaderMask(RootSignatureArgsIn.Samplers[SamplerIndex].ShaderMask);
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC D3D12RootSignatureDesc = {};
		D3D12RootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		D3D12RootSignatureDesc.Desc_1_1.NumParameters = UINT(D3D12RootParamters.GetSize());
		D3D12RootSignatureDesc.Desc_1_1.pParameters = D3D12RootParamters.GetData();
		D3D12RootSignatureDesc.Desc_1_1.NumStaticSamplers = UINT(D3D12StaticSamplerDescs.GetSize());
		D3D12RootSignatureDesc.Desc_1_1.pStaticSamplers = D3D12StaticSamplerDescs.GetData();
		D3D12RootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		TReferPtr<ID3DBlob> SignatureBlob;
		TReferPtr<ID3DBlob> SignatureBlobError;
		HR = D3D12SerializeVersionedRootSignature(&D3D12RootSignatureDesc, SignatureBlob.GetPP(), SignatureBlobError.GetPP());
		if (FAILED(HR))
		{
			LogError(Str( " D3D12SerializeVersionedRootSignature failed, {}, {}"), Win32::ErrorStr(HR & 0xFFFF), D3D12BlobMessage(SignatureBlobError));
			return EError::Inner;
		}

		TReferPtr<ID3D12RootSignature> TempD3D12RootSignature;
		HR = Device->CreateRootSignature(0, SignatureBlob->GetBufferPointer(), SignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), TempD3D12RootSignature.GetVV());
		if (FAILED(HR))
		{
			LogError(Str( " CreateRootSignature failed, {}"), Win32::ErrorStr(HR & 0xFFFF));
			return EError::Inner;
		}

		RootSignatureArgs = RootSignatureArgsIn;
		D3D12RootSignature = TempD3D12RootSignature;
		return EError::OK;
	}

	void FRHID3D12RootSignature::SetName(FStringView Name)
	{
		SetD3D12ObjectName(D3D12RootSignature.Get(), Name);
	}
}
