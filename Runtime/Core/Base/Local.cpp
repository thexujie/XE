#include "PCH.h"
#include "Local.h"

namespace XE
{
	namespace Locals
	{
		_locale_t Ansi()
		{
			static TUniquePtr<__crt_locale_pointers, decltype(_free_locale) *> AnsiLocal(_create_locale(LC_CTYPE, ".ACP"), _free_locale);
			return AnsiLocal.Get();
		}

		_locale_t Local()
		{
			return _get_current_locale();
		}
		
		_locale_t Utf8()
		{
			static TUniquePtr<__crt_locale_pointers, decltype(_free_locale) *> AnsiLocal(_create_locale(LC_CTYPE, ".UTF8"), _free_locale);
			return AnsiLocal.Get();
		}
	}
}
