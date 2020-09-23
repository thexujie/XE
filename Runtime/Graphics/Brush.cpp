#include "PCH.h"
#include "Brush.h"

namespace XE
{
	FVariant TDataConverter<FBrush>::Convert(const IDataProvider * DataProvider) const
	{
		//if (const IDataProvider * ContentDataProvider = DataProvider->GetContent(); ContentDataProvider)
		//{
		//	FStringView TypeName = DataProvider->GetName();
		//	return FVariant();
		//}
		//else
		{
			FColor32 Color = Parse<FColor32>(DataProvider->GetValue());
			TReferPtr<FSolidBrush> SolidBrush = MakeRefer<FSolidBrush>(Color);

			return FVariant(SolidBrush);
		}
	}
}