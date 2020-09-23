#pragma once

#include "IOInc.h"

namespace XE
{
	class CORE_API FMemoryStream : public FStream
	{
	public:
		FMemoryStream() = default;
		~FMemoryStream() = default;

		uintx_t Read(void * Data, uintx_t Size) override
		{
			Memcpy(Data, Buffer.GetData() + Position, Size);
			Position += Size;
			return Size;
		}

		uintx_t Write(const void * Data, uintx_t Size) override
		{
			Buffer.Add(static_cast<const byte_t*>(Data), Size);
			Position += Size;
			return Size;
		}

		TVector<byte_t> & GetBuffer() { return Buffer; }
		const TVector<byte_t> & GetBuffer() const { return Buffer; }
		
		void Reset()
		{
			Position = 0;
		}

	private:
		TVector<byte_t> Buffer;
		size_t Position = 0;
	};
	
}