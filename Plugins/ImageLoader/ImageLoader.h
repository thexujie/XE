#pragma once

#include "ImageLoaderInc.h"

namespace XE
{
	class IMAGE_LOADER_API FImageLoader
	{
	public:
		FImageLoader() = default;
		
		static TSharedPtr<FImage> LoadImageFromFile(FStringView FileName);
		static TSharedPtr<Engine::FTexture> LoadFromFile(FStringView FileName);
	};
}
