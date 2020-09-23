#pragma once

#include "IOInc.h"
#include "File.h"

namespace XE
{
	class CORE_API FFileStream : public FStream
	{
	public:
		FFileStream() = default;
		~FFileStream();
		FFileStream(const FFileStream &) = delete;
		FFileStream(FFileStream && That) noexcept : Handle(That.Handle) { That.Handle = nullptr; }
		FFileStream(FStringView FilePath, EFileModes FileModes);

		bool IsValid() const { return !!Handle; }
		size_t GetSize() const;

		EError Open(FStringView FilePath, EFileModes FileModes);
		void Close();
		void Flush();

		void Seek(EFilePosition FilePosition, intx_t Position);
		uintx_t Read(void * Data, uintx_t Size);
		uintx_t Write(const void * Data, uintx_t Size);

	private:
		ptr_t Handle = nullptr;
	};
}
