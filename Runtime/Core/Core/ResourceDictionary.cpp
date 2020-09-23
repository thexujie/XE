#include "PCH.h"
#include "ResourceDictionary.h"

#include "Application.h"
#include "Xml.h"

namespace XE
{
	FResourceDictionary::FResourceDictionary(FStringView ResourceDirectoryFilePath)
	{
	}
	
	void FResourceDictionary::MergeResources(FStringView ResourceDirectoryFilePath)
	{
		TReferPtr<IDataProvider> DataProvider = MakeRefer<FXmlDataProvider>(ResourceDirectoryFilePath);

		for (const IDataProvider * ResourceDirectoryDataProvider = DataProvider->GetFirstChild();
			ResourceDirectoryDataProvider != nullptr;
			ResourceDirectoryDataProvider = ResourceDirectoryDataProvider->GetSibling())
		{
			FName ResourceTypeName(ResourceDirectoryDataProvider->GetName());
			FType ResourceType = TypeBy(ResourceTypeName.Name);
			if (!ResourceType)
				return;

			//TReferPtr<FResource> Resource = MakeRefer<FResource>(ResourceType);

			FVariant Key = FVariant::Empty;
			FVariant Resource = ResourceType.CreateValue(ResourceDirectoryDataProvider);

			for (const IDataProvider * ResourceAttributeProvider = ResourceDirectoryDataProvider->GetFirstAttribute();
				ResourceAttributeProvider != nullptr;
				ResourceAttributeProvider = ResourceAttributeProvider->GetSibling())
			{
				FName PropertyName(ResourceAttributeProvider->GetName());
				if (PropertyName.NameSpace == NameSpaces::X)
				{
					FProperty Property = TypeOf<FResource>().GetProperty(PropertyName.Name);
					if (Property == FResource::KeyProperty)
						Key = TypeOf<FVariant>().CreateValue(ResourceAttributeProvider);
				}
				else
				{
				} 
			}

			if (Key)
				Resources[Key] = Resource;
		}

		DataProviders.Add(DataProvider);
	}
}
