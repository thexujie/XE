#include "PCH.h"
#include "UIFactory.h"

#include "ContentControl.h"

namespace XE::UI
{
	FUIFactory::FUIFactory()
	{
		UIPath = FPath::GetAbsolutePath(Str("../Content/UI"));
	}
	
	TReferPtr<UControl> FUIFactory::LoadFromXml(FStringView XmlFilePath)
	{
		FXmlDocument XmlDocument(FPath::Combine(UIPath, XmlFilePath));
		FXmlNode RootNode = XmlDocument.GetRootNode();
		
		if (!RootNode)
			return nullptr;

		return LoadFromXmlNode(RootNode);
	}

	TReferPtr<UControl> FUIFactory::LoadFromXmlNode(const FXmlNode & Node)
	{
		FType Type = TypeBy(Node.GetName());
		static TReferPtr<FXmlNodeDataProvider> XmlValueProvider = MakeRefer<FXmlNodeDataProvider>(Node);
		FVariant Variant = Type.CreateValue(XmlValueProvider.Get());
		TReferPtr<UControl> Control = Variant.Get<TReferPtr<UControl>>();
		Control->InitializeUI();
		return Control;
	}
}
