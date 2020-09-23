#include "PCH.h"
#include "Engine.h"
#include "Scene.h"

namespace XE::Engine
{
	static FEngine * GEnginePointer = nullptr;

	FEngine::FEngine()
	{
		GEnginePointer = this;
	}
	
	FEngine & FEngine::Instance()
	{
		return *GEnginePointer;
	}
	
	void FEngine::Initialize(FRHIFactory & RHIFactory)
	{
		AssetFactory.Initialize();

		TVector<RHI::FRHIAdapterDesc> RHIAdapterDescs = RHIFactory.GetAdapterDescs();
		RHIDevice = RHIFactory.CreateDevice(RHIAdapterDescs[0].DeviceId);
		RHICommandQueue = RHIDevice->CreateCommandQueue(ECommandType::Direct, ECommandQueueFlags::None);

		RenderFactory = MakeShared<FRenderFactory>(*RHIDevice, *RHICommandQueue);
		RenderThread = FThread(&FEngine::Render_Run, this);
	}

	void FEngine::Finalize()
	{
		AssetFactory.Finalize();
		{
			TUniqueLock Lock(RenderMutex);
			RenderRuning = false;
		}
		RenderConditionalVariable.NotifyAll();
		RenderThread.Join();
	}
	
	TSharedPtr<FScene> FEngine::CreateScene()
	{
		TSharedPtr<FScene> Scene = MakeShared<FScene>();
		Scene->SetSceneIndex(Scenes.Size);
		Scenes.Add(Scene);
		return Scene;
	}
	
	FAssetFactory & FEngine::GetAssetFactory()
	{
		return AssetFactory;
	}

	void FEngine::FlushScene(size_t SceneIndex)
	{
		{
			TUniqueLock Lock(RenderMutex);
			if (SceneIndex == NullIndex)
				SceneMaskFlushPendding = NullIndex;
			else
				SetBit(SceneMaskFlushPendding, SceneIndex, true);
			RenderCommandListPendding.Append(RenderCommandList);
			RenderCommandList.Reset();
		}
		
		{
			TUniqueLock Lock(AsyncMutex);
			RenderConditionalVariable.NotifyAll();
			AsyncConditionalVariable.Wait(Lock);
			// Handle spurious wake
		}
	}

	void FEngine::FlushCommands()
	{
		//{
		//	TUniqueLock Lock(RenderMutex);
		//	RenderCommandListPendding.Append(RenderCommandList);
		//	RenderCommandList.Reset();
		//}
		//RenderConditionalVariable.NotifyAll();
	}
	
	void FEngine::Render_Run()
	{
		FSystem::SetThreadDescription(Str("Render_Run"));

		TCommandList<FRenderFactory &, FRHICommandList &> RenderCommandListExcute;

		FRHICommandList & RHICommandList = RenderFactory->AllcoateRHICommandList(ECommandType::Direct);
		RHIFence = RHIDevice->CreateFence(EFenceFlags::None);

		while (RenderRuning)
		{
			uintx_t SceneMaskFlush = 0;
			{
				TUniqueLock Lock(RenderMutex);
				RenderConditionalVariable.Wait(Lock, [this]()
					{
						return !RenderCommandListPendding.IsEmpty() || !RenderRuning || SceneMaskFlushPendding != 0;
					});
				Swap(RenderCommandListPendding, RenderCommandListExcute);
				SceneMaskFlush = SceneMaskFlushPendding;
				SceneMaskFlushPendding = 0;
			}

			RHICommandList.Reset();

			if (!RenderCommandListExcute.IsEmpty())
			{
				RHICommandList.Open();
				RenderCommandListExcute.Flush(*RenderFactory, RHICommandList);
				RHICommandList.Close();
				RHICommandQueue->ExcuteCommandList(&RHICommandList);
			}

			if (SceneMaskFlush)
			{
				for (size_t SceneIndex = 0; SceneIndex < Scenes.Size; ++SceneIndex)
				{
					if (!GetBit(SceneMaskFlush, SceneIndex))
						continue;

					FScene & Scene = *Scenes[SceneIndex];

					const TVector<FView *> & Views = Scene.GetViews();
					for (size_t SceneViewIndex = 0; SceneViewIndex < Views.Size; ++SceneViewIndex)
					{
						FView & View = *Views[SceneViewIndex];
						RHICommandList.Open();
						const TVector<IScenePrimitive *> & Primitives = Scene.GetPrimitives();
						for (size_t PrimitiveIndex = 0; PrimitiveIndex < Primitives.GetSize(); ++PrimitiveIndex)
						{
							IScenePrimitive * ScenePrimitive = Primitives[PrimitiveIndex];
							ScenePrimitive->Render(*RenderFactory, View, RHICommandList);
						}
						RHICommandList.Close();
						RHICommandQueue->ExcuteCommandList(&RHICommandList);

						View.Render(*RenderFactory, *RHICommandQueue, RHICommandList);
						View.PrimitiveRenderCommands.Reset();

						RHICommandQueue->Signal(RHIFence.Get(), ++FenceValue);

						RHIFence->Wait(FenceValue, &FenceSignal);

						RenderFactory->EndFrame();

						RenderFlushing = false;
					}
				}
			}
			else
			{
				RHICommandQueue->Signal(RHIFence.Get(), ++FenceValue);
				RHIFence->Wait(FenceValue, &FenceSignal);
			}

			{
				TUniqueLock Lock(AsyncMutex);
				AsyncConditionalVariable.NotifyOne();
			}
		}
	}
}
