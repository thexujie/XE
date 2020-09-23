#pragma once

#include "EngineInc.h"
#include "AssetFactory.h"
#include "Texture.h"
#include "StaticMeshPrimitive.h"

namespace XE::Engine
{
	class ENGINE_API FEngine
	{
	public:
		FEngine();
		static FEngine & Instance();
		
		void Initialize(FRHIFactory & RHIFactory);
		void Finalize();
		
	public:
		TSharedPtr<FScene> CreateScene();
		FRenderFactory & GetRenderFactory() const { return *RenderFactory; }
		FAssetFactory & GetAssetFactory();

		void FlushScene(size_t SceneIndex = NullIndex);
		void FlushCommands();

		template<typename LambdaT>
		void EnqueueCommand(LambdaT && Lambda)
		{
			TUniqueLock Lock(CommandListMutex);
			RenderCommandList.Excute(Forward<LambdaT>(Lambda));
		}
		
	private:
		void Render_Run();
		
	private:
		FAssetFactory AssetFactory;

		TVector<TSharedPtr<FScene>> Scenes;

		TSharedPtr<FRHIDevice> RHIDevice;
		TSharedPtr <FRHICommandQueue> RHICommandQueue;

		FSignal FenceSignal;
		uint64_t FenceValue = 0;
		TSharedPtr <FRHIFence> RHIFence;

		TSharedPtr<FRenderFactory> RenderFactory;

		uint32_t NumBackBuffers = 3;

		bool RenderRuning = true;
		bool RenderFlushing = false;
		uintx_t SceneMaskFlushPendding = 0;
		
		FThread RenderThread;
		FMutex RenderMutex;
		FConditionalVariable RenderConditionalVariable;

		FMutex CommandListMutex;
		TCommandList<FRenderFactory &, FRHICommandList &> RenderCommandList;
		TCommandList<FRenderFactory &, FRHICommandList &> RenderCommandListPendding;

		FMutex AsyncMutex;
		FConditionalVariable AsyncConditionalVariable;
	};
}