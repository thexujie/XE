#pragma once

#include "Types.h"
#include "Flag.h"
#include "StringView.h"
#include "Variant.h"
#include "Formatter.h"

namespace XE
{
	class CORE_API IResourceDataProvider : public IRefer
	{
	};
	
	class CORE_API IDataProvider : public IRefer
	{
	public:
		virtual FStringView GetName() const = 0;
		virtual FStringView GetValue() const = 0;
		virtual const IDataProvider * GetSibling() const = 0;

		virtual const IDataProvider * GetFirstAttribute() const = 0;
		virtual const IDataProvider * GetFirstChild() const = 0;
	};

	class CORE_API IDataConverter : public IRefer
	{
	public:
		virtual FVariant Convert(const IDataProvider * ValueProvider) const = 0;
	};

	template<typename T>
	class TDataConverter;

	template<>
	class TDataConverter<FVariant> : public TRefer<IDataConverter>
	{
	public:
		FVariant Convert(const IDataProvider * ValueProvider) const;
	};
	
	enum class ETypeFlags
	{
		None = 0,
		Basic = 0x0001,
		System = 0x0002,
		Abstract = 0x0004,

		ReferPointer = 0x00010000,
	};
	template struct TFlag<ETypeFlags>;

	class IValue;
	class IPropertyAccessor;
	class IFormatter;
	class IDataConverter;
	class IDataProvider;
	class FType;

	enum class EPropertyAccessorFlags
	{
		None = 0,
	};
	template struct TFlag<EPropertyAccessorFlags>;

	class IPropertyAccessor : public IRefer
	{
	public:
		virtual ~IPropertyAccessor() = default;
		virtual EPropertyAccessorFlags GetFlags() const = 0;
		virtual FStringView GetPropertyName() const = 0;
		virtual const FType & GetPropertyType() const = 0;

		virtual void SetValue(const FVariant & Instance, const FVariant & Variable) const = 0;
		virtual FVariant GetValue(const FVariant & Instance) const = 0;
	};

	class CORE_API FProperty
	{
	public:
		FProperty() = default;
		FProperty(const FProperty &) = default;
		FProperty & operator =(const FProperty &) = default;
		friend bool operator == (const FProperty &, const FProperty &) = default;
		friend auto operator <=> (const FProperty &, const FProperty &) = default;

		FProperty(NullPtrT) {}
		FProperty(TReferPtr<IPropertyAccessor> PropertyAccessor_);

		template<typename GetterT, typename SetterT>
		FProperty(FStringView Name, GetterT && Getter, SetterT && Setter);

		template<typename MemberObjectT>
		FProperty(FStringView Name, MemberObjectT && MemberObject);

		explicit operator bool() const { return !!PropertyAccessor; }

		FStringView GetName() const { return PropertyAccessor->GetPropertyName(); }
		const FType & GetType() const { return PropertyAccessor->GetPropertyType(); }

		void SetValue(const FVariant & Instance, const FVariant & Variable) const { PropertyAccessor->SetValue(Instance, Variable); }
		FVariant GetValue(const FVariant & Instance) const { return PropertyAccessor->GetValue(Instance); }

		const IPropertyAccessor * PropertyAccessor = nullptr;;

	public:
		static FProperty Empty;
	};

	inline namespace Private
	{
		template<typename MemberT>
		struct ClassMemberPointerHelper
		{
			using InstanceType = RemoveCVT<typename IsMemberPointer<MemberT>::InstanceType>;
			using ValueType = RemoveCVT<typename IsMemberPointer<MemberT>::ValueType>;
		};

		template<>
		struct ClassMemberPointerHelper<NullPtrT>
		{
			using InstanceType = VoidT<NullPtrT>;
			using ValueType = VoidT<NullPtrT>;
		};
	}
	
	class CORE_API IMethodAccessor : public IRefer
	{
	public:
		virtual ~IMethodAccessor() = default;
		virtual FStringView GetMethodName() const = 0;
		virtual const FType & GetDeclaringType() const = 0;
		virtual const FType & GetReturnType() const = 0;

		virtual FVariant Invoke(const FVariant & Instance, TView<FVariant> Args) const = 0;
	};

	class CORE_API FMethod
	{
	public:
		FMethod() = default;
		FMethod(const FMethod &) = default;
		auto operator<=>(const FMethod &) const = default;
		bool operator==(const FMethod &)const = default;
		
		FMethod(TReferPtr<IMethodAccessor> MethodAccessor_);
		template<typename MemberFunctionT>
		FMethod(FStringView Name, MemberFunctionT && MemberFunction);

		FStringView GetName() const { return MethodAccessor->GetMethodName(); }
		const FType & GetDeclaringType() const { return  MethodAccessor->GetDeclaringType(); }
		const FType & GetReturnType() const { return  MethodAccessor->GetReturnType(); }

		FVariant Invoke(const FVariant & Instance, TView<FVariant> Args) const { return  MethodAccessor->Invoke(Instance, Args); }

	private:
		const IMethodAccessor * MethodAccessor = nullptr;
	};
	
	class CORE_API INativeType
	{
	public:
		INativeType(const std::type_info & StdTypeInfoIn) : StdTypeInfo(StdTypeInfoIn) {}
		virtual ~INativeType() = default;
		
		virtual FStringView GetName() const = 0;
		virtual ETypeFlags GetTypeFlags() const = 0;

		virtual uintx_t GetSize() const = 0;
		virtual uintx_t GetAlignment() const = 0;
		
		virtual void Construct(void * Pointer) const = 0;
		virtual void Destruct(void * Pointer) const = 0;

		virtual void SetUnderlyingType(const INativeType * Type) const = 0;
		virtual const INativeType * GetUnderlyingType() const = 0;

		virtual void SetDataConverter(const IDataConverter * DataConverter_) const = 0;
		virtual const IDataConverter * GetDataConverter() const = 0;
		
		virtual void SetParent(const INativeType * Type) const = 0;
		virtual const INativeType * GetParent() const = 0;

		virtual void AppendChild(const INativeType * Type) const = 0;
		virtual const INativeType * GetFirstChild() const = 0;
		
		virtual void SetNextSibling(const INativeType * Type) const = 0;
		virtual const INativeType * GetNextSibling() const = 0;

		virtual FVariant CreateValue() const = 0;

		virtual FStringView Format(const FVariant & Variant, FStringView Formal) const = 0;
		virtual FVariant Parse(FStringView String) const = 0;

		virtual FProperty GetProperty(FStringView Name) const = 0;
		
	public:
		bool IsParentOf(const INativeType * Another) const;
		bool IsChildOf(const INativeType * Another) const;

	public:
		const std::type_info & StdTypeInfo;
	};

	class CORE_API FType
	{
	public:
		FType() = default;
		FType(const INativeType * NativeTypeIn) : NativeType(NativeTypeIn) {}
		FType(const FType &) = default;
		FType(FType &&) noexcept = default;
		FType & operator = (const FType &) noexcept = default;
		virtual ~FType() = default;
		FStringView ToString() const { return GetName(); }
		
		bool operator==(const FType & Another) const noexcept = default;
		auto operator<=>(const FType & Another) const noexcept = default;

		const INativeType * GetNativeType() const { return NativeType; }
		explicit operator bool() const { return !!NativeType; }
		FStringView GetName() const { return NativeType ? NativeType->GetName() : FStringView::Empty; }
		FStringView GetNormalName() const;
		FStringView GetShortName() const;

		ETypeFlags GetTypeFlags() const { return NativeType ? NativeType->GetTypeFlags() : ETypeFlags::None; }
		void Construct(void * Pointer) const { if (NativeType) NativeType->Construct(Pointer); }
		void Destruct(void * Pointer) const { if (NativeType) NativeType->Destruct(Pointer); }

		FType GetUnderlyingType() const { return NativeType ? FType(NativeType->GetUnderlyingType()) : Unkown; }
		FType GetParent() const { return NativeType ? FType(NativeType->GetParent()) : Unkown; }
		uintx_t GetSize() const { return NativeType ? NativeType->GetSize() : 0; }
		uintx_t GetAlignment() const { return NativeType ? NativeType->GetAlignment() : 0; }

	public:
		FStringView Format(const FVariant & Variant, FStringView Formal = FStringView::Empty) const { return NativeType ? NativeType->Format(Variant, Formal) : nullptr; }
		FVariant Parse(FStringView String) const { return NativeType ? NativeType->Parse(String) : FVariant(); }
		
	public:
		const IDataConverter * GetDataConverter() const { return NativeType ? NativeType->GetDataConverter() : nullptr; }
		FVariant CreateValue() const { return NativeType ? NativeType->CreateValue() : nullptr; }
		FVariant CreateValue(const IDataProvider * IDataProvider) const
		{
			const IDataConverter * DataConverter = GetDataConverter();
			if (DataConverter)
				return DataConverter->Convert(IDataProvider);
			return Parse(IDataProvider->GetValue());
		}
		FProperty GetProperty(FStringView Name) const {return NativeType ? NativeType->GetProperty(Name) : FProperty::Empty; }
		
	public:
		FType GetFirstChild() const { return NativeType ? NativeType->GetFirstChild() : nullptr; }
		bool IsParentOf(const FType & Another) const { return NativeType ? NativeType->IsParentOf(Another.GetNativeType()) : false; }
		bool IsChildOf(const FType & Another) const { return NativeType ? NativeType->IsChildOf(Another.GetNativeType()) : false; }
		bool DefaultConvertableTo(const FType & Another) const { if (!NativeType) return false;  return operator==(Another) || IsChildOf(Another); }
		bool DefaultConvertableFrom(const FType & Another) const { if (!NativeType) return false;  return operator==(Another) || IsParentOf(Another); }
		
	public:
		bool IsBasic() const { return GetFlag(GetTypeFlags(), ETypeFlags::Basic); }
		bool IsSystem() const { return GetFlag(GetTypeFlags(), ETypeFlags::System); }
		bool IsAbstract() const { return GetFlag(GetTypeFlags(), ETypeFlags::Abstract); }
		bool IsReferPointer() const { return GetFlag(GetTypeFlags(), ETypeFlags::ReferPointer); }
		
	protected:
		const INativeType * NativeType = nullptr;

	public:
		static FType Unkown;
	};

	template<>
	inline FVariant::FVariant(const FType & Value)
	{
		ValueType = EValueType::Type;
		NativeTypeValue = Value.GetNativeType();
	}

	template<>
	inline void FVariant::Create<FType>()
	{
		ValueType = EValueType::Type;
		NativeTypeValue = nullptr;
	}

	template<>
	inline FVariant & FVariant::operator =<FType>(const FType & Value)
	{
		Reset();
		ValueType = EValueType::Type;
		NativeTypeValue = Value.GetNativeType();
		return *this;
	}

	CORE_API const FType & TypeBy(const Char * Name);
#ifndef XE_ANSI
	inline const FType & TypeBy(const char * Name) { return TypeBy(reinterpret_cast<const Char*>(Name)); }
#endif
	CORE_API const FType & TypeBy(FStringView Name);
	template<typename T>
	const FType & TypeOf()
	{
		return TypeBy(reinterpret_cast<const Char *>(typeid(T).name()));
	}

	inline const FType & FVariant::GetObjectType() const
	{
		switch (ValueType)
		{
		case EValueType::None: return FType::Unkown;
		case EValueType::Char:  return TypeOf<char>();
		case EValueType::WChar:  return TypeOf<wchar_t>();
		case EValueType::Char8:  return TypeOf<char8_t>();
		case EValueType::Char16:  return TypeOf<char16_t>();
		case EValueType::Char32:  return TypeOf<char32_t>();
		case EValueType::Int8:  return TypeOf<int8_t>();
		case EValueType::UInt8:  return TypeOf<uint8_t>();
		case EValueType::Int16:  return TypeOf<int16_t>();
		case EValueType::UInt16:  return TypeOf<uint16_t>();
		case EValueType::Int32:  return TypeOf<int32_t>();
		case EValueType::UInt32:  return TypeOf<uint32_t>();
		case EValueType::Int64:  return TypeOf<int64_t>();
		case EValueType::UInt64:  return TypeOf<uint64_t>();
		case EValueType::Float32:  return TypeOf<float32_t>();
		case EValueType::Float64:  return TypeOf<float64_t>();

		case EValueType::Vec2I:  return TypeOf<Vec2I>();
		case EValueType::Vec3I:  return TypeOf<Vec3I>();
		case EValueType::Vec4I:  return TypeOf<Vec4I>();
		case EValueType::Vec2U:  return TypeOf<Vec2U>();
		case EValueType::Vec3U:  return TypeOf<Vec3U>();
		case EValueType::Vec4U:  return TypeOf<Vec4U>();
		case EValueType::Vec2F:  return TypeOf<Vec2F>();
		case EValueType::Vec3F:  return TypeOf<Vec3F>();
		case EValueType::Vec4F:  return TypeOf<Vec4F>();

		case EValueType::Vector2:  return TypeOf<FVector2>();
		case EValueType::Vector3:  return TypeOf<FVector3>();
		case EValueType::Vector4:  return TypeOf<FVector4>();

		case EValueType::Color:  return TypeOf<FColor32>();
		case EValueType::String: return TypeOf<FString>();
		case EValueType::StringView:return TypeOf<FStringView>();
		case EValueType::Type: return TypeOf<FType>();
		case EValueType::Pointer:return TypeOf<void *>();
		case EValueType::ObjectPointer:
			if (ObjectPointerValue) return ObjectPointerValue->GetObjectType();
			else return TypeOf<NullPtrT>();
		case EValueType::ReferPointer:
			if (ReferPointerValue) return ReferPointerValue->GetObjectType();
			else return TypeOf<NullPtrT>();
		case EValueType::Raw: return FType::Unkown;
		default: return FType::Unkown;
		}
	}

	template<>
	class TFormatter<FType>
	{
	public:
		FStringView ToString(const FType & Value, FStringView Formal = FStringView::Empty) const
		{
			return Value.GetName();
		}

		FType FromString(FStringView String) const
		{
			return TypeBy(String);
		}

		FType FromData(const IDataProvider * DataProvider) const { return FromString(DataProvider->GetValue()); }
	};
}
