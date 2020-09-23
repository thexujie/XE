#include "PCH.h"
#include "Compiler.h"

#include <d3dcompiler.h>
#include <d3d12.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace XE::Render
{
	inline FStringView D3DBlobMessage(ID3DBlob * D3DBlob)
	{
		if (!D3DBlob || D3DBlob->GetBufferSize() < 2)
			return FStringView();

		return D3DBlob ? Strings::ToAuto(FAStringView(static_cast<const char *>(D3DBlob->GetBufferPointer()), D3DBlob->GetBufferSize() - 1)) : FStringView();
	}

	inline FStringView D3DBlobMessage(TReferPtr<ID3DBlob> D3DBlob)
	{
		if (!D3DBlob || D3DBlob->GetBufferSize() < 2)
			return FStringView();

		return D3DBlob ? Strings::ToAuto(FAStringView(static_cast<const char *>(D3DBlob->GetBufferPointer()), D3DBlob->GetBufferSize() - 1)) : FStringView();
	}
	
	inline EResourceViewDimension ToResourceViewDimension(D3D12_SRV_DIMENSION ResourceViewDimension, bool MSAA = false)
	{
		switch (ResourceViewDimension)
		{
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_UNKNOWN:
			return EResourceViewDimension::None;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER:
			return EResourceViewDimension::Buffer;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE1D:
			return EResourceViewDimension::Texture1D;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
			return EResourceViewDimension::Texture1DArray;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D:
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2DMS:
			return EResourceViewDimension::Texture2D;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
			return EResourceViewDimension::Texture2DArray;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE3D:
			return EResourceViewDimension::Texture3D;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURECUBE:
			return EResourceViewDimension::TextureCube;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
			return EResourceViewDimension::TextureCubeArray;
		default:
			return EResourceViewDimension::None;
		}
	}
	
	EShaderParameterType ToEShaderParameterType(D3D_SHADER_INPUT_TYPE ShaderParameterType)
	{
		switch(ShaderParameterType)
		{
		case D3D_SIT_CBUFFER: return EShaderParameterType::ConstBuffer;
		case D3D_SIT_TBUFFER: return EShaderParameterType::None;
		case D3D_SIT_TEXTURE: return EShaderParameterType::ShaderResource;
		case D3D_SIT_SAMPLER: return EShaderParameterType::Sampler;
		case D3D_SIT_STRUCTURED: return EShaderParameterType::ShaderResource;
		case D3D_SIT_UAV_RWTYPED: return EShaderParameterType::UnorderedResource;
		case D3D_SIT_UAV_RWSTRUCTURED: return EShaderParameterType::UnorderedResource;
		case D3D_SIT_BYTEADDRESS: return EShaderParameterType::None;
		case D3D_SIT_UAV_RWBYTEADDRESS: return EShaderParameterType::UnorderedResource;
		case D3D_SIT_UAV_APPEND_STRUCTURED: return EShaderParameterType::UnorderedResource;
		case D3D_SIT_UAV_CONSUME_STRUCTURED: return EShaderParameterType::UnorderedResource;
		case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER: return EShaderParameterType::UnorderedResource;
		default: return EShaderParameterType::None;
		}
	}

	void FShaderReflection::Combine(FShaderReflection & That)
	{
		for (FShaderParameterDesc & ConstBufferParameterThat : That.ConstBufferParameters)
		{
			for (FShaderParameterDesc & ConstBufferParameterThis : ConstBufferParameters)
			{
				if (ConstBufferParameterThis.Name == ConstBufferParameterThat.Name)
				{
				}
			}
			
		}
	}
	
	class FD3DBlob : public FBlob
	{
	public:
		FD3DBlob(TReferPtr<ID3DBlob> BlobIn) : Blob(BlobIn) {}
		
		TView<byte_t> GetData() const override
		{
			if (!Blob)
				return nullptr;
			
			return TView<byte_t>(static_cast<const byte_t*>(Blob->GetBufferPointer()), Blob->GetBufferSize());
		}

	private:
		TReferPtr<ID3DBlob> Blob;
	};

	class FCompilerInclude : public ID3DInclude
	{
	public:
		FCompilerInclude() = default;
		
		HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT * pBytes) override
		{
			FString FileName= FPath::Combine(FPath::Combine(FPath::GetWorkingDirectory(),  Str("../Content/Shaders")), Strings::FromAnsi(pFileName));
			FileBytes = FFile::ReadToBytes(FileName);
			*ppData = FileBytes.GetData();
			*pBytes = UINT(FileBytes.GetSize());
			return S_OK;
		}
		
		HRESULT Close(LPCVOID pData) override
		{
			FileBytes.Clear();
			return S_OK;
		}

	private:
		TVector<byte_t> FileBytes;
	};

	FStringView FCompiler::GetDefaultTarget(EShaderType Shader)
	{
		switch(Shader)
		{
		case EShaderType::Vertex: return Str("vs_5_0");
		case EShaderType::Hull: return Str("hs_5_0");
		case EShaderType::Domain: return Str("ds_5_0");
		case EShaderType::Geoetry: return Str("gs_5_0");
		case EShaderType::Pixel: return Str("ps_5_0");
		case EShaderType::Compute: return Str("cs_5_0");
		default: return Str("unknown");
		}
	}
	TSharedPtr<FBlob> FCompiler::CompileFile(FStringView FilePath, FStringView Entry, FStringView Target, uint32_t CompileFlags)
	{
		std::wstring FilePathW = Strings::ToStdWString(FilePath);
		std::string MainEntryA = Strings::ToStdString(Entry);
		std::string TargetA = Strings::ToStdString(Target);

		TReferPtr<ID3DBlob> ErrorBlob;
		TReferPtr<ID3DBlob> ShaderBlob;

		FCompilerInclude CompilerInclude;
		HRESULT Result = D3DCompileFromFile(FilePathW.c_str(), nullptr, &CompilerInclude, MainEntryA.c_str(), TargetA.c_str(), CompileFlags, 0, ShaderBlob.GetPP(), ErrorBlob.GetPPSafe());
		if (Result)
		{
			LogInfo(Str("D3DCompileFromFile failed, 0x{:x}, \n{}"), uint32_t(Result), D3DBlobMessage(ErrorBlob));
			return nullptr;
		}
		
		return MakeShared<FD3DBlob>(ShaderBlob);
	}

	static EFormat ToRWFormat(D3D_RESOURCE_RETURN_TYPE ReturnType, uint32_t Flags)
	{
		uint32_t Component4Flags = D3D_SIF_TEXTURE_COMPONENT_0 | D3D_SIF_TEXTURE_COMPONENT_1;
		uint32_t Component3Flags = D3D_SIF_TEXTURE_COMPONENT_1;
		uint32_t Component2Flags = D3D_SIF_TEXTURE_COMPONENT_0;
		
		if (ReturnType == D3D_RESOURCE_RETURN_TYPE::D3D_RETURN_TYPE_FLOAT)
		{
			if ((Flags & Component4Flags) == Component4Flags)
				return  EFormat::Float4;
			else if ((Flags & Component3Flags) == Component3Flags)
				return  EFormat::Float3;
			else if ((Flags & Component2Flags) == Component2Flags)
				return  EFormat::Float2;
			else
				return  EFormat::Float1;
		}
		else if (ReturnType == D3D_RESOURCE_RETURN_TYPE::D3D_RETURN_TYPE_SINT)
		{
			if ((Flags & Component4Flags) == Component4Flags)
				return  EFormat::Int4;
			else if ((Flags & Component3Flags) == Component3Flags)
				return  EFormat::Int3;
			else if ((Flags & Component2Flags) == Component2Flags)
				return  EFormat::Int2;
			else
				return  EFormat::Int1;
		}
		else if (ReturnType == D3D_RESOURCE_RETURN_TYPE::D3D_RETURN_TYPE_UINT)
		{
			if ((Flags & Component4Flags) == Component4Flags)
				return  EFormat::UInt4;
			else if ((Flags & Component3Flags) == Component3Flags)
				return  EFormat::UInt3;
			else if ((Flags & Component2Flags) == Component2Flags)
				return  EFormat::UInt2;
			else
				return  EFormat::UInt1;
		}
		else
		{
			return  EFormat::None;
		}
	}
	
	FShaderReflection FCompiler::Reflect(FBlob * Code, EShaderMasks ShaderMask)
	{
		ID3D12ShaderReflection * D3D12ShaderReflection = nullptr;
		HRESULT HR = D3DReflect(Code->GetData().Data, Code->GetData().Size, __uuidof(ID3D12ShaderReflection), reinterpret_cast<void **>(&D3D12ShaderReflection));
		if (HR)
			return {};

		D3D12_SHADER_DESC D3D12ShaderDesc = {};
		D3D12ShaderReflection->GetDesc(&D3D12ShaderDesc);

		/**
		 * Constant Buffers
		 */
		FShaderReflection ShaderReflection;

		for (uint32_t ResourceIndex = 0; ResourceIndex < D3D12ShaderDesc.BoundResources; ++ResourceIndex)
		{
			D3D12_SHADER_INPUT_BIND_DESC D3D12ResourceBindDesc = {};
			D3D12ShaderReflection->GetResourceBindingDesc(ResourceIndex, &D3D12ResourceBindDesc);

			FShaderParameterDesc ShaderParameterDesc;
			ShaderParameterDesc.ShaderMask = ShaderMask;
			ShaderParameterDesc.Type = ToEShaderParameterType(D3D12ResourceBindDesc.Type);
			ShaderParameterDesc.Name = Strings::FromAnsi(D3D12ResourceBindDesc.Name);
			ShaderParameterDesc.RegisterIndex = D3D12ResourceBindDesc.BindPoint;
			ShaderParameterDesc.RegisterCount = D3D12ResourceBindDesc.BindCount;
			ShaderParameterDesc.RegisterSpace = D3D12ResourceBindDesc.Space;
			ShaderParameterDesc.Dimension = ToResourceViewDimension(D3D12_SRV_DIMENSION(D3D12ResourceBindDesc.Dimension), D3D12ResourceBindDesc.NumSamples > 0);
			ShaderParameterDesc.RWTypeFormat = ToRWFormat(D3D12ResourceBindDesc.ReturnType, D3D12ResourceBindDesc.uFlags);
			ShaderParameterDesc.ElememtSize = D3D12ResourceBindDesc.NumSamples;
			
			switch(D3D12ResourceBindDesc.Type)
			{
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER:
			{
				//ID3D12ShaderReflectionConstantBuffer * D3D12ShaderReflectionConstantBuffer = D3D12ShaderReflection->GetConstantBufferByIndex(D3D12ResourceBindDesc.BindPoint);
				//if (D3D12ShaderReflectionConstantBuffer)
				//{
				//	D3D12_SHADER_BUFFER_DESC D3D12ShaderBufferDesc;
				//	D3D12ShaderReflectionConstantBuffer->GetDesc(&D3D12ShaderBufferDesc);
				//}
				break;
			}
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_TBUFFER:
				break;
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE:
				break;
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED:
			{
				ShaderParameterDesc.RWType = EShaderParameterRWType::Struct;
				break;
			}
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED:
				ShaderParameterDesc.RWType = EShaderParameterRWType::Typed;
				break;
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED:
				ShaderParameterDesc.RWType = EShaderParameterRWType::Struct;
				break;
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWBYTEADDRESS:
				ShaderParameterDesc.RWType = EShaderParameterRWType::ByteAddress;
				break;
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_APPEND_STRUCTURED:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_CONSUME_STRUCTURED:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
				break;
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER:
				break;
			default:
				break;
			}

			switch(ShaderParameterDesc.Type)
			{
			case EShaderParameterType::ConstBuffer:
				ShaderReflection.ConstBufferParameters.Add(ShaderParameterDesc);
				break;
			case EShaderParameterType::ShaderResource:
				ShaderReflection.ShaderResourceParemeters.Add(ShaderParameterDesc);
				break;
			case EShaderParameterType::UnorderedResource:
				ShaderReflection.UnorderedResourceParemetersDesc.Add(ShaderParameterDesc);
				break;
			case EShaderParameterType::Sampler:
				ShaderReflection.SamplerParemetersDesc.Add(ShaderParameterDesc);
				break;
			default: ;
			}
		}

		for (uint32_t ConstantIndex = 0; ConstantIndex < D3D12ShaderDesc.ConstantBuffers; ++ConstantIndex)
		{
			ID3D12ShaderReflectionConstantBuffer * D3D12ShaderReflectionConstantBuffer = D3D12ShaderReflection->GetConstantBufferByIndex(ConstantIndex);
			if (!D3D12ShaderReflectionConstantBuffer)
				continue;

			D3D12_SHADER_BUFFER_DESC D3D12ShaderBufferDesc;
			D3D12ShaderReflectionConstantBuffer->GetDesc(&D3D12ShaderBufferDesc);
			if (D3D12ShaderBufferDesc.Type != D3D_CT_CBUFFER)
				continue;

			FShaderParameterDesc * ShaderParameterDesc = FindIf(ShaderReflection.ConstBufferParameters.Begin(), ShaderReflection.ConstBufferParameters.End(), 
				[&](const FShaderParameterDesc & Parameter) { return Parameter.Name == Strings::FromAnsi(D3D12ShaderBufferDesc.Name); });
			if (!ShaderParameterDesc)
				continue;
			
			//const FShaderParameterDesc * ShaderParameterDesc = ShaderReflection.FindConstBufferByName(FString(D3D12ShaderBufferDesc.Name));
			//if (!ShaderParameterDesc)
				//continue;

			/**
			 * Variables in this constant buffer
			 */
			for (uint32_t VariableIndex = 0; VariableIndex < D3D12ShaderBufferDesc.Variables; ++VariableIndex)
			{
				ID3D12ShaderReflectionVariable * D3D12ShaderReflectionVariable = D3D12ShaderReflectionConstantBuffer->GetVariableByIndex(VariableIndex);
				if (!D3D12ShaderReflectionVariable)
					continue;

				D3D12_SHADER_VARIABLE_DESC D3D12ShaderVariableDesc;
				D3D12ShaderReflectionVariable->GetDesc(&D3D12ShaderVariableDesc);

				FShaderVariableDesc ShaderVariableDesc;
				ShaderVariableDesc.Name = Strings::FromAnsi(D3D12ShaderVariableDesc.Name);
				ShaderVariableDesc.RegisterIndex = ShaderParameterDesc->RegisterIndex;
				ShaderVariableDesc.Offset = D3D12ShaderVariableDesc.StartOffset;
				ShaderVariableDesc.Size = D3D12ShaderVariableDesc.Size;
				ShaderReflection.Variables.Add(ShaderVariableDesc);
			}
		}
		
		return ShaderReflection;
	}

}
