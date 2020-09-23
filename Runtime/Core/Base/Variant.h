#pragma once

#include "Types.h"
#include "ReferPtr.h"
#include "String.h"

namespace XE
{
	class FType;
	class INativeType;
	
	template<typename T>
	inline constexpr bool IsRawVariableV = false;

	template<TriviallyCopyable T>
	inline constexpr bool IsRawVariableV<T> = true;
	
	constexpr size_t RawVariableSizeMax = 16;

	template<typename T>
	inline constexpr bool IsRawVariableFinalV = IsRawVariableV<T> && sizeof(T) <= RawVariableSizeMax;

	template<typename T>
	concept RawVariableFinal = IsRawVariableFinalV<T>;
	
	enum class EValueType
	{
		None = 0,
		Bool,
		
		Char,
		WChar,
		Char8,
		Char16,
		Char32,

		Int8,
		UInt8,
		
		Int16,
		UInt16,
		
		Int32,
		UInt32,
		
		Int64,
		UInt64,

		Float32,
		Float64,

		Vec2I,
		Vec3I,
		Vec4I,
		Vec2U,
		Vec3U,
		Vec4U,
		Vec2F,
		Vec3F,
		Vec4F,

		Vector2,
		Vector3,
		Vector4,
		
		Color,
		String,
		StringView,
		Type,
		Enum,
		
		Pointer,
		ObjectPointer,
		ReferPointer,
		
		Raw,
	};
	
	class CORE_API FVariant
	{
	public:
		FVariant() {}
		FVariant(NullPtrT) {}

		FVariant(const FVariant & That)
		{
			operator = (That);
		}

		auto operator <=> (const FVariant & Another) const
		{
			if (auto Result = ValueType <=> Another.ValueType; Result != 0)
				return Result;

			switch (ValueType)
			{
			case EValueType::Bool: return  BoolValue <=> Another.BoolValue;
			case EValueType::Char: return  CharValue <=> Another.CharValue;
			case EValueType::WChar: return  WCharValue <=> Another.WCharValue;
			case EValueType::Char8: return  Char8Value <=> Another.Char8Value;
			case EValueType::Char16: return  Char16Value <=> Another.Char16Value;
			case EValueType::Char32: return  Char32Value <=> Another.Char32Value;
			case EValueType::Int8: return  Int8Value <=> Another.Int8Value;
			case EValueType::UInt8: return  UInt8Value <=> Another.UInt8Value;
			case EValueType::Int16: return  Int16Value <=> Another.Int16Value;
			case EValueType::UInt16: return  UInt16Value <=> Another.UInt16Value;
			case EValueType::Int32: return  Int32Value <=> Another.Int32Value;
			case EValueType::UInt32: return  UInt32Value <=> Another.UInt32Value;
			case EValueType::Int64: return  Int64Value <=> Another.Int64Value;
			case EValueType::UInt64: return  UInt64Value <=> Another.UInt64Value;
			case EValueType::Float32: return  Compare(Float32Value, Another.Float32Value);
			case EValueType::Float64: return  Compare(Float64Value, Another.Float64Value);

			case EValueType::Vec2F: return Vec2FValue <=> Another.Vec2FValue;
			case EValueType::Vec3F: return Vec3FValue <=> Another.Vec3FValue;
			case EValueType::Vec4F: return Vec4FValue <=> Another.Vec4FValue;
			case EValueType::Vec2I: return Vec2IValue <=> Another.Vec2IValue;
			case EValueType::Vec3I: return Vec3IValue <=> Another.Vec3IValue;
			case EValueType::Vec4I: return Vec4IValue <=> Another.Vec4IValue;
			case EValueType::Vec2U: return Vec2UValue <=> Another.Vec2UValue;
			case EValueType::Vec3U: return Vec3UValue <=> Another.Vec3UValue;
			case EValueType::Vec4U: return Vec4UValue <=> Another.Vec4UValue;

			case EValueType::Vector2: return  Vector2Value <=> Another.Vector2Value;
			case EValueType::Vector3: return  Vector3Value <=> Another.Vector3Value;
			case EValueType::Vector4: return  Vector4Value <=> Another.Vector4Value;

			case EValueType::Color: return  ColorValue <=> Another.ColorValue;
			case EValueType::String: return StringValue <=> Move(Another.StringValue);
			case EValueType::StringView: return StringViewValue <=> Another.StringViewValue;
			case EValueType::Type: return NativeTypeValue <=> Another.NativeTypeValue;
			case EValueType::Enum: return EnumValue <=> Another.EnumValue;
				
			case EValueType::Pointer: return  PointerValue <=> Another.PointerValue;
			case EValueType::ObjectPointer: return  ObjectPointerValue <=> Another.ObjectPointerValue;
			case EValueType::ReferPointer: return  ReferPointerValue <=> Another.ReferPointerValue;
			case EValueType::Raw: return Memcmp(RawValue, Another.RawValue, RawVariableSizeMax) <=> 0;
			default: return 0 <=> 0;
			}
		}
		
		~FVariant()
		{
			Reset();
		}

		FVariant(FVariant && That) noexcept
		{
			ValueType = That.ValueType;
			That.ValueType = EValueType::None;

			switch (ValueType)
			{
			case EValueType::Bool:  BoolValue = That.BoolValue; break;
			case EValueType::Char:  CharValue = That.CharValue; break;
			case EValueType::WChar:  WCharValue = That.WCharValue; break;
			case EValueType::Char8:  Char8Value = That.Char8Value; break;
			case EValueType::Char16:  Char16Value = That.Char16Value; break;
			case EValueType::Char32:  Char32Value = That.Char32Value; break;
			case EValueType::Int8:  Int8Value = That.Int8Value; break;
			case EValueType::UInt8:  UInt8Value = That.UInt8Value; break;
			case EValueType::Int16:  Int16Value = That.Int16Value; break;
			case EValueType::UInt16:  UInt16Value = That.UInt16Value; break;
			case EValueType::Int32:  Int32Value = That.Int32Value; break;
			case EValueType::UInt32:  UInt32Value = That.UInt32Value; break;
			case EValueType::Int64:  Int64Value = That.Int64Value; break;
			case EValueType::UInt64:  UInt64Value = That.UInt64Value; break;
			case EValueType::Float32:  Float32Value = That.Float32Value; break;
			case EValueType::Float64:  Float64Value = That.Float64Value; break;
				
			case EValueType::Vec2F:  Vec2FValue = That.Vec2FValue; break;
			case EValueType::Vec3F:  Vec3FValue = That.Vec3FValue; break;
			case EValueType::Vec4F:  Vec4FValue = That.Vec4FValue; break;
			case EValueType::Vec2I:  Vec2IValue = That.Vec2IValue; break;
			case EValueType::Vec3I:  Vec3IValue = That.Vec3IValue; break;
			case EValueType::Vec4I:  Vec4IValue = That.Vec4IValue; break;
			case EValueType::Vec2U:  Vec2UValue = That.Vec2UValue; break;
			case EValueType::Vec3U:  Vec3UValue = That.Vec3UValue; break;
			case EValueType::Vec4U:  Vec4UValue = That.Vec4UValue; break;

			case EValueType::Vector2:  Vector2Value = That.Vector2Value; break;
			case EValueType::Vector3:  Vector3Value = That.Vector3Value; break;
			case EValueType::Vector4:  Vector4Value = That.Vector4Value; break;
				
			case EValueType::Color:  ColorValue = That.ColorValue; break;
			case EValueType::String: Memzero(RawValue); StringValue = Move(That.StringValue); break;
			case EValueType::StringView: StringViewValue = Move(That.StringViewValue); break;
			case EValueType::Type: NativeTypeValue = That.NativeTypeValue; break;
			case EValueType::Enum: EnumValue = That.EnumValue; break;
				
			case EValueType::Pointer:
				PointerValue = That.PointerValue;
				PointerDeleter = That.PointerDeleter;
				That.PointerValue = nullptr;
				That.PointerDeleter = nullptr;
				break;
			case EValueType::ObjectPointer:
				ObjectPointerValue = That.ObjectPointerValue;
				ObjectPointerDeleter = That.ObjectPointerDeleter;
				That.ObjectPointerValue = nullptr;
				That.ObjectPointerDeleter = nullptr;
				break;
			case EValueType::ReferPointer:
				ReferPointerValue = That.ReferPointerValue;
				That.ReferPointerValue = nullptr;
				break;
			case EValueType::Raw:
				Memcpy(RawValue, That.RawValue, RawVariableSizeMax);
				break;
			default:
				ValueType = EValueType::None;
				break;
			}
		}

		template<typename T>
		FVariant(const T & Value)
		{
			operator=(Value);
		}

		explicit operator bool() const { return ValueType != EValueType::None; }
		bool HasValue() const { return ValueType != EValueType::None; }

		const class FType & GetObjectType() const;
		
		FVariant & operator = (const FVariant & That)
		{
			Reset();

			ValueType = That.ValueType;

			switch (ValueType)
			{
			case EValueType::None: break;
			case EValueType::Bool:  BoolValue = That.BoolValue; break;
			case EValueType::Char:  CharValue = That.CharValue; break;
			case EValueType::WChar:  WCharValue = That.WCharValue; break;
			case EValueType::Char8:  Char8Value = That.Char8Value; break;
			case EValueType::Char16:  Char16Value = That.Char16Value; break;
			case EValueType::Char32:  Char32Value = That.Char32Value; break;
			case EValueType::Int8:  Int8Value = That.Int8Value; break;
			case EValueType::UInt8:  UInt8Value = That.UInt8Value; break;
			case EValueType::Int16:  Int16Value = That.Int16Value; break;
			case EValueType::UInt16:  UInt16Value = That.UInt16Value; break;
			case EValueType::Int32:  Int32Value = That.Int32Value; break;
			case EValueType::UInt32:  UInt32Value = That.UInt32Value; break;
			case EValueType::Int64:  Int64Value = That.Int64Value; break;
			case EValueType::UInt64:  UInt64Value = That.UInt64Value; break;
			case EValueType::Float32:  Float32Value = That.Float32Value; break;
			case EValueType::Float64:  Float64Value = That.Float64Value; break;

			case EValueType::Vec2I:  Vec2IValue = That.Vec2IValue; break;
			case EValueType::Vec3I:  Vec3IValue = That.Vec3IValue; break;
			case EValueType::Vec4I:  Vec4IValue = That.Vec4IValue; break;
			case EValueType::Vec2U:  Vec2UValue = That.Vec2UValue; break;
			case EValueType::Vec3U:  Vec3UValue = That.Vec3UValue; break;
			case EValueType::Vec4U:  Vec4UValue = That.Vec4UValue; break;
			case EValueType::Vec2F:  Vec2FValue = That.Vec2FValue; break;
			case EValueType::Vec3F:  Vec3FValue = That.Vec3FValue; break;
			case EValueType::Vec4F:  Vec4FValue = That.Vec4FValue; break;

			case EValueType::Vector2:  Vector2Value = That.Vector2Value; break;
			case EValueType::Vector3:  Vector3Value = That.Vector3Value; break;
			case EValueType::Vector4:  Vector4Value = That.Vector4Value; break;
				
			case EValueType::Color:  ColorValue = That.ColorValue; break;
			case EValueType::String:
				Memzero(RawValue);
				StringValue = That.StringValue;
				break;
			case EValueType::StringView:
				Memzero(RawValue);
				ValueType = EValueType::String;
				StringValue = That.StringViewValue;
				break;
			case EValueType::Type:
				NativeTypeValue = That.NativeTypeValue;
				break;
			case EValueType::Enum:
				EnumValue = That.EnumValue;
				EnumType = That.EnumType;
				break;
			case EValueType::Pointer:
				PointerValue = That.PointerValue;
				PointerDeleter = nullptr;
				break;
			case EValueType::ObjectPointer:
				ObjectPointerValue = That.ObjectPointerValue;
				ObjectPointerDeleter = nullptr;
				break;
			case EValueType::ReferPointer:
				ReferPointerValue = That.ReferPointerValue;
				ReferAddRef(ReferPointerValue);
				break;
			case EValueType::Raw:
			{
				Memcpy(RawValue, That.RawValue, RawVariableSizeMax);
				break;
			}
			default:
				throw EError::BadFormat;
			}
			return *this;
		}

		bool operator == (const FVariant & Another) const
		{
			if (ValueType != Another.ValueType) return false;

			switch (ValueType)
			{
			case EValueType::None: return true;
			case EValueType::Bool: return BoolValue == Another.BoolValue;
			case EValueType::Char: return CharValue == Another.CharValue;
			case EValueType::WChar: return WCharValue == Another.WCharValue;
			case EValueType::Char8: return Char8Value == Another.Char8Value;
			case EValueType::Char16: return Char16Value == Another.Char16Value;
			case EValueType::Char32: return Char32Value == Another.Char32Value;
			case EValueType::Int8: return Int8Value == Another.Int8Value;
			case EValueType::UInt8: return UInt8Value == Another.UInt8Value;
			case EValueType::Int16: return Int16Value == Another.Int16Value;
			case EValueType::UInt16: return UInt16Value == Another.UInt16Value;
			case EValueType::Int32: return Int32Value == Another.Int32Value;
			case EValueType::UInt32: return UInt32Value == Another.UInt32Value;
			case EValueType::Int64: return Int64Value == Another.Int64Value;
			case EValueType::UInt64: return UInt64Value == Another.UInt64Value;
			case EValueType::Float32: return Float32Value == Another.Float32Value;
			case EValueType::Float64: return Float64Value == Another.Float64Value;
			case EValueType::Color: return ColorValue == Another.ColorValue;
			case EValueType::String: return StringValue == Another.StringValue;
			case EValueType::StringView: return StringViewValue == Another.StringViewValue;
			case EValueType::Type: return NativeTypeValue == Another.NativeTypeValue;
			case EValueType::Enum: return EnumValue == Another.EnumValue && EnumType == Another.EnumType;
			case EValueType::Pointer: return PointerValue == Another.PointerValue;
			case EValueType::ObjectPointer: return ObjectPointerValue == Another.ObjectPointerValue;
			case EValueType::ReferPointer: return ReferPointerValue == Another.ReferPointerValue;
			case EValueType::Raw: return Memcmp(RawValue, Another.RawValue, RawVariableSizeMax) == 0;
			default:
				return false;
			}
		}

		template<typename T>
		FVariant & operator = (const T & Value)
		{
			Reset();

			if constexpr (IsRefV<T>)
			{
				using ReferenceValueType = typename IsRef<T>::Type;

				if constexpr (IsReferAbleV<ReferenceValueType>)
				{
					ValueType = EValueType::ReferPointer;
					ReferPointerValue = ReferAddRef(&(Value.Value));
				}
				else if constexpr (IsObjectV<T>)
				{
					ValueType = EValueType::ObjectPointer;
					ObjectPointerValue = &(Value.Value);
					ObjectPointerDeleter = nullptr;
				}
				else
				{
					ValueType = EValueType::Pointer;
					PointerValue = &(Value.Value);
					PointerDeleter = nullptr;
				}
			}
			else if constexpr (IsSameV<T, void>) { ValueType = EValueType::None; }
			else if constexpr (IsSameV<T, VoidT<T>>) { ValueType = EValueType::None; }
			else if constexpr (IsSameV<T, bool>) { ValueType = EValueType::Bool; BoolValue = Value; }
			else if constexpr (IsSameV<T, char>) { ValueType = EValueType::Char; CharValue = Value; }
			else if constexpr (IsSameV<T, wchar_t>) { ValueType = EValueType::WChar; WCharValue = Value; }
			else if constexpr (IsSameV<T, char8_t>) { ValueType = EValueType::Char8; Char8Value = Value; }
			else if constexpr (IsSameV<T, char16_t>) { ValueType = EValueType::Char16; Char16Value = Value; }
			else if constexpr (IsSameV<T, char32_t>) { ValueType = EValueType::Char32; Char32Value = Value; }
			else if constexpr (IsSameV<T, int8_t>) { ValueType = EValueType::Int8; Int8Value = Value; }
			else if constexpr (IsSameV<T, uint8_t>) { ValueType = EValueType::UInt8; UInt8Value = Value; }
			else if constexpr (IsSameV<T, int16_t>) { ValueType = EValueType::Int16; Int16Value = Value; }
			else if constexpr (IsSameV<T, uint16_t>) { ValueType = EValueType::UInt16; UInt16Value = Value; }
			else if constexpr (IsSameV<T, int32_t>) { ValueType = EValueType::Int32; Int32Value = Value; }
			else if constexpr (IsSameV<T, uint32_t>) { ValueType = EValueType::UInt32; UInt32Value = Value; }
			else if constexpr (IsSameV<T, int64_t>) { ValueType = EValueType::Int64; Int64Value = Value; }
			else if constexpr (IsSameV<T, uint64_t>) { ValueType = EValueType::UInt64; UInt64Value = Value; }
			else if constexpr (IsSameV<T, float32_t>) { ValueType = EValueType::Float32; Float32Value = Value; }
			else if constexpr (IsSameV<T, float64_t>) { ValueType = EValueType::Float64; Float64Value = Value; }
			else if constexpr (IsSameV<T, float32_t>) { ValueType = EValueType::Float32; Float32Value = Value; }
			else if constexpr (IsSameV<T, Vec2F>) { ValueType = EValueType::Vec2F; Vec2FValue = Value; }
			else if constexpr (IsSameV<T, Vec3F>) { ValueType = EValueType::Vec3F; Vec3FValue = Value; }
			else if constexpr (IsSameV<T, Vec4F>) { ValueType = EValueType::Vec4F; Vec4FValue = Value; }
			else if constexpr (IsSameV<T, Vec2I>) { ValueType = EValueType::Vec2I; Vec2IValue = Value; }
			else if constexpr (IsSameV<T, Vec3I>) { ValueType = EValueType::Vec3I; Vec3IValue = Value; }
			else if constexpr (IsSameV<T, Vec4I>) { ValueType = EValueType::Vec4I; Vec4IValue = Value; }
			else if constexpr (IsSameV<T, Vec2U>) { ValueType = EValueType::Vec2U; Vec2UValue = Value; }
			else if constexpr (IsSameV<T, Vec3U>) { ValueType = EValueType::Vec3U; Vec3UValue = Value; }
			else if constexpr (IsSameV<T, Vec4U>) { ValueType = EValueType::Vec4U; Vec4UValue = Value; }
			else if constexpr (IsSameV<T, FVector2>) { ValueType = EValueType::Vector2; Vector2Value = Value; }
			else if constexpr (IsSameV<T, FVector3>) { ValueType = EValueType::Vector3; Vector3Value = Value; }
			else if constexpr (IsSameV<T, FVector4>) { ValueType = EValueType::Vector4; Vector4Value = Value; }
			else if constexpr (IsSameV<T, FColor32>) { ValueType = EValueType::Color; ColorValue = Value; }
			else if constexpr (IsSameV<T, FString>) { ValueType = EValueType::String; Memzero(RawValue); StringValue = Value; }
			else if constexpr (IsSameV<T, FStringView>) { ValueType = EValueType::StringView; StringViewValue = Value; }
			else if constexpr (IsSameV<T, const INativeType *>) { ValueType = EValueType::Type; NativeTypeValue = Value; }
			else if constexpr (IsEnumV<T>) { ValueType = EValueType::Enum; EnumValue = static_cast<uintx_t>(Value); EnumType = &typeid(T); }
			else if constexpr (IsReferPointerV<T>) { ValueType = EValueType::ReferPointer; ReferPointerValue = ReferAddRef(Value.Get()); }
			else if constexpr (IsPointerV<T>)
			{
				using PointerValueType = RemovePointerT<T>;
				if constexpr (IsReferAbleV<PointerValueType>)
				{
					ValueType = EValueType::ReferPointer;
					ReferPointerValue = ReferAddRef(Value);
				}
				else if constexpr (IsObjectV<PointerValueType>)
				{
					ValueType = EValueType::ObjectPointer;
					ObjectPointerValue = Value;
					ObjectPointerDeleter = nullptr;
				}
				else
				{
					ValueType = EValueType::Pointer;
					PointerValue = Value;
					PointerDeleter = nullptr;
				}
			}
			else if constexpr (IsRawVariableFinalV<T>)
			{
				ValueType = EValueType::Raw;
				Memzero(RawValue + sizeof(T), RawVariableSizeMax - sizeof(T));
				Memcpy(RawValue, &Value, sizeof(T));
			}
			else if constexpr (IsReferPointerV<T>)
			{
				ValueType = EValueType::ReferPointer;
				ReferPointerValue = ReferAddRef(Value.Get());
			}
			else
			{
				if constexpr (IsReferAbleV<T>)
				{
					ValueType = EValueType::ReferPointer;
					ReferPointerValue = New<T>(Value);
				}
				else if constexpr (IsObjectV<T>)
				{
					ValueType = EValueType::ObjectPointer;
					ObjectPointerValue = New<T>(Value);
					ObjectPointerDeleter = [](IObject * Object) { Delete(Object); };
				}
				else
				{
					ValueType = EValueType::Pointer;
					PointerValue = New<T>(Value);
					PointerDeleter = [](void * Pointer) { DeleteUnSafe(static_cast<T *>(Pointer)); };
				}
			}
			return *this;
		}

		template<CopyConstructible T>
		explicit operator T() const { return Get<T>(); }
		
		template<CopyConstructible T>
		T Get() const
		{
			if constexpr (IsLValueReferenceV<RemoveCVT<T>>)
			{
				using UnderlyingT = RemoveCVRefT<T>;
				if constexpr (IsSameV<T, FVariant>) { return *this; }
				else if constexpr (IsSameV<UnderlyingT, bool>) { if (ValueType != EValueType::Bool) throw EError::BadFormat; return BoolValue; }
				else if constexpr (IsSameV<UnderlyingT, char>) { if (ValueType != EValueType::Char) throw EError::BadFormat; return CharValue; }
				else if constexpr (IsSameV<UnderlyingT, wchar_t>) { if (ValueType != EValueType::WChar) throw EError::BadFormat; return WCharValue; }
				else if constexpr (IsSameV<UnderlyingT, char8_t>) { if (ValueType != EValueType::Char8) throw EError::BadFormat; return Char8Value; }
				else if constexpr (IsSameV<UnderlyingT, char16_t>) { if (ValueType != EValueType::Char16) throw EError::BadFormat; return Char16Value; }
				else if constexpr (IsSameV<UnderlyingT, char32_t>) { if (ValueType != EValueType::Char32) throw EError::BadFormat; return Char32Value; }
				else if constexpr (IsSameV<UnderlyingT, int8_t>) { if (ValueType != EValueType::Int8) throw EError::BadFormat; return Int8Value; }
				else if constexpr (IsSameV<UnderlyingT, uint8_t>) { if (ValueType != EValueType::UInt8) throw EError::BadFormat; return UInt8Value; }
				else if constexpr (IsSameV<UnderlyingT, int16_t>) { if (ValueType != EValueType::Int16) throw EError::BadFormat; return Int16Value; }
				else if constexpr (IsSameV<UnderlyingT, uint16_t>) { if (ValueType != EValueType::UInt16) throw EError::BadFormat; return UInt16Value; }
				else if constexpr (IsSameV<UnderlyingT, int32_t>) { if (ValueType != EValueType::Int32) throw EError::BadFormat; return Int32Value; }
				else if constexpr (IsSameV<UnderlyingT, uint32_t>) { if (ValueType != EValueType::UInt32) throw EError::BadFormat; return UInt32Value; }
				else if constexpr (IsSameV<UnderlyingT, int64_t>) { if (ValueType != EValueType::Int64) throw EError::BadFormat; return Int64Value; }
				else if constexpr (IsSameV<UnderlyingT, uint64_t>) { if (ValueType != EValueType::UInt64) throw EError::BadFormat; return UInt64Value; }
				else if constexpr (IsSameV<UnderlyingT, float32_t>) { if (ValueType != EValueType::Float32) throw EError::BadFormat; return Float32Value; }
				else if constexpr (IsSameV<UnderlyingT, float64_t>) { if (ValueType != EValueType::Float64) throw EError::BadFormat; return Float64Value; }
				else if constexpr (IsSameV<UnderlyingT, float32_t>) { if (ValueType != EValueType::Float32) throw EError::BadFormat; return Float32Value; }
				else if constexpr (IsSameV<UnderlyingT, Vec2F>) { if (ValueType != EValueType::Vec2F) throw EError::BadFormat; return Vec2FValue; }
				else if constexpr (IsSameV<UnderlyingT, Vec3F>) { if (ValueType != EValueType::Vec3F) throw EError::BadFormat; return Vec3FValue; }
				else if constexpr (IsSameV<UnderlyingT, Vec4F>) { if (ValueType != EValueType::Vec4F) throw EError::BadFormat; return Vec4FValue; }
				else if constexpr (IsSameV<UnderlyingT, Vec2I>) { if (ValueType != EValueType::Vec2I) throw EError::BadFormat; return Vec2IValue; }
				else if constexpr (IsSameV<UnderlyingT, Vec3I>) { if (ValueType != EValueType::Vec3I) throw EError::BadFormat; return Vec3IValue; }
				else if constexpr (IsSameV<UnderlyingT, Vec4I>) { if (ValueType != EValueType::Vec4I) throw EError::BadFormat; return Vec4IValue; }
				else if constexpr (IsSameV<UnderlyingT, Vec2U>) { if (ValueType != EValueType::Vec2U) throw EError::BadFormat; return Vec2UValue; }
				else if constexpr (IsSameV<UnderlyingT, Vec3U>) { if (ValueType != EValueType::Vec3U) throw EError::BadFormat; return Vec3UValue; }
				else if constexpr (IsSameV<UnderlyingT, Vec4U>) { if (ValueType != EValueType::Vec4U) throw EError::BadFormat; return Vec4UValue; }
				else if constexpr (IsSameV<UnderlyingT, FVector2>) { if (ValueType != EValueType::Vector2) throw EError::BadFormat; return Vector2Value; }
				else if constexpr (IsSameV<UnderlyingT, FVector3>) { if (ValueType != EValueType::Vector3) throw EError::BadFormat; return Vector3Value; }
				else if constexpr (IsSameV<UnderlyingT, FVector4>) { if (ValueType != EValueType::Vector4) throw EError::BadFormat; return Vector4Value; }
				else if constexpr (IsSameV<UnderlyingT, FColor32>) { if (ValueType != EValueType::Color) throw EError::BadFormat; return ColorValue; }
				else if constexpr (IsSameV<UnderlyingT, FString>) { if (ValueType != EValueType::String) throw EError::BadFormat; return StringValue; }
				else if constexpr (IsSameV<UnderlyingT, FStringView>) { if (ValueType != EValueType::StringView) throw EError::BadFormat; return StringViewValue; }
				else if constexpr (IsEnumV<UnderlyingT>) { if (ValueType != EValueType::Enum) throw EError::BadFormat; return EnumValue; }
				else
				{
					if (ValueType == EValueType::Pointer)
						return *static_cast<UnderlyingT *>(PointerValue);
					else if (ValueType == EValueType::ReferPointer)
					{
						if constexpr (IsReferAbleV<T>)
							return *static_cast<UnderlyingT *>(ReferPointerValue);
					}
					else if (ValueType == EValueType::ObjectPointer)
					{
						if constexpr (IsObjectV<UnderlyingT>)
							return *static_cast<UnderlyingT *>(ObjectPointerValue);
					}
					else if (ValueType == EValueType::Raw)
					{
						if constexpr (IsRawVariableFinalV<UnderlyingT>)
							return *reinterpret_cast<UnderlyingT *>((void *)(RawValue));
					}
					else {}
					
					throw EError::BadFormat;
				}
			}
			else if constexpr (IsSameV<T, FVariant>) { return *this; }
			else if constexpr (IsSameV<T, bool>) { if (ValueType != EValueType::Bool) throw EError::BadFormat; return BoolValue; }
			else if constexpr (IsSameV<T, char>) { if (ValueType != EValueType::Char) throw EError::BadFormat; return CharValue; }
			else if constexpr (IsSameV<T, wchar_t>) { if (ValueType != EValueType::WChar) throw EError::BadFormat; return WCharValue; }
			else if constexpr (IsSameV<T, char8_t>) { if (ValueType != EValueType::Char8) throw EError::BadFormat; return Char8Value; }
			else if constexpr (IsSameV<T, char16_t>) { if (ValueType != EValueType::Char16) throw EError::BadFormat; return Char16Value; }
			else if constexpr (IsSameV<T, char32_t>) { if (ValueType != EValueType::Char32) throw EError::BadFormat; return Char32Value; }
			else if constexpr (IsSameV<T, int8_t>) { if (ValueType != EValueType::Int8) throw EError::BadFormat; return Int8Value; }
			else if constexpr (IsSameV<T, uint8_t>) { if (ValueType != EValueType::UInt8) throw EError::BadFormat; return UInt8Value; }
			else if constexpr (IsSameV<T, int16_t>) { if (ValueType != EValueType::Int16) throw EError::BadFormat; return Int16Value; }
			else if constexpr (IsSameV<T, uint16_t>) { if (ValueType != EValueType::UInt16) throw EError::BadFormat; return UInt16Value; }
			else if constexpr (IsSameV<T, int32_t>) { if (ValueType != EValueType::Int32) throw EError::BadFormat; return Int32Value; }
			else if constexpr (IsSameV<T, uint32_t>) { if (ValueType != EValueType::UInt32) throw EError::BadFormat; return UInt32Value; }
			else if constexpr (IsSameV<T, int64_t>) { if (ValueType != EValueType::Int64) throw EError::BadFormat; return Int64Value; }
			else if constexpr (IsSameV<T, uint64_t>) { if (ValueType != EValueType::UInt64) throw EError::BadFormat; return UInt64Value; }
			else if constexpr (IsSameV<T, float32_t>) { if (ValueType != EValueType::Float32) throw EError::BadFormat; return Float32Value; }
			else if constexpr (IsSameV<T, float64_t>) { if (ValueType != EValueType::Float64) throw EError::BadFormat; return Float64Value; }
			else if constexpr (IsSameV<T, float32_t>) { if (ValueType != EValueType::Float32) throw EError::BadFormat; return Float32Value; }
			else if constexpr (IsSameV<T, Vec2F>) { if (ValueType != EValueType::Vec2F) throw EError::BadFormat; return Vec2FValue; }
			else if constexpr (IsSameV<T, Vec3F>) { if (ValueType != EValueType::Vec3F) throw EError::BadFormat; return Vec3FValue; }
			else if constexpr (IsSameV<T, Vec4F>) { if (ValueType != EValueType::Vec4F) throw EError::BadFormat; return Vec4FValue; }
			else if constexpr (IsSameV<T, Vec2I>) { if (ValueType != EValueType::Vec2I) throw EError::BadFormat; return Vec2IValue; }
			else if constexpr (IsSameV<T, Vec3I>) { if (ValueType != EValueType::Vec3I) throw EError::BadFormat; return Vec3IValue; }
			else if constexpr (IsSameV<T, Vec4I>) { if (ValueType != EValueType::Vec4I) throw EError::BadFormat; return Vec4IValue; }
			else if constexpr (IsSameV<T, Vec2U>) { if (ValueType != EValueType::Vec2U) throw EError::BadFormat; return Vec2UValue; }
			else if constexpr (IsSameV<T, Vec3U>) { if (ValueType != EValueType::Vec3U) throw EError::BadFormat; return Vec3UValue; }
			else if constexpr (IsSameV<T, Vec4U>) { if (ValueType != EValueType::Vec4U) throw EError::BadFormat; return Vec4UValue; }
			else if constexpr (IsSameV<T, FVector2>) { if (ValueType != EValueType::Vector2) throw EError::BadFormat; return Vector2Value; }
			else if constexpr (IsSameV<T, FVector3>) { if (ValueType != EValueType::Vector3) throw EError::BadFormat; return Vector3Value; }
			else if constexpr (IsSameV<T, FVector4>) { if (ValueType != EValueType::Vector4) throw EError::BadFormat; return Vector4Value; }
			else if constexpr (IsSameV<T, FColor32>) { if (ValueType != EValueType::Color) throw EError::BadFormat; return ColorValue; }
			else if constexpr (IsSameV<T, FString>) { if (ValueType != EValueType::String) throw EError::BadFormat; return StringValue; }
			else if constexpr (IsSameV<T, FStringView>)
			{
				if (ValueType == EValueType::String && !IsReferenceV<T>)
					return StringValue;
				else if (ValueType == EValueType::StringView)
					return StringViewValue;
				else
					throw EError::BadFormat;
			}
			else if constexpr (IsSameV<RemoveCVRefT<T>, const INativeType *>) { if (ValueType != EValueType::Type) throw EError::BadFormat; return NativeTypeValue; }
			else if constexpr (IsSameV<T, FType>) { if (ValueType != EValueType::Type) throw EError::BadFormat; return NativeTypeValue; }
			else if constexpr (IsEnumV<T>) { if (ValueType != EValueType::Enum) throw EError::BadFormat; return static_cast<T>( EnumValue); }
			else if constexpr (IsReferPointerV<T>)
			{
				if (ValueType == EValueType::None)
					return {};
				if (ValueType == EValueType::ReferPointer)
				{
					using ReferValueType = typename IsReferPointer<T>::Type;
					return T(static_cast<ReferValueType*>(ReferAddRef(ReferPointerValue)));
				}

				throw EError::BadFormat;
			}
			else if constexpr (IsPointerV<T>)
			{
				if (ValueType == EValueType::None)
					return nullptr;

				if (ValueType == EValueType::Pointer)
					return static_cast<T>(PointerValue);

				if (ValueType == EValueType::ObjectPointer)
					return static_cast<T>(ObjectPointerValue);

				if (ValueType == EValueType::ReferPointer)
					return static_cast<T>(ReferPointerValue);

				throw EError::BadFormat;
			}
			else if constexpr (!IsAbstractV<T>)
			{
				if (ValueType == EValueType::Pointer)
					return *static_cast<const T *>(PointerValue);
				else if (ValueType == EValueType::ReferPointer)
				{
					if constexpr (IsReferAbleV<T>)
						return *static_cast<const T *>(ReferPointerValue);
				}
				else if (ValueType == EValueType::ObjectPointer)
				{
					if constexpr (IsObjectV<T>)
						return *static_cast<const T *>(ObjectPointerValue);
				}
				else if (ValueType == EValueType::Raw)
				{
					if constexpr (IsRawVariableFinalV<T>)
						return *reinterpret_cast<const T *>(RawValue);
				}
				else {}
				
				throw EError::BadFormat;
			}
			else
				throw EError::BadFormat;
		}

		template<typename T>
		void Create()
		{
			if constexpr (IsSameV<T, bool>) { ValueType = EValueType::Bool; BoolValue = {}; }
			else if constexpr (IsSameV<T, char>) { ValueType = EValueType::Char; CharValue = {}; }
			else if constexpr (IsSameV<T, wchar_t>) { ValueType = EValueType::WChar; WCharValue = {}; }
			else if constexpr (IsSameV<T, char8_t>) { ValueType = EValueType::Char8; Char8Value = {}; }
			else if constexpr (IsSameV<T, char16_t>) { ValueType = EValueType::Char16; Char16Value = {}; }
			else if constexpr (IsSameV<T, char32_t>) { ValueType = EValueType::Char16; Char32Value = {}; }
			else if constexpr (IsSameV<T, int8_t>) { ValueType = EValueType::Int8; Int8Value = {}; }
			else if constexpr (IsSameV<T, uint8_t>) { ValueType = EValueType::UInt8; UInt8Value = {}; }
			else if constexpr (IsSameV<T, int16_t>) { ValueType = EValueType::Int16; Int16Value = {}; }
			else if constexpr (IsSameV<T, uint16_t>) { ValueType = EValueType::UInt16; UInt16Value = {}; }
			else if constexpr (IsSameV<T, int32_t>) { ValueType = EValueType::Int32; Int32Value = {}; }
			else if constexpr (IsSameV<T, uint32_t>) { ValueType = EValueType::UInt32; UInt32Value = {}; }
			else if constexpr (IsSameV<T, int64_t>) { ValueType = EValueType::Int64; Int64Value = {}; }
			else if constexpr (IsSameV<T, uint64_t>) { ValueType = EValueType::UInt64; UInt64Value = {}; }
			else if constexpr (IsSameV<T, float32_t>) { ValueType = EValueType::Float32; Float32Value = {}; }
			else if constexpr (IsSameV<T, float64_t>) { ValueType = EValueType::Float64; Float64Value = {}; }
			else if constexpr (IsSameV<T, float32_t>) { ValueType = EValueType::Float32; Float32Value = {}; }
			else if constexpr (IsSameV<T, Vec2F>) { ValueType = EValueType::Vec2F; Vec2FValue = {}; }
			else if constexpr (IsSameV<T, Vec3F>) { ValueType = EValueType::Vec3F; Vec3FValue = {}; }
			else if constexpr (IsSameV<T, Vec4F>) { ValueType = EValueType::Vec4F; Vec4FValue = {}; }
			else if constexpr (IsSameV<T, Vec2I>) { ValueType = EValueType::Vec2I; Vec2IValue = {}; }
			else if constexpr (IsSameV<T, Vec3I>) { ValueType = EValueType::Vec3I; Vec3IValue = {}; }
			else if constexpr (IsSameV<T, Vec4I>) { ValueType = EValueType::Vec4I; Vec4IValue = {}; }
			else if constexpr (IsSameV<T, Vec2U>) { ValueType = EValueType::Vec2U; Vec2UValue = {}; }
			else if constexpr (IsSameV<T, Vec3U>) { ValueType = EValueType::Vec3U; Vec3UValue = {}; }
			else if constexpr (IsSameV<T, Vec4U>) { ValueType = EValueType::Vec4U; Vec4UValue = {}; }
			else if constexpr (IsSameV<T, FVector2>) { ValueType = EValueType::Vector2; Vector2Value = {}; }
			else if constexpr (IsSameV<T, FVector3>) { ValueType = EValueType::Vector3; Vector3Value = {}; }
			else if constexpr (IsSameV<T, FVector4>) { ValueType = EValueType::Vector4; Vector4Value = {}; }
			else if constexpr (IsSameV<T, FColor32>) { ValueType = EValueType::Color; ColorValue = {}; }
			else if constexpr (IsSameV<T, FString>) { ValueType = EValueType::String; Memzero(RawValue); StringValue = {}; }
			else if constexpr (IsSameV<T, FStringView>) { ValueType = EValueType::StringView; Memzero(RawValue); }
			else if constexpr (IsSameV<T, const INativeType *>) { ValueType = EValueType::Type; NativeTypeValue = nullptr; }
			else if constexpr (IsEnumV<T>) { ValueType = EValueType::Enum; EnumValue = 0; }
			else if constexpr (IsPointerV<T>) { ValueType = EValueType::Pointer; PointerValue = {}; }
			else if constexpr (IsReferPointerV<T>)
			{
				ValueType = EValueType::ReferPointer;
				ReferPointerValue = nullptr;
			}
			else if constexpr (!IsAbstractV<T>)
			{
				if constexpr (IsReferAbleV<T>)
				{
					ValueType = EValueType::ReferPointer;
					ReferPointerValue = New<T>();
				}
				else if constexpr (IsObjectV<T>)
				{
					ValueType = EValueType::ObjectPointer;
					ObjectPointerValue = New<T>();
					ObjectPointerDeleter = [](IObject * Object) { Delete(Object); };
				}
				else if constexpr (IsRawVariableFinalV<T>)
				{
					Memzero(RawValue);
					ValueType = EValueType::Raw;
					new (RawValue) T();
				}
				else
				{
					ValueType = EValueType::Pointer;
					PointerValue = New<T>();
					PointerDeleter = [](void * Pointer) { DeleteUnSafe(static_cast<T *>(Pointer)); };
				}
			}
			else
				throw EError::BadFormat;
		}

		void Reset()
		{
			if (ValueType == EValueType::String)
				StringValue.Reset();
			else if (ValueType == EValueType::Pointer)
			{
				if (PointerDeleter)
					PointerDeleter(PointerValue);
				PointerValue = nullptr;
				PointerDeleter = nullptr;
			}
			else if (ValueType == EValueType::ObjectPointer)
			{
				if (ObjectPointerDeleter)
				{
					ObjectPointerDeleter(ObjectPointerValue);
					ObjectPointerDeleter = nullptr;
					ObjectPointerValue = nullptr;
				}
			}
			else if (ValueType == EValueType::ReferPointer)
			{
				if (ReferPointerValue)
				{
					ReferRelease(ReferPointerValue);
					ReferPointerValue = nullptr;
				}
			}
			else if (ValueType == EValueType::Raw) {}
			else {}
			ValueType = EValueType::None;
		}
		
		void Abandon()
		{
			ValueType = EValueType::None;
		}

		//INativeType * NativeType = nullptr;
		EValueType ValueType = EValueType::None;
		union
		{
			bool BoolValue;
			char CharValue;
			wchar_t WCharValue;
			char8_t Char8Value;
			char16_t Char16Value;
			char32_t Char32Value;

			int8_t Int8Value;
			uint8_t UInt8Value;
			
			int16_t Int16Value;
			uint16_t UInt16Value;
			
			int32_t Int32Value;
			uint32_t UInt32Value;
			
			int64_t Int64Value;
			uint64_t UInt64Value;

			float32_t Float32Value;
			float64_t Float64Value;

			Vec2I Vec2IValue;
			Vec3I Vec3IValue;
			Vec4I Vec4IValue;
			
			Vec2U Vec2UValue;
			Vec3U Vec3UValue;
			Vec4U Vec4UValue;

			Vec2F Vec2FValue;
			Vec3F Vec3FValue;
			Vec4F Vec4FValue;

			FVector2 Vector2Value;
			FVector3 Vector3Value;
			FVector4 Vector4Value;
			
			FColor32 ColorValue;
			FString StringValue;
			FStringView StringViewValue;
			const INativeType * NativeTypeValue = nullptr;

			struct
			{
				uintx_t EnumValue;
				//const INativeType * EnumType;
				const type_info * EnumType;
			};

			struct
			{
				void * PointerValue;
				void (*PointerDeleter)(void *);
			};

			struct
			{
				IObject * ObjectPointerValue;
				void (*ObjectPointerDeleter)(IObject *);
			};
			
			IRefer * ReferPointerValue;
			
			byte_t RawValue[RawVariableSizeMax];
		};

		public:
			static FVariant Empty;
	};
}
