#include "PCH.h"
#include "StringView.h"

namespace XE
{
	namespace Strings
	{
		int32_t Stricmp(FAStringView StringLeft, FAStringView StringRight)
		{
			if (!StringLeft.Size || !StringRight.Size)
				return int32_t(StringLeft.Size - StringRight.Size);

			size_t Size = std::min(StringLeft.Size, StringRight.Size);
			int32_t Result = _strnicmp(StringLeft.Data, StringRight.Data, Size);
			if (Result)
				return Result;

			if (StringLeft.Size == StringRight.Size)
				return 0;

			return StringLeft.Size > StringRight.Size ? int32_t(StringLeft[Size]) : int32_t(StringRight[Size]);
		}

		int32_t Stricmp(FU8StringView StringLeft, FU8StringView StringRight)
		{
			if (!StringLeft.Size || !StringRight.Size)
				return int32_t(StringLeft.Size - StringRight.Size);

			size_t Size = std::min(StringLeft.Size, StringRight.Size);
			int32_t Result = _strnicmp((const char *)StringLeft.Data, (const char *)StringRight.Data, Size);
			if (Result)
				return Result;

			if (StringLeft.Size == StringRight.Size)
				return 0;

			return StringLeft.Size > StringRight.Size ? int32_t(StringLeft[Size]) : int32_t(StringRight[Size]);
		}

		int32_t Stricmp(FU16StringView StringLeft, FU16StringView StringRight)
		{
			if (!StringLeft.Size || !StringRight.Size)
				return int32_t(StringLeft.Size - StringRight.Size);

			size_t Size = std::min(StringLeft.Size, StringRight.Size);
			int32_t Result = _wcsnicmp((const wchar_t *)StringLeft.Data, (const wchar_t *)StringRight.Data, Size);
			if (Result)
				return Result;

			if (StringLeft.Size == StringRight.Size)
				return 0;

			return StringLeft.Size > StringRight.Size ? int32_t(StringLeft[Size]) : int32_t(StringRight[Size]);
		}

		int32_t Stricmp(FWStringView StringLeft, FWStringView StringRight)
		{
			if (!StringLeft.Size || !StringRight.Size)
				return int32_t(StringLeft.Size - StringRight.Size);

			size_t Size = std::min(StringLeft.Size, StringRight.Size);
			int32_t Result = _wcsnicmp(StringLeft.Data, StringRight.Data, Size);
			if (Result)
				return Result;

			if (StringLeft.Size == StringRight.Size)
				return 0;

			return StringLeft.Size > StringRight.Size ? int32_t(StringLeft[Size]) : int32_t(StringRight[Size]);
		}

		int32_t Stricmp(FU32StringView StringLeft, FU32StringView StringRight)
		{
			return 0;
		}
	}
}
