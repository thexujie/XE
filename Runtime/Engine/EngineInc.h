#pragma once

#include "Render/Render.h"
#include "Graphics/Graphics.h"

#if XE_WINDOWS
#ifdef XEngine_Engine_Module
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif
#endif

namespace XE::Engine
{
	using namespace RHI;
	using namespace Render;

	class FEngine;
	class FRenderer;

	class IEngineResource;
	class FAssetFactory;
	
	class FScene;
	class FShaderParameter;

	class FVertexBuffer;

	using FSessionHandle = uintx_t;

	enum class EAssetType : uint32_t
	{
		None = 0,
		Texture = MakeFourCC("xtex"),
		Mesh = MakeFourCC("xmsh"),
		SkinedMesh = MakeFourCC("xsmh"),
		Map = MakeFourCC("xmap"),
	};
	
	class ENGINE_API IRenderable : public FObject
	{
	public:
		virtual void Render(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList) = 0;
	};
}
