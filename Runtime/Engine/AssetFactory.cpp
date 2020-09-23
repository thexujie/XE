#include "PCH.h"
#include "AssetFactory.h"


#include "Engine.h"
#include "EngineResource.h"
#include "Texture.h"

namespace XE::Engine
{
	void FAssetFactory::Initialize()
	{
		LoadThread = FThread(&FAssetFactory::Asset_Run, this);
	}

	void FAssetFactory::Finalize()
	{
		{
			TUniqueLock Lock(LoadMutex);
			LoadRunning = false;
		}
		LoadConditionalVariable.NotifyAll();
		LoadThread.Join();
	}
	
	TSharedPtr<IEngineResource> FAssetFactory::LoadAsset(FStringView AssetPath)
	{
		if (!FFile::Exists(AssetPath))
		{
			LogError(Str("LoadAsset {} failed, file not exists."), AssetPath);
			return nullptr;
		}
		
		FFileStream FileStream(AssetPath, EFileModes::In);
		FDataStream DataStream(FileStream);
		FArchive Archive(DataStream, EArchiveFlags::Input);

		SAssetHeader AssetHeader = {};
		Archive >> AssetHeader;
		if (AssetHeader.Magic != AssetMagic)
		{
			LogError(Str("LoadAsset {} failed, invalid file header."), AssetPath);
			return nullptr;
		}

		TSharedPtr<IEngineResource> Asset = CreateAsset(AssetHeader.AssetType);
		EError Error = Asset->Serialize(Archive);
		if (Error != EError::OK)
		{
			LogError(Str("LoadAsset {} failed, {}."), AssetPath, ErrorStr(Error));
			return nullptr;
		}
		return Asset;
	}

	EError FAssetFactory::SaveResource(FStringView AssetPath, IEngineResource & SceneResource)
	{
		FFileStream FileStream(AssetPath, EFileModes::Out);
		FDataStream DataStream(FileStream);
		FArchive Archive(DataStream, EArchiveFlags::Output);

		SAssetHeader AssetHeader = { AssetMagic, SceneResource.GetAssetType(), AssetVersion };
		Archive >> AssetHeader;
		EError Error = SceneResource.Serialize(Archive);
		if (Error != EError::OK)
		{
			LogError(Str("LoaSaveAsset {} failed, {}."), AssetPath, ErrorStr(Error));
			return Error;
		}
		return EError::OK;
	}

	TSharedPtr<IEngineResource> FAssetFactory::CreateAsset(EAssetType AssetType)
	{
		switch(AssetType)
		{
		case EAssetType::Texture:
			return MakeShared<FTexture>(FEngine::Instance());
		case EAssetType::Mesh:
			return MakeShared<FStaticMeshPrimitive>(FEngine::Instance());
		default:
			return nullptr;
		}
	}

	//TSharedPtr<FAsset> FAssetFactory::CreateAsset(FStringView AssetType)
	//{
	//	return nullptr;
	//}

	FSessionHandle FAssetFactory::LoadAsset_any(FStringView AssetPath, IEngineResource * SceneResource, TFunction<void(IEngineResource *, EError)> CallBack)
	{
		FSessionHandle Handle = Atomics::IncFetch(SessionHandle);
		SSession Session = { Handle, AssetPath, SceneResource, CallBack };
		{
			TUniqueLock Lock(LoadMutex);
			SessionsPendding.Emplace(Session);
		}
		LoadConditionalVariable.NotifyAll();
		return Handle;
	}
	
	void FAssetFactory::Asset_Run()
	{
		FSystem::SetThreadDescription(Str("Asset_Run"));
		while (LoadRunning)
		{
			TVector<SSession> Sessions;
			{
				TUniqueLock Lock(LoadMutex);
				LoadConditionalVariable.Wait(Lock, [this]()
					{
						return !SessionsPendding.IsEmpty() || !LoadRunning;
					});
				Swap(Sessions, SessionsPendding);
			}

			for (SSession & Session : Sessions)
			{
				LoadAsset(Session);
				Session = {};
			}
			
			Sessions.Reset();
		}
	}

	void FAssetFactory::LoadAsset(SSession & Session)
	{
		if (!FFile::Exists(Session.AssetPath))
		{
			LogError(Str("LoadAsset {} failed, file not exists."), Session.AssetPath);
			Session.CallBack(Session.SceneResource, EError::Exists);
			return;
		}

		FFileStream FileStream(Session.AssetPath, EFileModes::In);
		FDataStream DataStream(FileStream);
		FArchive Archive(DataStream, EArchiveFlags::Input);

		SAssetHeader AssetHeader = {};
		Archive >> AssetHeader;
		if (AssetHeader.Magic != AssetMagic)
		{
			LogError(Str("LoadAsset {} failed, invalid file header."), Session.AssetPath);
			Session.CallBack(Session.SceneResource, EError::BadData);
		}

		EError Error = Session.SceneResource->Serialize(Archive);
		if (Error != EError::OK)
			LogError(Str("LoadAsset {} failed, {}."), Session.AssetPath, ErrorStr(Error));
		Session.CallBack(Session.SceneResource, Error);
	}
}

