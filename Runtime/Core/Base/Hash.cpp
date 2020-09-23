#include "PCH.h"
#include "Hash.h"

namespace XE
{
	uintx_t HashBytes(const byte_t * Bytes, size_t Size, uintx_t HashValueBase)
	{
		size_t NumUIX = Size / sizeof(uintx_t);
		size_t NumByteTails = Size % sizeof(uintx_t);

		const uintx_t * UIXValues = reinterpret_cast<const uintx_t *>(Bytes);
		uintx_t HashValue = HashValueBase;
		for (size_t UIXIndex = 0; UIXIndex < NumUIX; ++UIXIndex)
			HashValue = DEF_BASE_X * HashValue ^ static_cast<uintx_t>(UIXValues[UIXIndex]);

		if (NumByteTails > 0)
		{
			union
			{
				uintx_t UIXTailValue = 0;
				byte_t ByteTailValues[sizeof(uintx_t)];
			};
			for (size_t ByteIndex = 0; ByteIndex < NumByteTails; ++ByteIndex)
				ByteTailValues[ByteIndex] = Bytes[NumUIX * sizeof(uintx_t) + ByteIndex];

			HashValue = DEF_BASE_X * HashValue ^ static_cast<uintx_t>(UIXTailValue);
		}
		return HashValue;
	}
}
