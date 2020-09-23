#pragma once

#include "StringView.h"

namespace XE
{
	class CORE_API FStringStream : public FStream
	{
	public:
		FStream & Stream;

	public:
		FStringStream(FStream & StreamIn) : Stream(StreamIn) { }
		uintx_t Read(void * Data, uintx_t Size) { return Stream.Read(Data, Size); }
		uintx_t Write(const void * Data, uintx_t Size) { return Stream.Write(Data, Size); }

		template<size_t Length>
		FStringStream & operator<<(const Char(&Value)[Length]) { if (Length > 1) Stream.Write(Value, (Length - 1) * sizeof(Char)); return *this; }
		FStringStream & operator<<(FStringView Value) { Stream.Write(Value.Data, Value.Size * sizeof(Char)); return *this; }
		FStringStream & operator<<(const FString & Value) { Stream.Write(Value.Data, Value.Size * sizeof(Char)); return *this; }

		FStringView ReadLine();
		bool ReadLine(FString & Buffer);
		
		//template<typename T>
		//FStringStream & operator << (const T & Value)
		//{
		//	FStringView String = Format(Str("{}"), Value);
		//	Write(String.Data, String.Size);
		//	return *this;
		//}

		//FStringStream & operator << (const char * Text) &
		//{
		//	Write(reinterpret_cast<const Char*>(Text), Length(Text));
		//	return *this;
		//}

		//FStringStream && operator << (const char * Text) &&
		//{
		//	Write(reinterpret_cast<const Char *>(Text), Length(Text));
		//	return static_cast<FStringStream&&>(*this);
		//}

		//FStringStream & operator << (const char8_t * Text) &
		//{
		//	Write(reinterpret_cast<const Char*>(Text), Length(Text));
		//	return *this;
		//}

		//FStringStream && operator << (const char8_t * Text) &&
		//{
		//	Write(reinterpret_cast<const Char *>(Text), Length(Text));
		//	return static_cast<FStringStream &&>(*this);
		//}

		//FStringStream & operator << (FStringView String) &
		//{
		//	Write(String.Data, String.Size);
		//	return *this;
		//}

		//FStringStream && operator << (FStringView String) &&
		//{
		//	Write(String.Data, String.Size);
		//	return static_cast<FStringStream &&>(*this);
		//}

		//FStringStream & operator << (const FString & String) &
		//{
		//	Write(String.Data, String.Size);
		//	return *this;
		//}

		//FStringStream && operator << (const FString & String) &&
		//{
		//	Write(String.Data, String.Size);
		//	return static_cast<FStringStream &&>(*this);
		//}
	};
}
