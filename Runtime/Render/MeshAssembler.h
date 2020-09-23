#pragma once

#include "RenderInc.h"

namespace XE::Render
{
	class RENDER_API IMeshAssembler : public IRenderResource
	{
	public:
		virtual EGeometry GetGeometry() const = 0;
		virtual FShader * GetVertexShader() const = 0;
		virtual FShader * GetHullShader() const = 0;
		virtual FShader * GetDomainShader() const = 0;
		virtual FShader * GetGeometryShader() const = 0;
	};

	class RENDER_API FLocalMeshAssembler : public IMeshAssembler
	{
	public:
		FLocalMeshAssembler();
		FLocalMeshAssembler(TSharedPtr<FShader> VSIn) : VS(VSIn) {}
		FLocalMeshAssembler(FStringView VShaderFilePath);
		~FLocalMeshAssembler() = default;

		EGeometry GetGeometry() const { return EGeometry::Triangle; }
		FShader * GetVertexShader() const override { return VS.Get(); }
		FShader * GetHullShader() const override { return nullptr; }
		FShader * GetDomainShader() const override { return nullptr; }
		FShader * GetGeometryShader() const override { return nullptr; }

	private:
		TSharedPtr<FShader> VS;
	};

	
	class RENDER_API FMeshAssemblerFactory
	{
	public:
		FMeshAssemblerFactory() = default;

		void RegisterMeshAssembler(const IMeshAssembler * MeshAssembler);

	private:
		mutable TVector<const IMeshAssembler *> MeshAssemblers;

	public:
		static FMeshAssemblerFactory & Instance();
	};
	inline FMeshAssemblerFactory & GMeshAssemblerFactory = FMeshAssemblerFactory::Instance();

	RENDER_API extern FLocalMeshAssembler BaseShader_LocalMeshAssembler;
}
