#pragma once

#include "EngineInc.h"
#include "ScenePrimitive.h"

namespace XE::Engine
{
	class ENGINE_API FScene
	{
	public:
		FScene() = default;
		
		void BeginPlay();
		void EndPlay();

		void SetSceneIndex(size_t IndexIn) { Index = IndexIn ; }
		
		void AddPrimitive(IScenePrimitive * ScenePrimitive);
		const TVector<IScenePrimitive *> & GetPrimitives() const { return Primitives; }

		void AddView(FView * View) { Views.Add(View); }
		const TVector<FView *> & GetViews() const { return Views; }
		
		FShaderParameter * GetShaderParameter() const { return ConstBuffer.Get(); }

	private:
		size_t Index = 0;
		TVector<IScenePrimitive *> Primitives;

		TVector<FView *> Views;
		
		TSharedPtr<FShaderParameter> ConstBuffer;
	};
}
