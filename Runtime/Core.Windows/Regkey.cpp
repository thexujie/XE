#include "PCH.h"
#include "Regkey.h"

namespace XE::Windows
{
	FRegkey::FRegkey(HKEY RootKey, FStringView Path)
	{
		
	}


	HRESULT FRegkey::SetKeyValue(HKEY RootKey, FWStringView Path, FWStringView KeyName, FWStringView KeyValue)
	{
		return HRESULT_FROM_WIN32(RegSetKeyValueW(RootKey, Path.Data, KeyName.Data, REG_SZ, KeyValue.Data, DWORD(KeyValue.Size * sizeof(wchar_t))));
	}
	
	HRESULT FRegkey::DeleteKey(HKEY RootKey, FWStringView KeyPath)
	{
		return HRESULT_FROM_WIN32(RegDeleteTreeW(RootKey, KeyPath.Data));
	}
}