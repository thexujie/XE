#pragma once

#include "Types.h"
#include "Allcoator.h"
#include "StringView.h"

namespace XE
{
	template class CORE_API TAllocator<Char>;
	
	template<typename CharT>
	class TString
	{
		using StringViewT = TView<CharT>;
		using AllocatorT = TAllocator<CharT>;
		
	public:
		size_t Size :  32 = 0;
		size_t Capacity : 32 = 0;
		union
		{
			CharT * Data = nullptr;
			void * DataPtr;
			char * DataCharPtr;
		};
		
	public:
		TString() = default;
		explicit TString(size_t SizeInit)
		{
			Size = SizeInit;
			Capacity = Size + 1;
			Data = AllocatorT::Alloc(Capacity);
			AllocatorT::Construct(Data, Size);
			FinishNull();
		}
		
		TString(TString && That) noexcept
		{
			Size = That.Size;
			Capacity = That.Capacity;
			Data = That.Data;
			That.Size = 0;
			That.Capacity = 0;
			That.Data = nullptr;
		}
		
		TString(const TString & That)
		{
			Size = That.Size;
			Capacity = Size > 0 ? That.Capacity : 0;
			Data = Size > 0 ? AllocatorT::Alloc(That.Capacity) : nullptr;
			AllocatorT::Construct(Data, That.Data, That.Size);
			FinishNull();
		}

		TString(StringViewT String)
		{
			Size = String.Size;
			Capacity = Size > 0 ? Size + 1 : 0;
			Data = Size > 0 ? AllocatorT::Alloc(Capacity) : nullptr;
			AllocatorT::Construct(Data, String.Data, String.Size);
			FinishNull();
		}

		constexpr TString(const CharT * String, size_t StringLength = NullPos) : TString(StringViewT(String, StringLength == NullPos ? Length(String) : StringLength))
		{
		}

		template<typename = EnableIfT<IsSameV<CharT, char>>>
		constexpr TString(TView<char8_t> String)
		{
			Size = String.Size;
			Capacity = Size > 0 ? Size + 1 : 0;
			Data = Size > 0 ? AllocatorT::Alloc(Capacity) : nullptr;
			AllocatorT::Construct(static_cast<char *>(Data), reinterpret_cast<const char *>(String.Data), String.Size);
			FinishNull();
		}
		
		template<typename = EnableIfT<IsSameV<CharT, char>>>
		constexpr TString(const char8_t * String, size_t StringLength = NullPos) : TString(StringViewT(reinterpret_cast<const char*>(String), StringLength == NullPos ? Length(String) : StringLength))
		{
		}
		
		~TString()
		{
			if (Data)
			{
				AllocatorT::Destruct(Data, Size);
				AllocatorT::Free(Data);

				Size = 0;
				Capacity= 0;
				Data = nullptr;
			}
		}
		
		TString & operator=(TString && That) noexcept
		{
			AllocatorT::Destruct(Data, Size);
			AllocatorT::Free(Data);
			
			Size = That.Size;
			Capacity = That.Capacity;
			Data = That.Data;
			That.Size = 0;
			That.Capacity = 0;
			That.Data = nullptr;
			return *this;
		}

		TString & operator=(const TString & That) noexcept
		{
			return operator=(StringViewT(That));
		}
		
		TString & operator=(StringViewT That) noexcept
		{
			AllocatorT::Destruct(Data, Size);
			if (That.Size == 0)
			{
				Size = 0;
				FinishNull();
				return *this;
			}
			
			if (Capacity < That.Size + 1)
			{
				AllocatorT::Free(Data);
				
				size_t NewCapacity = Capacity > 0 ? Capacity : 1;
				while (That.Size + 1 > NewCapacity)
					NewCapacity *= 2;
				Capacity = NewCapacity;
				Data = AllocatorT::Alloc(Capacity);
			}

			Size = That.Size;
			AllocatorT::Construct(Data, That.Data, That.Size);
			FinishNull();
			return *this;
		}

		TString & operator=(std::basic_string_view<CharT> String) noexcept
		{
			operator=(StringViewT(String.data(), String.size()));
			return *this;
		}

		TString & operator=(const CharT * String) noexcept
		{
			operator=(StringViewT(String, Length(String)));
			return *this;
		}

		TString & operator+=(const CharT * String)
		{
			Append(StringViewT(String, Length(String)));
			return *this;
		}

		TString & operator+=(StringViewT String)
		{
			Append(String);
			return *this;
		}

		size_t GetSize() const { return Size; }
		size_t GetCapacity() const { return Capacity; }
		size_t GetCapacityWithoutNull() const { return Capacity > 0 ? Capacity - 1 : 0; }
		CharT * GetData() const { return Data; }
		
		FORCEINLINE void Append(const CharT * String, size_t StringLength = NullPos)
		{
			if (StringLength == NullPos)
				StringLength = Length(String);

			Append(StringViewT(String, StringLength));
		}
		
		FORCEINLINE void Append(StringViewT String)
		{
			if (String.IsEmpty())
				return;

			size_t OldSize = Size;
			Size += String.Size;
			if (Size + 1 > Capacity)
			{
				size_t NewCapacity = Capacity > 0 ? Capacity : 1;
				while (Size + 1 > NewCapacity)
					NewCapacity *= 2;
				CharT * NewData = AllocatorT::Alloc(NewCapacity);
				AllocatorT::MoveCopy(NewData, Data, OldSize);
				AllocatorT::Free(Data);
				Data = NewData;
			}
			AllocatorT::Construct(Data + OldSize, String.Data, String.Size);
			FinishNull();
		}

		FORCEINLINE bool IsEmpty() const { return Size == 0; }
		FORCEINLINE bool IsValidIndex(size_t Index) const
		{
			return Index >= 0 && Index < Size;
		}

		FORCEINLINE CharT & operator[](size_t Index) const
		{
			//assert(IsValidIndex(Index), "String index out of bounds: Index %i from a string with a length of %i", Index, Length);
			return Data[Index];
		}

		StringViewT Slice(size_t Index, size_t Count = NullIndex) const
		{
			if (Count == NullIndex)
			{
				if (Index >= Size)
					return StringViewT();
				Count = Size - Index;
			}
			else if (Index + Count > Size)
				return StringViewT();
			else {}

			return StringViewT(Data + Index, Count);
		}

		FORCEINLINE operator StringViewT() const
		{
			return StringViewT(Data, Size);
		}

		void Add(size_t Count)
		{
			if (!Count)
				return;

			size_t Index = Size;
			size_t NewSize = Size + Count;
			if (NewSize + 1 > Capacity)
			{
				size_t NewCapacity = Capacity > 0 ? Capacity : 1;
				while (NewCapacity < NewSize + 1)
					NewCapacity *= 2;
				CharT * NewData = AllocatorT::Alloc(NewCapacity);
				AllocatorT::MoveCopy(NewData, Data, Size);
				AllocatorT::Free(Data);
				AllocatorT::Construct(NewData + Index, Count);
				Data = NewData;
				Capacity = NewCapacity;
			}
			Size = NewSize;
			FinishNull();
		}

		void Reduce(size_t Count)
		{
			if (Count > Size) Count = Size;
			size_t Index = Size - Count;
			AllocatorT::Destruct(Data + Index, Count);
			Size = Size - Count;
			FinishNull();
		}

		void Add(size_t Count, const CharT & Ch)
		{
			size_t Index = Size;
			size_t NewSize = Size + Count;
			if (NewSize + 1 > Capacity)
			{
				size_t NewCapacity = Capacity > 0 ? Capacity : 1;
				while (NewCapacity < NewSize + 1)
					NewCapacity *= 2;

				CharT * NewData = AllocatorT::Alloc(NewCapacity);
				AllocatorT::MoveCopy(NewData, Data, Size);
				AllocatorT::Free(Data);
				AllocatorT::Construct(NewData + Index, Count);
				Data = NewData;
				Capacity = NewCapacity;
			}

			AllocatorT::Construct(Data + Index, Ch, Count);
			Size = Size + Count;
			FinishNull();
		}
		
		void Resize(size_t NewSize, size_t NewCapacity = 0)
		{
			if(NewCapacity > 0 && NewCapacity != Capacity)
			{
				if (NewCapacity <= NewSize)
					throw EError::OutOfBound;

				size_t Index = Size;
				CharT * NewData = AllocatorT::Alloc(NewCapacity);
				if (NewSize > Size)
				{
					AllocatorT::MoveCopy(NewData, Data, Size);
					AllocatorT::Construct(NewData + Index, NewSize - Size);
				}
				else if (NewSize < Size)
				{
					AllocatorT::MoveCopy(NewData, Data, NewSize);
					AllocatorT::Destruct(Data + (Size - NewSize), Size - NewSize);
				}
				else {}
				AllocatorT::Free(Data);
				Data = NewData;
				Size = NewSize;
				Capacity = NewCapacity;
			}
			else
			{
				if (NewSize > Size)
					Add(NewSize - Size);
				else if (NewSize < Size)
					Reduce(Size - NewSize);
				else {}
			}
			FinishNull();
		}

		void Clear()
		{
			Resize(0);
		}

		void Reset()
		{
			if (Data)
			{
				AllocatorT::Destruct(Data, Size);
				AllocatorT::Free(Data);

				Size = 0;
				Capacity = 0;
				Data = nullptr;
			}
		}

		TString & Remove(StringViewT Pattern)
		{
			if (Size < Pattern.Size || Pattern.Size == 0)
				return *this;

			for (size_t Index = 0; Index <= Size - Pattern.Size; ++Index)
			{
				if (Strings::Compare<CharT>(TView<CharT>(Data + Index, Pattern.Size), Pattern) == 0)
				{
					AllocatorT::Destruct(Data + Index, Pattern.Size);
					AllocatorT::MoveCopy(Data + Index, Data + Index + Pattern.Size, Size - Index - Pattern.Size);
					Size -= Pattern.Size;
				}
			}
			FinishNull();
			return *this;
		}
		
		TString & Replace(StringViewT Pattern, StringViewT Substitute)
		{
			if (Size < Pattern.Size || Pattern.Size == 0 || Substitute.Size == 0)
				return *this;

			for (size_t Index = 0; Index <= Size - Pattern.Size;/**/)
			{
				size_t Step = 1;
				if (Strings::Compare<CharT>(TView<CharT>(Data + Index, Pattern.Size), Pattern) == 0)
				{
					size_t NewSize = Size + Substitute.Size - Pattern.Size;
					if (NewSize >= Capacity)
					{
						size_t NewCapacity = Capacity > 0 ? Capacity : 1;
						while (NewCapacity < NewSize + 1)
							NewCapacity *= 2;

						CharT * NewData = AllocatorT::Alloc(NewCapacity);
						AllocatorT::MoveCopy(NewData, Data, Index);
						AllocatorT::MoveCopy(NewData + Index + Substitute.Size, Data + Index + Pattern.Size, Size - Index - Pattern.Size);
						AllocatorT::Free(Data);
						Data = NewData;
						Capacity = NewCapacity;
					}
					else
						AllocatorT::MoveCopy(Data + Index + Substitute.Size, Data + Index + Pattern.Size, Size - Index - Pattern.Size);
					AllocatorT::Copy(Data + Index, Substitute.Data, Substitute.Size);
					Size = NewSize;
					Step = Substitute.Size;
				}
				Index += Step;
			}
			FinishNull();
			return *this;
		}

		TString & ReplaceAt(size_t Index, size_t Count, StringViewT Substitute)
		{
			if (Size < Count || Count == 0 || Substitute.Size == 0)
				return *this;
			
			size_t NewSize = Size + Substitute.Size - Count;
			if (NewSize >= Capacity)
			{
				size_t NewCapacity = Capacity > 0 ? Capacity : 1;
				while (NewCapacity < NewSize + 1)
					NewCapacity *= 2;

				CharT * NewData = AllocatorT::Alloc(NewCapacity);
				AllocatorT::MoveCopy(NewData, Data, Index);
				AllocatorT::MoveCopy(NewData + Index + Substitute.Size, Data + Index + Count, Size - Index - Count);
				AllocatorT::Free(Data);
				Data = NewData;
				Capacity = NewCapacity;
			}
			else
				AllocatorT::MoveCopy(Data + Index + Substitute.Size, Data + Index + Count, Size - Index - Count);
			AllocatorT::Copy(Data + Index, Substitute.Data, Substitute.Size);
			Size = NewSize;

			FinishNull();
			return *this;
		}
		
		TString & SetAt(size_t Index, StringViewT Substitute)
		{
			if (Index + Substitute.Size >= Size)
				throw EError::OutOfBound;

			AllocatorT::Copy(Data + Index, Substitute.Data, Substitute.Size);
			return *this;
		}

		TString & RemoveAt(size_t Index, size_t Count = 1)
		{
			if (Index + Count >= Size)
				throw EError::OutOfBound;

			AllocatorT::Destruct(Data + Index, Count);
			AllocatorT::MoveCopy(Data + Index, Data + Index + Count, Size - Index - Count);
			Size -= Count;
			FinishNull();
			return *this;
		}

		TString & TrimStart(TView<CharT> TrimmedChars = Strings::WhiteSpaces<CharT>)
		{
			size_t Index = 0;
			for (; Index < Size; ++Index)
			{
				if (TrimmedChars.FindFirst(Data[Index]) == NullIndex)
					break;
			}

			if (Index > 0)
			{
				AllocatorT::Destruct(Data, Index);
				AllocatorT::MoveCopy(Data, Data + Index, Size - Index);
				Size -= Index;
				FinishNull();
			}

			return *this;
		}
		
		TString & TrimEnd(TView<CharT> TrimmedChars = Strings::WhiteSpaces<CharT>)
		{
			size_t Index = Size;
			for (; Index > 0; --Index)
			{
				if (TrimmedChars.FindFirst(Data[Index - 1]) == NullIndex)
					break;
			}

			if (Index < Size)
			{
				AllocatorT::Destruct(Data + Index, Size - Index);
				Size = Index;
				FinishNull();
			}
			return *this;
		}

		TString & Trim(TView<CharT> TrimmedChars = Strings::WhiteSpaces<CharT>)
		{
			size_t IndexFrom = 0;
			for (; IndexFrom < Size; ++IndexFrom)
			{
				if (TrimmedChars.FindFirst(Data[IndexFrom]) == NullIndex)
					break;
			}

			size_t IndeTo = Size;
			for (; IndeTo > IndexFrom; --IndeTo)
			{
				if (TrimmedChars.FindFirst(Data[IndeTo - 1]) == NullIndex)
					break;
			}

			if (Size != IndeTo - IndexFrom)
			{
				AllocatorT::Destruct(Data, IndexFrom);
				AllocatorT::Destruct(Data + IndeTo, Size - IndeTo);
				AllocatorT::MoveCopy(Data, Data + IndexFrom, IndeTo - IndexFrom);
				Size = IndeTo - IndexFrom;
				FinishNull();
			}
			return *this;
		}

		TString & ToUpper()
		{
			for (size_t Index = 0; Index < Size; ++Index)
				Data[Index] = Strings::ToUpper(Data[Index]);
			return *this;
		}

		TString & ToLower()
		{
			for (size_t Index = 0; Index < Size; ++Index)
				Data[Index] = Strings::ToLower(Data[Index]);
			return *this;
		}

		size_t FindFirst(const CharT & Ch) const
		{
			return Array::FindFirst<CharT>(Data, Size, Ch);
		}

		size_t FindFirst(StringViewT Pattern) const
		{
			return Array::FindFirst<CharT>(Data, Size, Pattern.Data, Pattern.Size);
		}

		size_t FindFirstOf(StringViewT Chs) const
		{
			return Array::FindFirstOf<CharT>(Data, Size, Chs.Data, Chs.Size);
		}

		size_t FindLast(const CharT & Ch) const
		{
			return Array::FindLast<CharT>(Data, Size, Ch);
		}

		size_t FindLast(StringViewT Pattern) const
		{
			return Array::FindLast<CharT>(Data, Size, Pattern.Data, Pattern.Size);
		}

		size_t FindLastOf(StringViewT Chs) const
		{
			return Array::FindLastOf<CharT>(Data, Size, Chs.Data, Chs.Size);
		}

		bool StartsWith(StringViewT Pattern) const
		{
			return Array::StartsWith(Data, Size, Pattern.Data, Pattern.Size);
		}

		bool EndsWith(StringViewT Pattern) const
		{
			return Array::EndsWith(Data, Size, Pattern.Data, Pattern.Size);
		}

		template<typename ...ArgsT>
		TString & Format(ArgsT ...Args)
		{
			return *this;
		}

		TString operator + (const TString & Another) const
		{
			if (Another.Size == 0)
				return *this;
			
			TString Result(Size + Another.Size);
			AllocatorT::Construct(Result.Data, Data, Size);
			AllocatorT::Construct(Result.Data + Size, Another.Data, Another.Size);
			return Result;
		}

		TString operator + (StringViewT Another) const
		{
			if (Another.Size == 0)
				return *this;
			
			TString Result(Size + Another.Size);
			AllocatorT::Construct(Result.Data, Data, Size);
			AllocatorT::Construct(Result.Data + Size, Another.Data, Another.Size);
			return Result;
		}

		TString operator + (const CharT * Another) const
		{
			return operator +(StringViewT(Another, Length(Another)));
		}

		auto operator<=>(const TString & Another) const
		{
			return Strings::Compare<CharT>(Data, Size, Another.Data, Another.Size) <=> 0;
		}
		auto operator==(const TString & Another) const
		{
			return Strings::Compare<CharT>(Data, Size, Another.Data, Another.Size) == 0;
		}

		auto operator<=>(const StringViewT & Another) const
		{
			return Strings::Compare<CharT>(Data, Size, Another.Data, Another.Size) <=> 0;
		}
		auto operator==(const StringViewT & Another) const
		{
			return Strings::Compare<CharT>(Data, Size, Another.Data, Another.Size) == 0;
		}
		
		void FinishNull()
		{
			if (Size)
				Data[Size] = CharT(0);
		}

		friend FArchive & operator >> (FArchive & Serializer, TString & String)
		{
			if (Serializer.IsInput())
			{
				uint32_t Length = Serializer.Read<uint32_t>();
				String.Resize(Length, Length + 1);
				Serializer.Read(String.Data, sizeof(Char) * Length);
				String.FinishNull();
			}
			else
			{
				Serializer.Write(uint32_t(String.GetSize()));
				Serializer.Write(String.GetData(), sizeof(Char) * String.GetSize());
			}
			return Serializer;
		}

		StringViewT ToString() const { return StringViewT(Data, Size); }

	public:
		static TString Empty;
	};

	template class CORE_API TString<char>;
	template class CORE_API TString<char8_t>;
	template class CORE_API TString<char16_t>;
	template class CORE_API TString<wchar_t>;
	
	template <typename T>
	TString<T> TString<T>::Empty = {};
	
	using FAString = TString<char>;
	using FU8String = TString<char8_t>;
	using FU16String = TString<char16_t>;
	using FU32String = TString<char32_t>;
	using FWString = TString<wchar_t>;
	
	using FString = TString<Char>;
	
	template<typename CharT> struct THash<TString<CharT>> { uintx_t operator()(const TString<CharT> & Value) const { return HashString(Value.GetData(), Value.GetSize()); } };

	template<typename CharT>
	TString<CharT> & GetThreadLocalStringBuffer();

	template CORE_API TString<char> & GetThreadLocalStringBuffer<char>();
	template CORE_API TString<char8_t> & GetThreadLocalStringBuffer<char8_t>();
	template CORE_API TString<wchar_t> & GetThreadLocalStringBuffer<wchar_t>();
	
	namespace Strings
	{
		CORE_API int32_t Stricmp(FAStringView StringLeft, FAStringView StringRight);
		CORE_API int32_t Stricmp(FU8StringView StringLeft, FU8StringView StringRight);
		CORE_API int32_t Stricmp(FU16StringView StringLeft, FU16StringView StringRight);
		CORE_API int32_t Stricmp(FU32StringView StringLeft, FU32StringView StringRight);
		CORE_API int32_t Stricmp(FWStringView StringLeft, FWStringView StringRight);

		CORE_API FU8StringView AnsiToUtf8(const char * String, size_t StringLength = NullPos);
		inline FU8StringView AnsiToUtf8(FAStringView String) { return AnsiToUtf8(String.Data, String.Size); }
		inline FU8StringView AnsiToUtf8(std::string_view String) { return AnsiToUtf8(String.data(), String.size()); }

		CORE_API FWStringView AnsiToWide(const char * String, size_t StringLength = NullPos);
		inline FWStringView AnsiToWide(FAStringView String) { return AnsiToWide(String.Data, String.Size); }
		inline FWStringView AnsiToWide(std::string_view String) { return AnsiToWide(String.data(), String.size()); }

		CORE_API FAStringView Utf8ToAnsi(const char8_t * String, size_t StringLength = NullPos);
		inline FAStringView Utf8ToAnsi(FU8StringView String) { return Utf8ToAnsi(String.Data, String.Size); }
		inline FAStringView Utf8ToAnsi(std::u8string_view String) { return Utf8ToAnsi(String.data(), String.size()); }

		CORE_API FWStringView Utf8ToWide(const char8_t * String, size_t StringLength = NullPos);
		inline FWStringView Utf8ToWide(FU8StringView String) { return Utf8ToWide(String.Data, String.Size); }
		inline FWStringView Utf8ToWide(std::u8string_view String) { return Utf8ToWide(String.data(), String.size()); }

		CORE_API FU8StringView WideToUtf8(const wchar_t * String, size_t StringLength = NullPos);
		inline FU8StringView WideToUtf8(FWStringView String) { return WideToUtf8(String.Data, String.Size); }
		inline FU8StringView WideToUtf8(std::wstring_view String) { return WideToUtf8(String.data(), String.size()); }

		CORE_API FAStringView WideToAnsi(const wchar_t * String, size_t StringLength = NullPos);
		inline FAStringView WideToAnsi(FWStringView String) { return WideToAnsi(String.Data, String.Size); }
		inline FAStringView WideToAnsi(std::wstring_view String) { return WideToAnsi(String.data(), String.size()); }

		inline FU8StringView Utf16ToUtf8(const char16_t * String, size_t StringLength = NullPos) { return WideToUtf8((const wchar_t *)String, StringLength); }
		inline FU8StringView Utf16ToUtf8(FU16StringView String) { return Utf16ToUtf8(String.Data, String.Size); }
		inline FU8StringView Utf16ToUtf8(std::u16string_view String) { return Utf16ToUtf8(String.data(), String.size()); }

		inline FWStringView Utf16ToWide(const char16_t * String, size_t StringLength = NullPos) { return FWStringView((const wchar_t *)String, StringLength); }
		inline FWStringView Utf16ToWide(FU16StringView String) { return Utf16ToWide(String.Data, String.Size); }
		inline FWStringView Utf16ToWide(std::u16string_view String) { return Utf16ToWide(String.data(), String.size()); }

		inline FAStringView ToAnsi(FAStringView String) { return String; }
		inline FAStringView ToAnsi(FU8StringView String) { return Utf8ToAnsi(String); }
		inline FAStringView ToAnsi(FWStringView String) { return WideToAnsi(String); }
		inline FU8StringView ToUtf8(FU8StringView String) { return String; }
		inline FU8StringView ToUtf8(FAStringView String) { return AnsiToUtf8(String); }
		inline FU8StringView ToUtf8(FWStringView String) { return WideToUtf8(String); }
		inline FWStringView ToWide(FWStringView String) { return String; }
		inline FWStringView ToWide(FU8StringView String) { return Utf8ToWide(String); }
		inline FWStringView ToWide(FAStringView String) { return AnsiToWide(String); }

		inline std::string ToAString(const char * String, size_t StringLength = NullPos)
		{
			if (StringLength == NullPos)
				StringLength = Length(String);
			return std::string(String, StringLength);
		}

		inline std::string ToAString(const char8_t * String, size_t StringLength)
		{
			FAStringView AString = Utf8ToAnsi(String, StringLength);
			return std::string(AString.Data, AString.Size);
		}

		inline std::string  ToStdString(FAStringView String) { return std::string(String.Data, String.Size); }
		inline std::string  ToStdString(FU8StringView String) { return ToStdString(ToAnsi(String)); }
		inline std::string  ToStdString(FWStringView String) { return ToStdString(ToAnsi(String)); }

		inline std::wstring ToStdWString(FWStringView String) { return std::wstring(String.Data, String.Size); }
		inline std::wstring ToStdWString(FAStringView String) { return ToStdWString(ToWide(String)); }
		inline std::wstring ToStdWString(FU8StringView String) { return ToStdWString(ToWide(String)); }

		template<typename CharT>
		FStringView FromStdString(std::basic_string_view<CharT> StdString)
		{
			return ToAuto({ StdString.data(), StdString.size() });
		}

		template<typename CharT>
		FStringView FromStdString(const std::basic_string<CharT> & StdString)
		{
			return ToAuto(TView<CharT>(StdString.data(), StdString.size()));
		}

		template<typename CharT>
		FStringView FromStdString(std::basic_string<CharT> && StdString)
		{
			TString<CharT> & Buffer = GetThreadLocalStringBuffer<CharT>();
			Buffer.Append(StdString.data(), StdString.size());
			return ToAuto(TView<CharT>(Buffer.Data, Buffer.Size));
		}
		
		template<typename CharT>
		FStringView ToAuto(TView<CharT> String)
		{
			if constexpr (IsSameV<Char, CharT>)
				return String;
			else if constexpr (IsSameV<Char, char>)
				return ToAnsi(String);
			else if constexpr (IsSameV<Char, char8_t>)
				return ToUtf8(String);
			else
				return String;
		}

		template<typename CharT = FStringView::ElementType>
		TView<CharT> FromAnsi(FAStringView String)
		{
			if constexpr (IsSameV<CharT, char>)
				return String;
			else
				return AnsiToUtf8(String);
		}
		template<typename CharT = FStringView::ElementType>
		TView<CharT> FromAnsi(const char * String, size_t StringLength = NullPos)
		{
			if (StringLength == NullPos)
				StringLength = Length(String);
			return FromAnsi(FAStringView(String, StringLength));
		}
		template<typename CharT = FStringView::ElementType>
		TView<CharT> FromAnsi(std::string_view String)
		{
			return FromAnsi(FAStringView(String));
		}

		template<typename CharT = FStringView::ElementType>
		TView<CharT> FromUtf8(FU8StringView String)
		{
			if constexpr (IsSameV<CharT, char>)
				return Utf8ToAnsi(String);
			else
				return String;
		}
		template<typename CharT = FStringView::ElementType>
		TView<CharT> FromUtf8(const char8_t * String, size_t StringLength = NullPos)
		{
			if (StringLength == NullPos)
				StringLength = Length(String);
			return FromUtf8(FU8StringView(String, StringLength));
		}
		template<typename CharT = FStringView::ElementType>
		TView<CharT> FromUtf8(std::u8string_view String)
		{
			return FromUtf8(FU8StringView(String));
		}

		template<typename CharT = FStringView::ElementType>
		TView<CharT> FromWide(FWStringView String)
		{
			if constexpr (IsSameV<CharT, char>)
				return WideToAnsi(String);
			else
				return WideToUtf8(String);
		}
		template<typename CharT = FStringView::ElementType>
		TView<CharT> FromWide(const wchar_t * String, size_t StringLength = NullPos)
		{
			if (StringLength == NullPos)
				StringLength = Length(String);
			return FromWide(FWStringView(String, StringLength));
		}
		template<typename CharT = FStringView::ElementType>
		TView<CharT> FromWide(std::wstring_view String)
		{
			return FromWide(FWStringView(String));
		}

		CORE_API FAStringView Printf(const char * Format, ...);
		CORE_API FU8StringView Printf(const char8_t * Format, ...);
		CORE_API FWStringView Printf(const wchar_t * Format, ...);

		template <typename... ArgsT>
		FAStringView PrintArgs(const char * Format, ArgsT... Args) { return Printf(Format, Forward<ArgsT>(Args)...); }
		template <typename... ArgsT>
		FAStringView PrintArgs(const char8_t * Format, ArgsT... Args) { return Printf(Format, Forward<ArgsT>(Args)...); }
		template <typename... ArgsT>
		FAStringView PrintArgs(const wchar_t * Format, ArgsT... Args) { return Printf(Format, Forward<ArgsT>(Args)...); }

		//template<typename stream_t>
		//static void FormatHelper(stream_t & Stream) {}

		//template<typename stream_t, typename HeadT, typename ...TailT>
		//static void FormatHelper(stream_t & Stream, const HeadT & Head, TailT && ...Tail)
		//{
		//	Stream << Head;
		//	return FormatHelper(Stream, Forward<TailT>(Tail)...);
		//}

		//template<typename ...Args>
		//static FString Format(Args && ...args)
		//{
		//	std::xostringstream Stream;
		//	FormatHelper(Stream, Forward<Args>(args)...);
		//	return FString(Stream.str());
		//}
		CORE_API int32_t ToBool(FStringView String);
		CORE_API int32_t ToInt32(FStringView String, uint32_t Radix = 10);
		CORE_API uint32_t ToUInt32(FStringView String, uint32_t Radix = 10);
		CORE_API int64_t ToInt64(FStringView String, uint32_t Radix = 10);
		CORE_API uint64_t ToUInt64(FStringView String, uint32_t Radix = 10);
		CORE_API intx_t ToIntX(FStringView String, uint32_t Radix = 10);
		CORE_API uintx_t ToUIntX(FStringView String, uint32_t Radix = 10);

		CORE_API float32_t ToFloat32(FStringView String);
		CORE_API float64_t ToFloat64(FStringView String);
	}

	template<>
	class TFormatter<FString>
	{
	public:
		FStringView Format(const FStringView & Value, FStringView Formal = FStringView::Empty) const
		{
			return Value;
		}

		FString Parse(FStringView String) const
		{
			return String;
		}
	};

	template<typename CharT>
	TView<CharT> GetLine(TView<CharT> String, size_t & Offset, CharT Delimiter)
	{
		size_t Position = Offset;
		size_t Count = 0;
		while (Offset < String.Size)
		{
			if (String[Offset++] == Delimiter)
				break;

			++Count;
		}
		return TView<CharT>(String.Data + Position, Count);
	}

	template<typename CharT>
	TView<CharT> GetLine(TView<CharT> String, size_t & Offset, TView<CharT> Delimiters)
	{
		size_t Begin = Offset;
		size_t Count = 0;
		while (Offset < String.Size)
		{
			if (Delimiters.FindFirst(String[Offset++]) != NullIndex)
				break;

			++Count;
		}
		return TView<CharT>(String.Data + Begin, Count);
	}

	template<typename CharT>
	TView<CharT> operator +(TView<CharT> Lhs, TView<CharT> Rhs);

	template CORE_API TView<char> operator +(TView<char> Lhs, TView<char> Rhs);
	template CORE_API TView<wchar_t> operator +(TView<wchar_t> Lhs, TView<wchar_t> Rhs);
	template CORE_API TView<char8_t> operator +(TView<char8_t> Lhs, TView<char8_t> Rhs);
	template CORE_API TView<char16_t> operator +(TView<char16_t> Lhs, TView<char16_t> Rhs);

	template<typename CharT>
	TView<CharT> operator +(const CharT * Lhs, TView<CharT> Rhs)
	{
		return XE::TView<CharT>(Lhs) + Rhs;
	}

	template<typename CharT>
	TView<CharT> operator +(TView<CharT> Lhs, const CharT * Rhs)
	{
		return Lhs + XE::TView<CharT>(Rhs);
	}
	
	//template<typename CharT, size_t Length>
	//TView<CharT> operator +(const CharT (&Lhs)[Length], TView<CharT> Rhs)
	//{
	//	return XE::TView<CharT>(Lhs) + Rhs;
	//}

	//template<typename CharT, size_t Length>
	//TView<CharT> operator +(TView<CharT> Lhs, const CharT(&Rhs)[Length])
	//{
	//	return Lhs + XE::TView<CharT>(Rhs);
	//}

	template<typename CharT>
	TView<CharT> operator +(TView<CharT> Lhs, const FString & Rhs)
	{
		return Lhs + XE::TView<CharT>(Rhs);
	}

	template<typename CharT>
	TView<CharT> operator +(const FString & Lhs, TView<CharT> Rhs)
	{
		return XE::TView<CharT>(Lhs) + Rhs;
	}

	template<typename CharT>
	TView<CharT> operator +(const CharT * Lhs, const FString & Rhs)
	{
		return XE::TView<CharT>(Lhs) + XE::TView<CharT>(Rhs);
	}

	template<typename CharT>
	TView<CharT> operator +(const FString & Lhs, const CharT * Rhs)
	{
		return XE::TView<CharT>(Lhs) + XE::TView<CharT>(Rhs);
	}

	//template<typename CharT, size_t Length>
	//TView<CharT> operator +(const CharT(&Lhs)[Length], const FString & Rhs)
	//{
	//	return XE::TView<CharT>(Lhs) + XE::TView<CharT>(Rhs);
	//}

	//template<typename CharT, size_t Length>
	//TView<CharT> operator +(const FString & Lhs, const CharT(&Rhs)[Length])
	//{
	//	return XE::TView<CharT>(Lhs) + XE::TView<CharT>(Rhs);
	//}
}
