#pragma once

#include "../Core/Core.h"
#include "../RHI/RHI.h"

#if XE_WINDOWS
#ifdef XEngine_Render_Module
#define RENDER_API __declspec(dllexport)
#else
#define RENDER_API __declspec(dllimport)
#endif
#endif

namespace XE::Render
{
	using namespace RHI;
	
	class FShader;
	class FRenderFactory;
	class FResourceBindings;

	class FView;
	class IMeshAssembler;
	class IMaterial;
	class IOutputLayout;
	class IInputLayout;
	class FPipelineState;

	constexpr size_t ShaderResourceMax = 32;
	
	enum ERootDescriptorTableIndex
	{
		RootDescriptorTableIndex_World = 0,
		// Per Scene
		RootDescriptorTableIndex_Scene,
		// Per PSO
		RootDescriptorTableIndex_VertexFactory,
		RootDescriptorTableIndex_Material,

		RootDescriptorTableIndex_Count,
	};

	enum ERootShaderParameterIndex
	{
		/** Descriptors **/
		// Per Engine
		RootShaderParameterIndex_Engine = 0,
		// Per World
		RootShaderParameterIndex_World,
		// Per Scene
		RootShaderParameterIndex_Scene,
		// Per SceneView
		RootShaderParameterIndex_SceneView,
		// Per PSO
		RootShaderParameterIndex_VertexFactory,
		RootShaderParameterIndex_Material,
		// Per Primitive
		RootShaderParameterIndex_Primitive,

		RootDescriptorTableIndex_Engine,

		RootShaderParameterIndex_Count,
	};

	enum class EVertexFormat
	{
		None = 0,

		PositionF3 = 0x0001,
		TexcoordF2 = 0x0002,
		ColorF3 = 0x0004,

		P3_T2 = PositionF3 | TexcoordF2,
		P3_C3 = PositionF3 | ColorF3,
		P3_T2_C3 = PositionF3 | TexcoordF2 | ColorF3,
	};
	
	using FPipelineStateCacheId = uintx_t;

	enum class EVertexElements
	{
		None = 0x0000,
		Position = 0x0001,
		UV = 0x0002,
		UV2 = 0x0004,
		Normal = 0x0008,
		BiNormal = 0x0010,
		Color = 0x0020
	};
	template struct TFlag<EVertexElements>;

	struct RENDER_API FVertexElement
	{
		FString Name;
		uint8_t StreamIndex = 0;
		uint8_t Offset = 0;
		uint8_t Stride = 0;
		EFormat Format = EFormat::None;

		bool operator ==(const FVertexElement &) const noexcept = default;
		auto operator <=>(const FVertexElement &) const noexcept = default;
		friend FArchive & operator >>(FArchive & Serializer, FVertexElement & VertexElement)
		{
			Serializer >> VertexElement.Name;
			Serializer >> VertexElement.StreamIndex;
			Serializer >> VertexElement.Offset;
			Serializer >> VertexElement.Stride;
			Serializer >> VertexElement.Format;
			return Serializer;
		}
	};

	struct FBoundingBox
	{
		FVector3 Min;
		FVector3 Max;
	};
	
	class RENDER_API IRenderObject
	{
	public:
		virtual ~IRenderObject() = default;
	};

	class RENDER_API ISceneObject
	{
	public:
		virtual ~ISceneObject() = default;
	};
	
	class RENDER_API IRenderResource : public IRenderObject
	{
	public:
		virtual void InitResource() {}
		virtual void FiniResource() {}
	};
}

