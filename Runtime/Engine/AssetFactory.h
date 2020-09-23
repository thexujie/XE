#pragma once

#include "EngineInc.h"

namespace XE::Engine
{
	constexpr uint32_t AssetMagic = MakeFourCC("xast");
	constexpr uint32_t AssetVersion = 0;

	struct SAssetHeader
	{
		uint32_t Magic = 0;
		EAssetType AssetType = EAssetType::None;
		uint32_t AssetVersion = 0;

		friend FArchive & operator >>(FArchive & Serializer, SAssetHeader & AssetHeader)
		{
			Serializer >> AssetHeader.Magic;
			Serializer >> AssetHeader.AssetType;
			Serializer >> AssetHeader.AssetVersion;
			return Serializer;
		}
	};
	
	class ENGINE_API FAssetFactory
	{
	public:
		FAssetFactory() = default;
		~FAssetFactory() = default;

		void Initialize();
		void Finalize();
		
		TSharedPtr<IEngineResource> LoadAsset(FStringView AssetPath);
		EError SaveResource(FStringView AssetPath, IEngineResource & SceneResource);
		TSharedPtr<IEngineResource> CreateAsset(EAssetType AssetType);
		//TSharedPtr<FAsset> CreateAsset(FStringView AssetType);

		FSessionHandle LoadAsset_any(FStringView AssetPath, IEngineResource * SceneResource, TFunction<void(IEngineResource *, EError)> CallBack);

	private:
		void Asset_Run();
		
	private:
		FSessionHandle SessionHandle = 0;

		struct SSession
		{
			FSessionHandle SessionHandle = 0;
			FString AssetPath;
			IEngineResource * SceneResource = nullptr;
			TFunction<void(IEngineResource *, EError)> CallBack;
		};
		void LoadAsset(SSession & Session);

		TVector<SSession> SessionsPendding;

		bool LoadRunning = true;
		FThread LoadThread;
		FMutex LoadMutex;
		FConditionalVariable LoadConditionalVariable;
	};
}
