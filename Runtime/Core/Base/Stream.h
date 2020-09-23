#pragma once

#include "Types.h"

namespace XE
{
	class CORE_API FStream
	{
	public:
		virtual ~FStream() = default;
		virtual uintx_t Read(void * Data, uintx_t Size) = 0;
		virtual uintx_t Write(const void * Data, uintx_t Size) = 0;
	};

	class CORE_API FDataStream : public FStream
	{
	public:
		FDataStream(FStream & StreamIn) : Stream(StreamIn) { }

		uintx_t Read(void * Data, uintx_t Size) { return Stream.Read(Data, Size); }
		uintx_t Write(const void * Data, uintx_t Size) { return Stream.Write(Data, Size); }

		void operator<<(int8_t Value) { Stream.Write(&Value, 1); }
		void operator>>(int8_t & Value) { Stream.Read(&Value, 1); }
		void operator<<(uint8_t Value) { Stream.Write(&Value, 1); }
		void operator>>(uint8_t & Value) { Stream.Read(&Value, 1); }

		void operator<<(int16_t Value) { Stream.Write(&Value, 2); }
		void operator>>(int16_t & Value) { Stream.Read(&Value, 2); }
		void operator<<(uint16_t Value) { Stream.Write(&Value, 2); }
		void operator>>(uint16_t & Value) { Stream.Read(&Value, 2); }

		void operator<<(int32_t Value) { Stream.Write(&Value, 4); }
		void operator>>(int32_t & Value) { Stream.Read(&Value, 4); }
		void operator<<(uint32_t Value) { Stream.Write(&Value, 4); }
		void operator>>(uint32_t & Value) { Stream.Read(&Value, 4); }

		void operator<<(int64_t Value) { Stream.Write(&Value, 8); }
		void operator>>(int64_t & Value) { Stream.Read(&Value, 8); }
		void operator<<(uint64_t Value) { Stream.Write(&Value, 8); }
		void operator>>(uint64_t & Value) { Stream.Read(&Value, 8); }

		void operator<<(float32_t Value) { Stream.Write(&Value, 4); }
		void operator>>(float32_t & Value) { Stream.Read(&Value, 4); }
		void operator<<(float64_t Value) { Stream.Write(&Value, 8); }
		void operator>>(float64_t & Value) { Stream.Read(&Value, 8); }

		void operator<<(bool Value) { Stream.Write(&Value, 1); }
		void operator>>(bool & Value) { Stream.Read(&Value, 1); }
		void operator<<(char Value) { Stream.Write(&Value, 1); }
		void operator>>(char & Value) { Stream.Read(&Value, 1); }
		void operator<<(char8_t Value) { Stream.Write(&Value, 1); }
		void operator>>(char8_t & Value) { Stream.Read(&Value, 1); }
		void operator<<(char16_t Value) { Stream.Write(&Value, 2); }
		void operator>>(char16_t & Value) { Stream.Read(&Value, 2); }
		void operator<<(char32_t Value) { Stream.Write(&Value, 4); }
		void operator>>(char32_t & Value) { Stream.Read(&Value, 4); }

		template<typename T, typename = EnableIfT<IsEnumV<T>>>
		void operator<<(T Value)
		{
			using UnderlyingT = UnderlyingTypeT<T>;
			operator<<(static_cast<UnderlyingT>(Value));
		}

		template<typename T, typename = EnableIfT<IsEnumV<T>>>
		void operator>>(T & Value)
		{
			using UnderlyingT = UnderlyingTypeT<T>;
			UnderlyingT Underlying;
			operator>>(Underlying);
			Value = static_cast<T>(Underlying);
		}
		
	public:
		FStream & Stream;
	};
}
