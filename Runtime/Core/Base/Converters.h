#pragma once

#include "Types.h"
#include "StringView.h"
#include "Vec4.h"
#include "Formatter.h"

namespace XE
{
	template<Integral T>
	class TDataConverter<T> : public TRefer<IDataConverter>
	{
	public:
		FVariant Convert(const IDataProvider * ValueProvider) const
		{
			return TFormatter<T>().Parse(ValueProvider->GetValue());
		}
	};

#ifdef XE_ANSI
	template<>
	class TFormatter<char8_t>
	{
	public:
		FStringView Format(const char8_t & Value, FStringView Formal = FStringView::Empty) const
		{
			return XE::Format(Str("{}"), Char(Value));
		}

		char8_t Parse(FStringView String) const
		{
			return char8_t(String[0]);
		}
	};
#else
	template<>
	class TFormatter<char>
	{
	public:
		FStringView Format(const char & Value, FStringView Formal = FStringView::Empty) const
		{
			return XE::Format(Str("{}"), Char(Value));
		}

		char Parse(FStringView String) const
		{
			return char(String[0]);
		}
	};
#endif

	template<typename T>
	class TFormatter<TVec2<T>>
	{
	public:
		FStringView Format(const TVec2<T> & Value, FStringView Formal = FStringView::Empty) const
		{
			return XE::Format(Str("{},{}"), Value.X, Value.Y);
		}
		
		TVec2<T> Parse(FStringView String) const
		{
			TVector<FStringView> ComponentStrings = Strings::Split(String, Char(','));
			if (ComponentStrings.Size != 2)
				return {};

			TVec2<T> Value;
			Value.X = TFormatter<T>().Parse(ComponentStrings[0]);
			Value.Y = TFormatter<T>().Parse(ComponentStrings[1]);
			return Value;
		}
	};

	template<typename T>
	class TFormatter<TVec4<T>>
	{
	public:
		FStringView Format(const TVec4<T> & Value, FStringView Formal = FStringView::Empty) const
		{
			return XE::Format(Str("{},{},{},{}"), Value.X, Value.Y, Value.Z, Value.W);
		}
		
		TVec4<T> Parse(FStringView String) const
		{
			TVector<FStringView> ComponentStrings = Strings::Split(String, Char(','));
			if (ComponentStrings.Size == 4)
			{
				TVec4<T> Value;
				Value.X = TFormatter<T>().Parse(ComponentStrings[0]);
				Value.Y = TFormatter<T>().Parse(ComponentStrings[1]);
				Value.Z = TFormatter<T>().Parse(ComponentStrings[2]);
				Value.W = TFormatter<T>().Parse(ComponentStrings[3]);
				return Value;
			}
			if (ComponentStrings.Size == 1)
			{
				TVec4<T> Value;
				Value.X = Value.Y = Value.Z = Value.W = TFormatter<T>().Parse(ComponentStrings[0]);
				return Value;
			}
			else
				return {};

		}
	};

	template<>
	class CORE_API TFormatter<FColor32>
	{
	public:
		FStringView Format(const FColor32 & Value, FStringView Formal = FStringView::Empty) const;
		FColor32 Parse(FStringView String) const;
	};

	template<>
	class TFormatter<FVariant>
	{
	public:
		FStringView Format(const FVariant & Value, FStringView Formal = FStringView::Empty) const
		{
			switch(Value.ValueType)
			{
			case EValueType::Int8: return XE::Format(Str("{}"), Value.Int8Value);
			case EValueType::Int16: return XE::Format(Str("{}"), Value.Int16Value);
			case EValueType::Int32: return XE::Format(Str("{}"), Value.Int32Value);
			case EValueType::Int64: return XE::Format(Str("{}"), Value.Int64Value);
			case EValueType::UInt8: return XE::Format(Str("{}"), Value.UInt8Value);
			case EValueType::UInt16: return XE::Format(Str("{}"), Value.UInt16Value);
			case EValueType::UInt32: return XE::Format(Str("{}"), Value.UInt32Value);
			case EValueType::UInt64: return XE::Format(Str("{}"), Value.UInt64Value);
			case EValueType::Float32: return XE::Format(Str("{}"), Value.Float32Value);
			case EValueType::Float64: return XE::Format(Str("{}"), Value.Float64Value);
			case EValueType::StringView: return Value.StringViewValue;
			case EValueType::String: return Value.StringValue;
			case EValueType::ReferPointer: return XE::Format(Str("Refer {}{}"), static_cast<void*>(Value.ReferPointerValue), Value.ReferPointerValue ? Value.ReferPointerValue->ToString() : FStringView::Empty);
			case EValueType::ObjectPointer: return XE::Format(Str("Object {}{}"), static_cast<void*>(Value.ObjectPointerValue), Value.ObjectPointerValue ? Value.ObjectPointerValue->ToString() : FStringView::Empty);
			default: return Str("<Unnkonw Variant>");
			}
		}
	};
}
