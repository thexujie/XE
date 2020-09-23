#pragma once

#include "CoreInc.h"

namespace XE
{
	inline namespace Private
	{
		template<typename MemberT>
		struct PropertyMemberPointerHelper
		{
			using InstanceType = RemoveCVT<typename IsMemberPointer<MemberT>::InstanceType>;
			using ValueType = RemoveCVT<typename IsMemberPointer<MemberT>::ValueType>;
		};

		template<>
		struct PropertyMemberPointerHelper<NullPtrT>
		{
			using InstanceType = VoidT<NullPtrT>;
			using ValueType = VoidT<NullPtrT>;
		};
	}
	
	template<typename GetterT, typename SetterT>
	class TPropertyAccessor : public TRefer<IPropertyAccessor>
	{
	public:
		using InstanceType = ConditionalT<IsNullPointerV<GetterT>, typename PropertyMemberPointerHelper<SetterT>::InstanceType, typename PropertyMemberPointerHelper<GetterT>::InstanceType>;
		using ValueType = ConditionalT<IsNullPointerV<GetterT>, typename PropertyMemberPointerHelper<SetterT>::ValueType, typename PropertyMemberPointerHelper<GetterT>::ValueType>;
		
	public:
		TPropertyAccessor(FString Name_, GetterT Getter_, SetterT Setter_) : Name(Name_), Getter(Getter_), Setter(Setter_)
		{
		}

		EPropertyAccessorFlags GetFlags() const override { return PropertyFlags; }
		FStringView GetPropertyName() const override { return Name; }
		const FType & GetPropertyType() const override { return TypeOf<ValueType>(); }

		void SetValue(const FVariant & Instance, const FVariant & Variable) const override
		{
			if constexpr (IsMemberFunctionPointerV<SetterT>)
			{
				InstanceType * This = Instance.Get<InstanceType *>();
				(This->*Setter)(Variable.Get<ValueType>());
			}
			else if constexpr (IsMemberObjectPointerV<SetterT>)
			{
				InstanceType * This = Instance.Get<InstanceType *>();
				This->*Setter = Variable.Get<ValueType>();
			}
			else {}
		}
		
		FVariant GetValue(const FVariant & Instance) const
		{
			if constexpr (IsMemberFunctionPointerV<GetterT>)
			{
				InstanceType * This = Instance.Get<InstanceType *>();
				return FVariant((This->*Getter)());
			}
			else if constexpr (IsMemberObjectPointerV<GetterT>)
			{
				InstanceType * This = Instance.Get<InstanceType *>();
				return FVariant(This->*Getter);
			}
			else
			{
				throw EError::NotImplemented;
			}
		}

		EPropertyAccessorFlags PropertyFlags = EPropertyAccessorFlags::None;
		FString Name;
		GetterT Getter;
		SetterT Setter;
	};

	template<typename GetterT, typename SetterT>
	FProperty::FProperty(FStringView Name, GetterT && Getter, SetterT && Setter)
		: FProperty(MakeRefer<TPropertyAccessor<GetterT, SetterT>>(Name, Getter, Setter))
	{
		static_assert(IsMemberPointerV<GetterT> || IsNullPointerV<GetterT>);
		static_assert(IsMemberPointerV<SetterT> || IsNullPointerV<SetterT>);
	}

	template<typename MemberObjectT>
	FProperty::FProperty(FStringView Name, MemberObjectT && MemberObject)
		: FProperty(MakeRefer<TPropertyAccessor<MemberObjectT, MemberObjectT>>(Name, MemberObject, MemberObject))
	{
		static_assert(IsMemberObjectPointerV<MemberObjectT> || IsNullPointerV<MemberObjectT>);
	}

	template <typename T>
	class TMemberFunction
	{
		static_assert(FalseTypeV<T>, "TMemberFunction does not accept non-function types as template arguments.");
	};

	template<typename InstanceT, typename ReturnT, typename... ParametersT>
	class TMemberFunction<ReturnT(InstanceT:: *)(ParametersT...)>
	{
	public:
		template<typename CallableT>
		static FVariant Invoke(CallableT && Callable, InstanceT * This, TView<FVariant> Args)
		{
			if (Args.Size < sizeof...(ParametersT))
				throw EError::Args;

			if constexpr (IsVoidV<ReturnT>)
			{
				InvokeImpl(IndexSequenceFor<ParametersT...>{}, Forward<CallableT>(Callable), This, Args);
				return nullptr;
			}
			else
				return InvokeImpl(IndexSequenceFor<ParametersT...>{}, Forward<CallableT>(Callable), This, Args);
		}

		template<size_t ...Indices, typename CallableT>
		static ReturnT InvokeImpl(IntegerSequence<size_t, Indices...>, CallableT && Callable, InstanceT * This, TView<FVariant> Args)
		{
			return XE::Invoke(Callable, This, (Args[Indices].template Get<ParametersT>())...);
		}
	};
	template<typename InstanceT, typename ReturnT, typename... ParametersT>
	class TMemberFunction<ReturnT(InstanceT:: *)(ParametersT...) const> : public TMemberFunction<ReturnT(InstanceT:: *)(ParametersT...)> {};

	template<typename MemberFunctionT>
	class TMethodAccessor : public TRefer<IMethodAccessor>
	{
	public:
		using InstanceType = typename ClassMemberPointerHelper<MemberFunctionT>::InstanceType;
		using ValueType = typename ClassMemberPointerHelper<MemberFunctionT>::ValueType;

	public:
		TMethodAccessor(FStringView Name_, MemberFunctionT && MemberFunction_) : Name(Name_), MemberFunction(Forward<MemberFunctionT>(MemberFunction_))
		{
		}

		FStringView GetMethodName() const override { return Name; }
		const FType & GetDeclaringType() const override { return TypeOf<InstanceType>(); }
		const FType & GetReturnType() const override { return TypeOf<ValueType>(); }

		FVariant Invoke(const FVariant & Instance, TView<FVariant> Args) const override
		{
			InstanceType * This = Instance.Get<InstanceType *>();
			return TMemberFunction<MemberFunctionT>::Invoke(MemberFunction, This, Args);
		}

		FString Name;
		MemberFunctionT MemberFunction;
	};

	template<typename MemberFunctionT>
	FMethod::FMethod(FStringView Name, MemberFunctionT && MemberFunction) : FMethod(MakeRefer<TMethodAccessor<MemberFunctionT>>(Name, Forward<MemberFunctionT>(MemberFunction)))
	{

	}

	template<typename T>
	class TNativeType : public INativeType
	{
	public:
		TNativeType() : INativeType(typeid(T))
		{
			SetFlag(TypeFlags, ETypeFlags::Basic, IsIntegralV<T> || IsFloatingPointV<T>);
			SetFlag(TypeFlags, ETypeFlags::Abstract, IsAbstractV<T>);
			SetFlag(TypeFlags, ETypeFlags::ReferPointer, IsReferPointerV<T>);
		}

		FStringView GetName() const override { return FStringView(reinterpret_cast<const Char *>(typeid(T).name())); }
		ETypeFlags GetTypeFlags() const override { return TypeFlags; }
		void Construct(void * Pointer) const override
		{
			if constexpr (!IsAbstractV<T>)
				new (Pointer) T();
		}
		void Destruct(void * Pointer) const override { static_cast<T *>(Pointer)->~T(); }
		uintx_t GetSize() const override { return sizeof(T); }
		uintx_t GetAlignment() const override { return alignof(T); }

		void SetUnderlyingType(const INativeType * Type) const override { Underlying = Type; }
		const INativeType * GetUnderlyingType() const override { return Underlying; }

		void SetDataConverter(const IDataConverter * DataConverter_) const override { DataConverter = DataConverter_; }
		const IDataConverter * GetDataConverter() const override
		{
			if (DataConverter)
				return DataConverter;
			
			if (Underlying)
			{
				if (const IDataConverter * UnderlyingDataConverter = Underlying->GetDataConverter(); UnderlyingDataConverter)
					return UnderlyingDataConverter;
			}
			
			if (Parent)
				return Parent->GetDataConverter();
			else
				return nullptr;
		}
		
		void SetParent(const INativeType * Type) const override { Parent = Type; }
		const INativeType * GetParent() const override { return Parent; }

		void AppendChild(const INativeType * Another) const override
		{
			if (!Child)
				Child = Another;
			else
			{
				const INativeType * Current = Child;
				while (Current->GetNextSibling())
					Current = Current->GetNextSibling();
				Current->SetNextSibling(Another);
			}
		}
		const INativeType * GetFirstChild() const override { return Child; }

		void SetNextSibling(const INativeType * NextSiblingIn) const override { NextSibling = NextSiblingIn; }
		const INativeType * GetNextSibling() const { return NextSibling; }

		ETypeFlags TypeFlags = ETypeFlags::None;
		
		mutable const INativeType * Underlying = nullptr;
		mutable const INativeType * Parent = nullptr;
		mutable const INativeType * Child = nullptr;
		mutable const INativeType * NextSibling = nullptr;
		mutable const IDataConverter * DataConverter = nullptr;

	public:
		FStringView Format(const FVariant & Variant, FStringView Formal = FStringView::Empty) const override
		{
			if constexpr (IsSameV<T, FVariant>)
				return XE::Format(StrView("{}"), Variant.ValueType);
			else if constexpr (IsAbstractV<T>)
				return FStringView::Empty;
			else if constexpr (IsOutterFormatAbleV<T>)
				return TFormatter<T>().Format(Variant.Get<T>(), Formal);
			else
				throw EError::NotImplemented;
		}
		
		FVariant Parse(FStringView String) const override
		{
			if constexpr (IsSameV<T, FVariant>)
				return FVariant(String);
			else if constexpr (IsAbstractV<T>)
				return FVariant();
			else if constexpr (IsOutterFormatAbleV<T>)
				return FVariant(TFormatter<T>().Parse(String));
			else
				throw EError::NotImplemented;
		}
		
	public:
		FVariant CreateValue() const override
		{
			FVariant Variable;
			Variable.Create<T>();
			return Variable;
		}
		
		FProperty GetProperty(FStringView Name) const override
		{
			if (auto PropertyIter = Properties.Find(Name); PropertyIter != Properties.End())
				return PropertyIter->Value;
			return Parent ? Parent->GetProperty(Name) : nullptr;
		}

		void RegisterProperty(FProperty Property)
		{
			Properties[Property.GetName()] = Property;
		}
		
	private:
		TMap<FString, FProperty> Properties;
	};

	template<typename T>
	void Reflection(TNativeType<T> & NativeType)
	{
		
	}
}