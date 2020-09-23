#pragma once

#include "RenderInc.h"
#include "Compiler.h"

namespace XE::Render
{
	class RENDER_API FShader
	{
	public:
		FShader(EShaderType ShaderTypeIn, FString PathIn, FString EntryIn) : ShaderType(ShaderTypeIn), Path(PathIn), Entry(EntryIn),
			Target (FCompiler::GetDefaultTarget(ShaderTypeIn))
		{
			CodeBlob = FCompiler::CompileFile(Path, Entry, Target, 0);
			ShaderReflection = FCompiler::Reflect(CodeBlob.Get(), ShaderMasksFromShaderType(ShaderTypeIn));
		}

		EShaderType GetType() const { return ShaderType; }
		TView<byte_t> GetCode() const { return CodeBlob ? CodeBlob->GetData() : nullptr; }
		const FShaderReflection & GetShaderReflection() const { return ShaderReflection; }
		
	private:
		EShaderType ShaderType;
		FString Path;
		FString Entry;
		FString Target;
		TSharedPtr<FBlob> CodeBlob;
		FShaderReflection ShaderReflection;
	};
}

