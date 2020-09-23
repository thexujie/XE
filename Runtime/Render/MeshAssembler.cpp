#include "PCH.h"
#include "MeshAssembler.h"
#include "Shader.h"

namespace XE::Render
{
	FLocalMeshAssembler BaseShader_LocalMeshAssembler(Str("../Content/Shaders/BaseVertexShader.hlsl"));

	FLocalMeshAssembler::FLocalMeshAssembler()
	{
		FMeshAssemblerFactory::Instance().RegisterMeshAssembler(this);
	}

	FLocalMeshAssembler::FLocalMeshAssembler(FStringView VShaderFilePath) : FLocalMeshAssembler()
	{
		VS = MakeShared<FShader>(EShaderType::Vertex, VShaderFilePath, Str("VSMain"));
	}
	
	FMeshAssemblerFactory & FMeshAssemblerFactory::Instance()
	{
		static FMeshAssemblerFactory GMeshAssemblerFactory;
		return GMeshAssemblerFactory;
	}
	
	void FMeshAssemblerFactory::RegisterMeshAssembler(const IMeshAssembler * MeshAssembler)
	{
		MeshAssemblers.Add(MeshAssembler);
	}
	
	//TMeshAssemblerRegister
}
