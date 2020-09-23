#pragma once

#include "Types.h"

#ifdef XE_WINDOWS
#define CLASS_UUID(ClassName, UUID) class __declspec(uuid(UUID)) ClassName
#endif

namespace XE
{
	namespace Private_Guid
	{
#pragma pack(push, 1)
		// E357FCCD-A995-4576-B01F-234630154E96\0
		struct SGuidString37
		{
			char Data1[2][2][2];
			char _s2;
			char Data2[2][2];
			char _s3;
			char Data3[2][2];
			char _s4;
			char Data4[2][2];
			char _s5;
			char Data5[6][2];
		};

		// {E357FCCD-A995-4576-B01F-234630154E96}\0
		struct SGuidString39
		{
			char _s1;

			char Data1[2][2][2];
			char _s2;
			char Data2[2][2];
			char _s3;
			char Data3[2][2];
			char _s4;
			char Data4[2][2];
			char _s5;
			char Data5[6][2];
			
			char _s6;
		};
#pragma pack(pop)

		inline uint8_t GuidCharToUInt4(char ch)
		{
			if ('A' <= ch && ch <= 'F')
				return 10 + ch - 'A';
			else if ('a' <= ch && ch <= 'f')
				return 10 + ch - 'a';
			else if ('0' <= ch && ch <= '9')
				return ch - '0';
			else
				return 0;
		}

		inline uint8_t GuidToUInt8(const char(&text)[2])
		{
			union
			{
				uint8_t value;
				struct
				{
					uint8_t high : 4;
					uint8_t low : 4;
				};
			};

			low = GuidCharToUInt4(text[0]);
			high = GuidCharToUInt4(text[1]);
			return value;
		}

		inline uint16_t GuidToUInt16(const char(&text)[2][2])
		{
			union
			{
				uint16_t value;
				struct
				{
					uint8_t high;
					uint8_t low;
				};
			};
			low = GuidToUInt8(text[0]);
			high = GuidToUInt8(text[1]);
			return value;
		}

		inline uint32_t GuidToUInt32(const char(&text)[2][2][2])
		{
			union
			{
				uint32_t value;
				struct
				{
					uint16_t high;
					uint16_t low;
				};
			};
			low = GuidToUInt16(text[0]);
			high = GuidToUInt16(text[1]);
			return value;
		}


		inline char FromUInt4(uint8_t _value, bool upper = false)
		{
			if (0 <= _value && _value <= 9)
				return '0' + _value;
			else if (_value < 16)
				return (upper ? 'A' : 'a') + (_value - 10);
			else
				return 0;
		}

		inline void FromUInt8(uint8_t _value, char(&text)[2], bool upper = false)
		{
			union
			{
				uint8_t value;
				struct
				{
					uint8_t high : 4;
					uint8_t low : 4;
				};
			};

			value = _value;
			text[0] = FromUInt4(low, upper);
			text[1] = FromUInt4(high, upper);
		}

		inline void FromUInt16(uint16_t _value, char(&text)[2][2], bool upper = false)
		{
			union
			{
				uint16_t value;
				struct
				{
					uint8_t high;
					uint8_t low;
				};
			};
			value = _value;
			FromUInt8(low, text[0], upper);
			FromUInt8(high, text[1], upper);
		}

		inline void FromUInt32(uint32_t _value, char(&text)[2][2][2], bool upper = false)
		{
			union
			{
				uint32_t value;
				struct
				{
					uint16_t high;
					uint16_t low;
				};
			};
			value = _value;
			FromUInt16(low, text[0], upper);
			FromUInt16(high, text[1], upper);
		}
	}

	CORE_API int TestFun();
	struct CORE_API FGuid
	{
		FGuid() = default;
		FGuid(const FGuid &) = default;
		FGuid(FGuid &&) noexcept = default;
		FGuid & operator=(const FGuid &) noexcept = default;
		friend bool operator == (const FGuid &, const FGuid &) = default;
		friend auto operator <=> (const FGuid &, const FGuid &) = default;

#ifdef XE_WINDOWS
		FGuid(const struct _GUID & GUID);
#endif
		// {0x34256d4b, 0xe67f, 0x4e05, {0x93, 0x60, 0xc, 0x86, 0x3a, 0x1c, 0x42, 0xc1}}
		FGuid(uint32_t Data1In, uint16_t Data2In, uint16_t Data3In, const uint8_t(&Data4In)[8])
		{
			Data1 = Data1In;
			Data2 = Data2In;
			Data3 = Data3In;
			Memcpy(Data4, Data4In, 8);
		}

		// {0x34256d4b, 0xe67f, 0x4e05, 0x93, 0x60, 0xc, 0x86, 0x3a, 0x1c, 0x42, 0xc1}
		FGuid(uint32_t Data1In, uint16_t Data2In, uint16_t Data3In,
			uint8_t Data40In, uint8_t Data41In, uint8_t Data42In, uint8_t Data43In,
			uint8_t Data44In, uint8_t Data45In, uint8_t Data46In, uint8_t Data47In)
		{
			Data1 = Data1In;
			Data2 = Data2In;
			Data3 = Data3In;
			Data4[0] = Data40In; Data4[1] = Data41In; Data4[2] = Data42In; Data4[3] = Data43In;
			Data4[4] = Data44In; Data4[5] = Data45In; Data4[6] = Data46In; Data4[7] = Data47In;
		}


		// 34256D4B-E67F-4E05-9360-0C863A1C42C1
		FGuid(const char(&GuidStr)[37])
		{
			const Private_Guid::SGuidString37 & text = *(const Private_Guid::SGuidString37 *)(&GuidStr[0]);
			Data1 = Private_Guid::GuidToUInt32(text.Data1);
			Data2 = Private_Guid::GuidToUInt16(text.Data2);
			Data3 = Private_Guid::GuidToUInt16(text.Data3);
			Data4[0] = Private_Guid::GuidToUInt8(text.Data4[0]);
			Data4[1] = Private_Guid::GuidToUInt8(text.Data4[1]);
			for (int cnt = 0; cnt < 6; ++cnt)
				Data4[2 + cnt] = Private_Guid::GuidToUInt8(text.Data5[cnt]);
		}

		// {34256D4B-E67F-4E05-9360-0C863A1C42C1}
		FGuid(const char(&GuidStr)[39])
		{
			const Private_Guid::SGuidString39 & text = *(const Private_Guid::SGuidString39 *)(&GuidStr[0]);
			Data1 = Private_Guid::GuidToUInt32(text.Data1);
			Data2 = Private_Guid::GuidToUInt16(text.Data2);
			Data3 = Private_Guid::GuidToUInt16(text.Data3);
			Data4[0] = Private_Guid::GuidToUInt8(text.Data4[0]);
			Data4[1] = Private_Guid::GuidToUInt8(text.Data4[1]);
			for (int cnt = 0; cnt < 6; ++cnt)
				Data4[2 + cnt] = Private_Guid::GuidToUInt8(text.Data5[cnt]);
		}

		FGuid(const char8_t(&GuidStr)[37])
		{
			const Private_Guid::SGuidString37 & text = *(const Private_Guid::SGuidString37 *)(&GuidStr[0]);
			Data1 = Private_Guid::GuidToUInt32(text.Data1);
			Data2 = Private_Guid::GuidToUInt16(text.Data2);
			Data3 = Private_Guid::GuidToUInt16(text.Data3);
			Data4[0] = Private_Guid::GuidToUInt8(text.Data4[0]);
			Data4[1] = Private_Guid::GuidToUInt8(text.Data4[1]);
			for (int cnt = 0; cnt < 6; ++cnt)
				Data4[2 + cnt] = Private_Guid::GuidToUInt8(text.Data5[cnt]);
		}
		
		FGuid(const char8_t(&GuidStr)[39])
		{
			const Private_Guid::SGuidString39 & text = *(const Private_Guid::SGuidString39 *)(&GuidStr[0]);
			Data1 = Private_Guid::GuidToUInt32(text.Data1);
			Data2 = Private_Guid::GuidToUInt16(text.Data2);
			Data3 = Private_Guid::GuidToUInt16(text.Data3);
			Data4[0] = Private_Guid::GuidToUInt8(text.Data4[0]);
			Data4[1] = Private_Guid::GuidToUInt8(text.Data4[1]);
			for (int cnt = 0; cnt < 6; ++cnt)
				Data4[2 + cnt] = Private_Guid::GuidToUInt8(text.Data5[cnt]);
		}

		void ToString37(char szGuid[37], bool bUpper = false) const
		{
			Private_Guid::SGuidString37 & text = *(Private_Guid::SGuidString37 *)(&szGuid[0]);
			Private_Guid::FromUInt32(Data1, text.Data1, bUpper);
			Private_Guid::FromUInt16(Data2, text.Data2, bUpper);
			Private_Guid::FromUInt16(Data3, text.Data3, bUpper);
			Private_Guid::FromUInt8(Data4[0], text.Data4[0], bUpper);
			Private_Guid::FromUInt8(Data4[1], text.Data4[1], bUpper);
			for (int cnt = 0; cnt < 6; ++cnt)
				Private_Guid::FromUInt8(Data4[2 + cnt], text.Data5[cnt], bUpper);
			text._s2 = text._s3 = text._s4 = text._s5 = L'-';
			szGuid[36] = 0;
		}

		void ToString39(char szGuid[39], bool bUpper = false) const
		{
			Private_Guid::SGuidString39 & text = *(Private_Guid::SGuidString39 *)(&szGuid[0]);
			ToString37(szGuid + 1, bUpper);
			text._s1 = L'{';
			text._s6 = L'}';
			szGuid[38] = 0;
		}

		FStringView Format(FStringView Formal = FStringView::Empty) const;
		
		uint32_t Data1;
		uint16_t Data2;
		uint16_t Data3;
		uint8_t Data4[8];

	public:
		static FGuid Generate();
		static FGuid Empty;
	};

#ifdef XE_WINDOWS
	template<typename T>
	constexpr FGuid GuidOf(T &&)
	{
		return __uuidof(T);
	}
	
	template<typename T>
	constexpr FGuid GuidOf()
	{
		return __uuidof(T);
	}
	
	template<typename T>
	inline FGuid GuidV = __uuidof(T);
#endif
}
