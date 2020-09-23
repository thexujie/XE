#pragma once

#include "../EngineInc.h"
#include "../Renderables/ViewRectGeometry.h"

namespace XE::Engine
{
	class ENGINE_API FBlendMeshAssembler : public IMeshAssembler
	{
	public:
		FBlendMeshAssembler()
		{
			VS = MakeShared<FShader>(EShaderType::Vertex, Str("../Content/Shaders/BaseVertexShader.hlsl"), Str("VSMain"));
		}
		~FBlendMeshAssembler() = default;

		EGeometry GetGeometry() const { return EGeometry::Triangle; }
		FShader * GetVertexShader() const { return VS.Get(); }
		FShader * GetHullShader() const { return nullptr; }
		FShader * GetDomainShader() const { return nullptr; }
		FShader * GetGeometryShader() const { return nullptr; }

	private:
		TSharedPtr<FShader> VS;
	};
	CLASS_UUID(FBlendMeshAssembler, "49759F76-E4F1-4E65-8ADB-ADD97B29A597");

	class FBlendPass : public FRenderPass
	{
	public:
		FBlendPass() = default;

		void Initialize(FRenderFactory & RenderFactory,  FRHICommandList & RHICommandList);
		void Setup(FRHIResourceView * DestinationIn, FRHIResource * SourceIn)
		{
			Destination = DestinationIn;
			Source = SourceIn;
		}
		void Update(FRenderFactory & RenderFactory, const FView & SceneView, FRHICommandList & RHICommandList) override {}
		void Excute(FRenderFactory & RenderFactory, const FView & SceneView, FRHICommandList & RHICommandList) override;

	private:
		TPointer<FRHIResourceView> Destination;
		TPointer<FRHIResource> Source;

		TSharedPtr<IRenderTarget> RenderTarget;
		TSharedPtr<FViewRectGeometry> ViewRectGeometry;
	};
}
