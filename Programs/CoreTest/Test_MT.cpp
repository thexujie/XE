#include "PCH.h"

#include <iostream>

#ifdef XE_TEST


namespace XE
{
	class Test_MT
	{
	public:
		FFileStream LogStream;
		FRegex RegexShortName_WithPics = FRegex(Str(R"(^(.*)\[.*\])"), ERegexFlags::IgnoreCase);

		Test_MT()
		{
			std::locale loc("chs");
			std::wcout.imbue(loc);

			LogStream.Open(Str("MT.log"), EFileModes::Out | EFileModes::Append);
		}
	}Test_MT_Instance;
}

#endif
