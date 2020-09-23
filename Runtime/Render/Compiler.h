#pragma once

#include "RenderInc.h"

namespace XE::Render
{
	enum class EShaderParameterType
	{
		None= 0,
		ConstBuffer,
		ShaderResource,
		UnorderedResource,
		Sampler,
		Struct,
	};

	enum class EShaderParameterRWType
	{
		None = 0,
		Typed,
		Struct,
		ByteAddress
	};

	struct FShaderParameterDesc
	{
		EShaderMasks ShaderMask = EShaderMasks::None;
		EShaderParameterType Type = EShaderParameterType::None;
		FString Name;
		uint16_t RegisterIndex = UInt16Max;
		uint16_t RegisterCount = 0;
		uint16_t RegisterSpace = 0;
		EResourceViewDimension Dimension = EResourceViewDimension::None;
		EShaderParameterRWType RWType = EShaderParameterRWType::None;
		EFormat RWTypeFormat = EFormat::None;
		uint32_t ElememtSize = 0;
	};

	enum class EShaderVariableType
	{
		None = 0,
	};
	
	struct FShaderVariableDesc
	{
		FString Name;
		EShaderVariableType Type = EShaderVariableType::None;
		uint32_t RegisterIndex = 0;
		uint32_t Offset = 0;
		uint32_t Size = 0;
	};

	inline constexpr uint32_t ShaderInvalidRegisterIndex = UInt32Max;
	struct FShaderReflection
	{
		TVector<FShaderParameterDesc> ConstBufferParameters;
		TVector<FShaderParameterDesc> ShaderResourceParemeters;
		TVector<FShaderParameterDesc> UnorderedResourceParemetersDesc;
		TVector<FShaderParameterDesc> SamplerParemetersDesc;
		TVector<FShaderVariableDesc> Variables;

		int32_t GetShaderParameterRegisterIndex(FStringView Name) const
		{
			for (const auto & ShaderParameterDesc : ConstBufferParameters)
			{
				if (ShaderParameterDesc.Name == Name)
					return ShaderParameterDesc.RegisterIndex;
			}
			return ShaderInvalidRegisterIndex;
		}
		
		const FShaderParameterDesc * FindConstBufferByName(FStringView Name) const
		{
			for (const auto & ShaderParameterDesc : ConstBufferParameters)
			{
				if (ShaderParameterDesc.Name == Name)
					return &ShaderParameterDesc;
			}
			return nullptr;
		}

		const FShaderParameterDesc * FindShaderResourceByName(FStringView Name) const
		{
			for (const auto & ShaderResourceParemeter : ShaderResourceParemeters)
			{
				if (ShaderResourceParemeter.Name == Name)
					return &ShaderResourceParemeter;
			}
			return nullptr;
		}

		void Combine(FShaderReflection & That);
	};
	
	class RENDER_API FCompiler
	{
	public:
		FCompiler() = delete;

		static FStringView GetDefaultTarget(EShaderType Shader);
		static TSharedPtr<FBlob> CompileFile(FStringView FilePath, FStringView Entry, FStringView Target, uint32_t CompileFlags = 0);
		static FShaderReflection Reflect(FBlob * Code, EShaderMasks ShaderMask);
	};
}

