#pragma once

#include "CoreWindowsInc.h"

namespace XE::Windows
{
	class CORE_WINDOWS_API FRegkey
	{
	public:
		FRegkey() = default;
		FRegkey(HKEY RootKey, FStringView Path);


	public:
		static HRESULT SetKeyValue(HKEY RootKey, FWStringView Path, FWStringView KeyName, FWStringView KeyValue);
		static HRESULT DeleteKey(HKEY RootKey, FWStringView KeyPath);
	};
}
