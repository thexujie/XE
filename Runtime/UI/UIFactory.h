#pragma once

#include "UIInc.h"
#include "Control.h"

namespace XE::UI
{
	class UI_API FUIFactory : public FUIObject
	{
	public:
		FUIFactory();

		TReferPtr<UControl> LoadFromXml(FStringView XmlFilePath);
		TReferPtr<UControl> LoadFromXmlNode(const FXmlNode & Node);

	private:
		FString UIPath;
		FString PresentersPath;
		FString TemplatesPath;
	};
}
