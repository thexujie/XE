#include "PCH.h"
#include "Types.h"

#define ERROR_TAG(tag) tag, #tag

namespace XE
{
	struct error_entry
	{
		EError num;
		const char * tag;
		const char * str;
	};

	static const struct error_entry error_entries[] =
	{
		{ ERROR_TAG(EError::Generic), "generic error" },
		{ ERROR_TAG(EError::Inner), "inner error" },
		{ ERROR_TAG(EError::Eof), "end of stream" },
		{ ERROR_TAG(EError::Nullptr), "null pointer" },
		{ ERROR_TAG(EError::Unreachable), "unreachable" },
		{ ERROR_TAG(EError::OutOfMemory), "out of memory" },
		{ ERROR_TAG(EError::OutOfBound), "out of bound" },
		{ ERROR_TAG(EError::Args), "invalid args" },
		{ ERROR_TAG(EError::Exists), "not exists" },
		{ ERROR_TAG(EError::Access), "no access" },
		{ ERROR_TAG(EError::State), "invalid state" },
		{ ERROR_TAG(EError::IO), "io error" },
		{ ERROR_TAG(EError::Broken), "broken" },
		{ ERROR_TAG(EError::Timeout), "timeout" },
		{ ERROR_TAG(EError::Cancel), "cancel" },
		{ ERROR_TAG(EError::BadData), "bad data" },
		{ ERROR_TAG(EError::BadFormat), "bad format" },
		{ ERROR_TAG(EError::NotNow), "not now" },
		{ ERROR_TAG(EError::NotSupported), "not supported" },
		{ ERROR_TAG(EError::NotSupported), "not found" },
		{ ERROR_TAG(EError::InvalidOperation), "invalid operation" },
		{ ERROR_TAG(EError::OK), "ok" },
		{ ERROR_TAG(EError::True), "true" },
		{ ERROR_TAG(EError::False), "false" },
		{ ERROR_TAG(EError::Pendding), "pendding" },
	};

	const Char * ErrorStr(EError err)
	{
		for (const error_entry & entry : error_entries)
		{
			if (err == entry.num)
				return reinterpret_cast<const Char*>(entry.str);
		}

		return Str("Unknown Error");
	}

	bool IsNan(float32_t Value)
	{
		return std::isnan(Value);
	}

	bool IsNan(float64_t Value)
	{
		return std::isnan(Value);
	}
}