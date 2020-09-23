#include "PCH.h"
#include "StringStream.h"

namespace XE
{
	FStringView FStringStream::ReadLine()
	{
		FString & Buffer = GetThreadLocalStringBuffer<Char>();
		ReadLine(Buffer);
		return Buffer;
	}

	bool FStringStream::ReadLine(FString & Buffer)
	{
		Buffer.Clear();
		char Ch = 0;
		while (true)
		{
			uintx_t ReadResult = Stream.Read(&Ch, 1);
			if (!ReadResult || ReadResult == NullIndex)
				break;
			
			if (Ch == 0 || Ch == '\n')
			{
				if (Buffer.IsEmpty())
					continue;
				
				break;
			}

			if (Ch == '\r')
				continue;

			Buffer.Add(1, Char(Ch));
		}
		return !Buffer.IsEmpty();
	}
}
