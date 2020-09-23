#pragma once

#include "Config.h"

namespace XE
{
	using Char = char;
#define Str(xxx) xxx
#define StrView(xxx) TView(xxx)
#define XE_ANSI 1

//	using Char = char8_t;
//#define Str(xxx) u8## xxx
//#define StrView(xxx) TView(u8##xxx)
//#define XE_UTF8 1
	
	using long_t = long;
	using ulong_t = unsigned long;
	using float32_t = float;
	using float64_t = double;
	using ptr_t = void *;
	
#if XE_X64
	using intx_t = int64_t;
	using uintx_t = uint64_t;
	using intx_h = int32_t;
	using uintx_h = uint32_t;
	using addr_t = uint64_t;
#else
	using intx_t = int32_t;
	using uintx_t = uint32_t;
	using intx_h = int16_t;
	using uintx_h = uint16_t;
	using addr_t = uint32_t;
#endif
	using byte_t = uint8_t;
	using diff_t = intx_t;
	
	constexpr addr_t NullAddress = addr_t(0);
	constexpr size_t NullPos = size_t(-1);
	constexpr size_t NullIndex = size_t(-1);
	constexpr uint16_t NullIndexU16 = uint16_t(-1);
	constexpr uint32_t NullIndexU32 = uint32_t(-1);
	constexpr uint64_t NullIndexU64 = uint64_t(-1);
	constexpr uint32_t InfinityU32 = 0x7f800000;
	const float32_t NanF = std::nanf("0");
	const float64_t NanF64 = std::nan("0");

	const int16_t Int16Min = std::numeric_limits<int16_t>::min();
	const int16_t Int16Max = std::numeric_limits<int16_t>::max();
	const uint16_t UInt16Min = std::numeric_limits<uint16_t>::min();
	const uint16_t UInt16Max = std::numeric_limits<uint16_t>::max();
	const int32_t Int32Min = std::numeric_limits<int32_t>::min();
	const int32_t Int32Max = std::numeric_limits<int32_t>::max();
	const uint32_t UInt32Min = std::numeric_limits<uint32_t>::min();
	const uint32_t UInt32Max = std::numeric_limits<uint32_t>::max();
	const int64_t Int64Min = std::numeric_limits<int64_t>::min();
	const int64_t Int64Max = std::numeric_limits<int64_t>::max();
	const uint64_t UInt64Min = std::numeric_limits<uint64_t>::min();
	const uint64_t UInt64Max = std::numeric_limits<uint64_t>::max();

	constexpr uint16_t InvalidIndexU16 = uint16_t(-1);
	constexpr uint32_t InvalidIndexU32 = uint32_t(-1);
	constexpr uint64_t InvalidIndexU64 = uint64_t(-1);
	
#if XE_X64
	const intx_t IntXMin = Int64Min;
	const intx_t IntXMax = Int64Max;
	const uintx_t UIntXMin = UInt64Min;
	const uintx_t UIntXMax = UInt64Max;
#else
	const uintx_t IntXMin = Int32Min;
	const uintx_t IntXMax = Int32Max;
	const uintx_t UIntXMin = UInt32Min;
	const uintx_t UIntXMax = UInt32Max;
#endif

	const float32_t Float32Min = 1.175494351e-38f;
	const float32_t Float32Max = 3.402823466e+38f;
	const float32_t Float32Epslion = 1.192092896e-07f;
	const float32_t Float32PI = 3.141592654f;

	const float64_t Float64Min = 2.2250738585072014e-308;
	const float64_t Float64Max = 1.7976931348623158e+308;
	const float64_t Float64Epslion = 2.2204460492503131e-016;
	const float64_t Float64PI = 3.14159265358979323846;

	template<typename T> const T FloatEpslion = 0;
	template<> const float32_t FloatEpslion<float32_t> = Float32Epslion;
	template<> const float64_t FloatEpslion<float64_t> = Float64Epslion;

	template <typename T>
	constexpr T * AddressOf(T & Value) noexcept { return __builtin_addressof(Value); }

	template <typename T>
	const T * AddressOf(const T &&) = delete;

	template <typename T>
	struct TDummy { };
	
	struct FStrongOrdering
	{
		static FStrongOrdering Less;
		static FStrongOrdering Greater;
		static FStrongOrdering Equal;
	};

	// ---------------- NullPtrT
	using NullPtrT = decltype(__nullptr);

	template<typename T>
	using NullPointerT = decltype(__nullptr);

	// ---------------- VoidT
	template <typename... Types>
	using VoidT = void;
	
	// ---------------- RemoveReference
	template <typename T>
	struct RemoveReference
	{
		using Type = T;
	};

	template <typename T>
	struct RemoveReference<T &>
	{
		using Type = T;
	};

	template <typename T>
	struct RemoveReference<T &&>
	{
		using Type = T;
	};

	template <class T>
	using RemoveReferenceT = typename RemoveReference<T>::Type;

	// ---------------- RemoveCV
	template <typename T> struct RemoveCV { typedef T Type; };
	template <typename T> struct RemoveCV<const T> { typedef T Type; };
	template <typename T> struct RemoveCV<volatile T> { typedef T Type; };
	template <typename T> struct RemoveCV<const volatile T> { typedef T Type; };

	template <typename T>
	using RemoveCVT = typename RemoveCV<T>::Type;

	template <typename T>
	using RemoveCVRefT = RemoveCVT<RemoveReferenceT<T>>;


	// ---------------- RemoveExtent
	template <typename T> struct RemoveExtent { typedef T Type; };
	template <typename T> struct RemoveExtent<T[]> { typedef T Type; };
	template <typename T, size_t N> struct RemoveExtent<T[N]> { typedef T Type; };
	template <typename T>
	using RemoveExtentT = typename RemoveExtent<T>::Type;

	// ---------------- Add CVRef
	template <typename... TypesT>
	void Swallow(TypesT &&...) {}

	template <typename T>
	struct AddConst { using Type = const T; };

	template <typename T>
	using AddConstT = typename AddConst<T>::Type;

	template <typename T>
	struct AddVolatile { using Type = volatile T; };

	template <typename T>
	using AddVolatileT = typename AddVolatile<T>::Type;

	template <typename T>
	struct AddCV { using Type = const volatile T; };

	template <typename T>
	using AddCVT = typename AddCV<T>::Type;

	// ---------------- AddPointer
	template <typename T, typename = void>
	struct AddPointer
	{
		using Type = T;
	};

	template <typename T>
	struct AddPointer<T, VoidT<RemoveReferenceT<T> *>>
	{
		using Type = RemoveReferenceT<T> *;
	};

	template <typename T>
	using AddPointerT = typename AddPointer<T>::Type;

	template <typename T>
	struct RemovePointer { using Type = T; };

	template <typename T>
	struct RemovePointer<T *> { using Type = T; };

	template <typename T>
	struct RemovePointer<T * const> { using Type = T; };

	template <typename T>
	struct RemovePointer<T * volatile> { using Type = T; };

	template <typename T>
	struct RemovePointer<T * const volatile> { using Type = T; };

	template <typename T>
	using RemovePointerT = typename RemovePointer<T>::Type;
	
	// ---------------- EnableIf
	template <bool Test, typename T = void>
	struct EnableIf {};

	template <typename T>
	struct EnableIf<true, T>
	{
		using Type = T;
	};

	template <bool Test, typename T = void>
	using EnableIfT = typename EnableIf<Test, T>::Type;
	

	//----------------- Add
	template<typename T>
	struct AddLValueReference { using Type = T &; };

	template<>
	struct AddLValueReference<void> { using Type = void; };

	template<typename T>
	using AddLValueReferenceT = typename AddLValueReference<T>::Type;

	template<typename T>
	struct AddRValueReference { using Type = T &&; };

	template<>
	struct AddRValueReference<void> { using Type = void; };

	template<typename T>
	using AddRValueReferenceT = typename AddRValueReference<T>::Type;

	template<bool Cond, typename TrueT, typename FalseT>
	struct Conditional { typedef TrueT Type; };

	template<typename TrueT, typename FalseT>
	struct Conditional<false, TrueT, FalseT> { typedef FalseT Type; };

	template<bool Cond, typename TrueT, typename FalseT>
	using ConditionalT = typename Conditional<Cond, TrueT, FalseT>::Type;

	template<typename T>
	AddRValueReferenceT<T> DeclVal() noexcept;
	
	// ---------------- IntegralConstant
	template <typename T, T ConstantValue>
	struct IntegralConstant
	{
		using ValueType = T;
		using Type = IntegralConstant;

		static constexpr T Value = ConstantValue;

		constexpr operator ValueType() const noexcept { return Value; }
		constexpr ValueType operator()() const noexcept { return Value; }
	};

	// ---------------- BoolConstant
	template <bool ConstantValue>
	using BoolConstant = IntegralConstant<bool, ConstantValue>;

	using TrueType = BoolConstant<true>;
	using FalseType = BoolConstant<false>;

	template <typename T>
	constexpr bool TrueTypeV = BoolConstant<true>::Value;

	template <typename T>
	constexpr bool FalseTypeV = BoolConstant<false>::Value;

	// ---------------- Is Array
	template <typename>
	inline constexpr bool IsArrayV = false;

	template <typename T, size_t Length>
	inline constexpr bool IsArrayV<T[Length]> = true;

	template <typename T>
	inline constexpr bool IsArrayV<T[]> = true;

	template <typename T>
	struct IsArray : BoolConstant<IsArrayV<T>>
	{
		using Type = T;
	};
	
	// ---------------- Is Reference
	template <typename>
	inline constexpr bool IsReferenceV = false;

	template <typename T>
	inline constexpr bool IsReferenceV<T &> = true;

	template <typename T>
	inline constexpr bool IsReferenceV<T &&> = true;

	template <typename T>
	struct IsReference : BoolConstant<IsReferenceV<T>> {};
	
	// ---------------- IsSame
	template <typename, typename>
	inline constexpr bool IsSameV = false;
	template <typename T>
	inline constexpr bool IsSameV<T, T> = true;

	template <typename T1, typename T2>
	struct IsSame : BoolConstant<IsSameV<T1, T2>> {};
	
	template<typename BaseT, typename DerivedT>
	inline constexpr bool IsBaseOfV = __is_base_of(BaseT, DerivedT);

	template<typename BaseT, typename DerivedT>
	using IsBaseOf = BoolConstant<IsBaseOfV<BaseT, DerivedT>>;

	template <typename T>
	inline constexpr bool IsVoidV = IsSameV<RemoveCVT<T>, void>;

	template <typename T>
	struct IsVoid : BoolConstant<IsVoidV<T>> {};
	
	// ---------------- IsEmpty
	template <typename T>
	inline constexpr bool IsEmptyV = __is_empty(T);

	template <typename T>
	struct IsEmpty : BoolConstant<IsEmptyV<T>> {};
	
	// ---------------- Types
	template<typename T> constexpr bool IsUnionV = __is_union(T);
	template<typename T> constexpr bool IsEnumV = __is_enum(T);
	template<typename T> constexpr bool IsClassV = __is_class(T);
	template<typename T> constexpr bool IsEnumClassV = __is_class(T) && __is_enum(T);

	template <typename T>
	inline constexpr bool IsAbstractV = __is_abstract(T);
	template <typename T>
	struct IsAbstract : BoolConstant<IsAbstractV<T>> {};
	template <typename T>
	concept Abstract = IsAbstractV<T>;
	
	template <typename T>
	inline constexpr bool IsFinalV = __is_final(T);
	template <typename T>
	struct IsFinal : BoolConstant<IsFinalV<T>> {};

	template<typename T, bool = IsEnumV<T>>
	struct UnderlyingType
	{
		using Type = __underlying_type(T);
	};
	template<typename T>
	using UnderlyingTypeT = typename UnderlyingType<T>::Type;

	template <typename FromT, typename ToT>
	inline constexpr bool IsConvertibleToV = __is_convertible_to(FromT, ToT);
	
	template <typename FromT, typename ToT>
	concept ConvertibleTo = __is_convertible_to(FromT, ToT) && requires(AddRValueReferenceT<FromT>(&ConvertFun)())
	{
		static_cast<FromT>(ConvertFun());
	};
	
	template<typename T>
	concept EqualAble = requires(const T & Lhs, const T & Rhs) { {Lhs == Rhs}->ConvertibleTo<bool>; };
	

	// ---------------- Disjunction
	template<typename...> struct Disjunction : FalseType { };
	template<typename T> struct Disjunction<T> : T { };
	template<typename T, typename... ArgsT>
	struct Disjunction<T, ArgsT...> : ConditionalT<bool(T::Value), T, Disjunction<ArgsT...>> { };

	template<typename ...ArgsT>
	inline constexpr bool DisjunctionV = Disjunction<ArgsT...>::Value;


	// ---------------- IsAnyOfV
	template<typename T, typename... ArgsT>
	inline constexpr bool IsAnyOfV = DisjunctionV<IsSame<T, ArgsT>...>;

	// ---------------- IsTrivially

	template <typename T>
	inline constexpr bool IsDefaultConstructibleV = __is_constructible(T);
	template <typename T>
	struct IsDefaultConstructible : BoolConstant<IsDefaultConstructibleV<T>> {};
	template <typename T>
	concept DefaultConstructible = IsDefaultConstructibleV<T>;

	template <typename T>
	inline constexpr bool IsMoveConstructibleV = __is_constructible(T, T);
	template <typename T>
	concept MoveConstructible = IsMoveConstructibleV<T>;

	template <typename T, typename... ArgsT>
	inline constexpr bool IsConstructibleV = __is_constructible(T, ArgsT...);

	template <typename T>
	inline constexpr bool IsCopyConstructibleV = __is_constructible(T, AddLValueReferenceT<const T>);
	template <typename T>
	struct IsCopyConstructible : BoolConstant<IsCopyConstructibleV<T>> {};
	template <typename T>
	concept CopyConstructible = IsCopyConstructibleV<T>;

	template <typename T>
	inline constexpr bool IsMoveAssignableV = __is_assignable(AddLValueReferenceT<T>, T);
	template <typename T>
	concept MoveAssignable = IsMoveAssignableV<T>;


	template <typename ToT, typename FromT>
	inline constexpr bool IsNoThrowAssignableV = __is_nothrow_assignable(ToT, FromT);

	template <typename ToT, typename FromT>
	struct IsNoThrowAssignable : BoolConstant<IsNoThrowAssignableV<ToT, FromT>> {};

	template <typename T>
	inline constexpr bool IsNoThrowMoveConstructibleV = __is_nothrow_constructible(T, T);

	template <typename T>
	inline constexpr bool IsNoThrowMoveAssignableV = __is_nothrow_assignable(AddLValueReferenceT<T>, T);
	
	// ---------------- IsTrivially
	template <typename T>
	inline constexpr bool IsTriviallyConstructibleV = __is_trivially_constructible(T);

	template <typename T>
	inline constexpr bool IsTriviallyCopyableV = __is_trivially_copyable(T);

	template<typename T>
	concept TriviallyCopyable = IsTriviallyCopyableV<T>;

	template <typename T>
	inline constexpr bool IsTriviallyV = __is_trivially_constructible(T) && __is_trivially_copyable(T);

	template <typename _Ty>
	inline constexpr bool IsTriviallyMoveConstructibleV = __is_trivially_constructible(_Ty, _Ty);

	// ---------------- Negation
	template <typename Trait>
	struct Negation : BoolConstant<!static_cast<bool>(Trait::Value)> {};

	template <typename Trait>
	inline constexpr bool NegationV = Negation<Trait>::Value;


	
	// ---------------- IsPointer
	template <typename>
	inline constexpr bool IsPointerV = false;

	template <typename T>
	inline constexpr bool IsPointerV<T *> = true;

	template <typename T>
	inline constexpr bool IsPointerV<T * const> = true;

	template <typename T>
	inline constexpr bool IsPointerV<T * volatile> = true;

	template <typename T>
	inline constexpr bool IsPointerV<T * const volatile> = true;

	template <typename T>
	struct IsPointer : BoolConstant<IsPointerV<T>> {};
	
	// ---------------- IsNullPointer
	template <typename T>
	inline constexpr bool IsNullPointerV = IsSameV<RemoveCVT<T>, NullPtrT>;
	
	template <typename T>
	using IsNullPointer = BoolConstant<IsNullPointerV<T>>;
	// 
	// ---------------- IsIntegral
	template <typename T>
	inline constexpr bool IsCharacterV = IsAnyOfV<RemoveCVT<T>, char, wchar_t, char8_t, char16_t, char32_t>;

	template<typename T>
	concept Character = IsCharacterV<T>;
	
	template <typename T>
	inline constexpr bool IsIntegralV = IsAnyOfV<RemoveCVT<T>, bool, char, signed char, unsigned char,
		wchar_t,
#ifdef __cpp_char8_t
		char8_t,
#endif // __cpp_char8_t
		char16_t, char32_t, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

	template <typename T>
	inline constexpr bool IsFloatingPointV = IsAnyOfV<RemoveCVT<T>, float, double, long double>;

	template <typename T>
	inline constexpr bool IsArithmeticV = IsIntegralV<T> || IsFloatingPointV<T>;

	template <typename T>
	inline constexpr bool IsNoneBoolIntegralV = IsIntegralV<T> && !IsSameV<T, bool>;
	
	// ---------------- IsSigned
	inline namespace Private
	{
		template<typename T, bool = IsIntegralV<T>>
		struct IsSignedPrivate
		{
			using SingedType = BoolConstant<T(-1) < T(0)> ;
			using UnSingedType = BoolConstant<!(T(-1) < T(0))>;
		};
		
		template<typename T>
		struct IsSignedPrivate<T, false>
		{
			using SingedType = BoolConstant<IsFloatingPointV<T>>;
			using UnSingedType = BoolConstant<false>;
		};

		template<>
		struct IsSignedPrivate<bool, true>
		{
			using SingedType = BoolConstant<false>;
			using UnSingedType = BoolConstant<true>;
		};
	}

	template<typename T>
	struct IsSigned : IsSignedPrivate<T>::SingedType {};

	template<typename T>
	inline constexpr bool IsSignedV = IsSigned<T>::Value;

	template<typename T>
	concept Signed = IsSignedV<T>;
	
	template<typename T>
	concept UnSigned = IsSignedV<T>;
	
	inline namespace Private
	{
		template<size_t Size>
		struct MakeSignedPrivate;

		template<>
		struct MakeSignedPrivate<1>
		{
			using SingedType = int8_t;
			using UnSingedType = uint8_t;
		};

		template<>
		struct MakeSignedPrivate<2>
		{
			using SingedType = int16_t;
			using UnSingedType = uint16_t;
		};


		template<>
		struct MakeSignedPrivate<4>
		{
			using SingedType = int32_t;
			using UnSingedType = uint32_t;
		};


		template<>
		struct MakeSignedPrivate<8>
		{
			using SingedType = int64_t;
			using UnSingedType = uint64_t;
		};
	}

	template<typename T, typename = EnableIfT<IsIntegralV<T> && !IsSameV<T, bool>>>
	struct MakeSigned { using Type = typename MakeSignedPrivate<sizeof(T)>::SingedType; };

	template<typename T, typename = EnableIfT<IsIntegralV<T> && !IsSameV<T, bool>>>
	struct MakeUnSigned { using Type = typename MakeSignedPrivate<sizeof(T)>::UnSingedType; };

	template<typename T, typename = EnableIfT<IsIntegralV<T> && !IsSameV<T, bool>>>
	using MakeSignedT = typename MakeSigned<T>::Type;

	template<typename T, typename = EnableIfT<IsIntegralV<T> && !IsSameV<T, bool>>>
	using MakeUnSignedT = typename MakeUnSigned<T>::Type;

	template<typename T>
	concept OppositeAble = requires (T Value) { -Value; };
	
	template<typename T>
	inline constexpr bool IsOppositeAbleV = false;
	template<OppositeAble T>
	inline constexpr bool IsOppositeAbleV<T> = true;
	
	template<typename T>
	struct IsOppositeAble {using Type = decltype(-(DeclVal<T>())); };

	template<OppositeAble T>
	using IsOppositeAbleT = typename IsOppositeAble<T>::Type;
	
	// ---------------- IntegerSequence
	template <class T, T... Values>
	struct IntegerSequence
	{
		static_assert(IsIntegralV<T>, "IntegerSequence<T, I...> requires T to be an integral type.");
		using ValueType = T;

		static constexpr size_t Size() noexcept { return sizeof...(Values); }
	};

	template <class T, T Size>
	using MakeIntegerSequence = __make_integer_seq<IntegerSequence, T, Size>;

	template <size_t... Values>
	using IndexSequence = IntegerSequence<size_t, Values...>;

	template <size_t Size>
	using MakeIndexSequence = MakeIntegerSequence<size_t, Size>;

	template<typename ...TypesT>
	using IndexSequenceFor = MakeIndexSequence<sizeof...(TypesT)>;

	// ---------------- IsFunction
	template <typename>
	struct IsFunction : public FalseType {};

	template <typename ReturnType, typename... ArgsType>
	struct IsFunction<ReturnType(ArgsType...)> : public TrueType {};

	template <typename ReturnType, typename... ArgsType>
	struct IsFunction<ReturnType(ArgsType..., ...)> : public TrueType {};

	template <class T>
	constexpr bool IsFunctionV = IsFunction<T>::Value;

	// ---------------- is_signed
	//template<typename T, typename = void>
	//struct IsSigned : FalseType { };

	//template<typename T, typename = EnableIfT<IsIntegralV<T>>>
	//struct IsSigned : FalseType { };

	// ---------------- IsMemberObjectPointer
	template<typename T>
	struct IsMemberFunctionPointer :FalseType { };

	template<typename ReturnT, typename InstanceT, typename ...ArgsT>
	struct IsMemberFunctionPointer<ReturnT(InstanceT:: *)(ArgsT...)> :TrueType
	{
		using InstanceType = InstanceT;
		using ValueType = ReturnT;
	};

	template<typename ReturnT, typename InstanceT, typename ...ArgsT>
	struct IsMemberFunctionPointer<ReturnT(InstanceT:: *)(ArgsT...) const > :TrueType
	{
		using InstanceType = InstanceT;
		using ValueType = ReturnT;
	};

	template <typename T>
	inline constexpr bool IsMemberFunctionPointerV = IsMemberFunctionPointer<RemoveCVT<T>>::Value;

	template <typename T, bool = IsMemberFunctionPointerV<T>>
	struct IsMemberObjectPointer : FalseType {};

	template <typename ObjectT, typename InstanceT>
	struct IsMemberObjectPointer<ObjectT InstanceT:: *, false> : TrueType
	{
		using InstanceType = InstanceT;
		using ValueType = ObjectT;
	};

	template <typename T>
	inline constexpr bool IsMemberObjectPointerV = IsMemberObjectPointer<RemoveCVT<T>>::Value;

	// STRUCT TEMPLATE is_member_pointer
	template <typename T>
	inline constexpr bool IsMemberPointerV = IsMemberFunctionPointerV<T> || IsMemberObjectPointerV <T>;

	template <typename T>
	using IsMemberPointer = ConditionalT<IsMemberFunctionPointerV<T>, IsMemberFunctionPointer<T>, IsMemberObjectPointer<T>>;
	

	// ---------------- Decay
	template <typename T>
	struct Decay
	{
		using NoneReferenceType = RemoveReferenceT<T>;
		using TypeInner = ConditionalT<IsFunctionV<NoneReferenceType>, AddPointerT<NoneReferenceType>, RemoveCVT<NoneReferenceType>>;
		using Type = ConditionalT<IsArrayV<NoneReferenceType>, AddPointerT<RemoveExtentT<NoneReferenceType>>, TypeInner>;
	};
	template <typename T>
	using DecayT = typename Decay<T>::Type;

	// STRUCT TEMPLATE is_lvalue_reference
	template <typename>
	constexpr bool IsLValueReferenceV = false;

	template <typename T>
	constexpr bool IsLValueReferenceV<T &> = true;

	template <typename T>
	constexpr T && Forward(RemoveReferenceT<T> & Arg) noexcept
	{
		return static_cast<T &&>(Arg);
	}

	template <typename T>
	constexpr T && Forward(RemoveReferenceT<T> && Arg) noexcept
	{
		static_assert(!IsLValueReferenceV<T>, "bad forward call");
		return static_cast<T &&>(Arg);
	}

	// ---------------- Conjunction
	template <bool FirstValue, typename FirstType, typename... RestTypes>
	struct ConjunctionHelper
	{
		using Type = FirstType;
	};

	template <class TrueT, typename NextT, typename... RestsT>
	struct ConjunctionHelper<true, TrueT, NextT, RestsT...>
	{
		using Type = typename ConjunctionHelper<NextT::Value, NextT, RestsT...>::Type;
	};

	template <typename... Traits>
	struct Conjunction : TrueType {};

	template <typename FirstType, typename... RestTypes>
	struct Conjunction<FirstType, RestTypes...> : ConjunctionHelper<FirstType::Value, FirstType, RestTypes...>::Type
	{
	};

	template <typename... Traits>
	constexpr bool ConjunctionV = Conjunction<Traits...>::Value;


	// ---------------- IsComplete
	inline namespace Private
	{
		template <typename T, std::size_t = sizeof(T)>
		TrueType IsCompletePrivate(T *);

		FalseType IsCompletePrivate(...);
	}

	template<typename T>
	using IsComplete = decltype(IsCompletePrivate(std::declval<T *>()));

	template<typename T>
	constexpr bool IsCompleteV = IsComplete<T>::Value;

	template<typename T>
	concept Integral = IsIntegralV<T>;

	template<typename T>
	concept NoneBoolIntegral = IsNoneBoolIntegralV<T>;

	template<typename T>
	concept FloatingPoint = IsFloatingPointV<T>;

	template<typename T>
	concept Arithmetic = IsArithmeticV<T>;

	template<typename T>
	concept Enum = IsEnumV<T>;

	template<typename T>
	concept Class = IsClassV<T>;

	template<typename T>
	concept EnumClass = IsEnumClassV<T>;

	template<typename ContainerT>
	concept IterAble = requires (ContainerT Container)
	{
		Container.Begin();
		Container.End();
	};


	template <typename OutT, typename InT>
	FORCEINLINE OutT * AddressAs(InT & Source) noexcept
	{
		static_assert(sizeof(InT) == sizeof(OutT), "Input and output must have same size");
		static_assert(IsIntegralV<OutT>, "Tried to reinterpret memory as non-integral");
		return &reinterpret_cast<OutT &>(Source);
	}

	template <class OutT, typename InT>
	FORCEINLINE OutT ValueAs(const InT & Source) noexcept
	{
		static_assert(sizeof(InT) == sizeof(OutT), "Input and output must have same size");
		static_assert(IsIntegralV<OutT>, "Tried to reinterpret memory as non-integral");
		return *reinterpret_cast<const OutT *>(&Source);
	}

	template<typename CharT>
	constexpr size_t Length(const CharT * String)
	{
		if (!String)
			return 0;

		size_t Result = 0;
		while (*String++)
			++Result;
		return Result;
	}

	template<typename CharT, typename IntergerT>
	IntergerT ToInterger(const CharT * String, size_t Size, int32_t Radis = 0)
	{
		if (Size == UIntXMax)
			Size = Length(String);

		while (Size && *String == ' ')
		{
			++String;
			--Size;
		}

		IntergerT sign = 1;
		if (String[0] == L'-')
		{
			sign = (IntergerT)-1;
			++String;
		}

		if (!Size || !String)
			return 0;

		const CharT * Curr = String;
		if (Radis <= 0)
		{
			if (Curr[0] == '0' && Size >= 2)
			{
				if (Radis > 1)
				{
					Curr += 1;
				}
				else if (Curr[1] == 'b' || Curr[1] == 'B' || Curr[1] == '0')
				{
					Radis = 2;
					Curr += 2;
				}
				else if (Curr[1] == 'x' || Curr[1] == 'X')
				{
					Radis = 16;
					Curr += 2;
				}
				else if (Curr[1] == 'h' || Curr[1] == 'H')
				{
					Radis = 8;
					Curr += 2;
				}
				else if (Curr[1] == 'd' || Curr[1] == 'D')
				{
					Radis = 10;
					Curr += 2;
				}
				else
					Radis = 10;
			}
			else
				Radis = 10;
		}

		IntergerT integer = 0;
		IntergerT number = 0;
		CharT ch = 0;

		while (Size--)
		{
			ch = *Curr++;
			if (!ch)
				break;

			if (ch >= '0' && ch <= '9')
				number = ch - '0';
			else if (ch >= 'A' && ch <= 'Z')
				number = ch - 'A' + 10;
			else if (ch >= 'a' && ch <= 'z')
				number = ch - 'a' + 10;
			else
				break;

			if (number >= (IntergerT)Radis)
				break;

			integer = integer * (IntergerT)Radis + number;
		}
		return integer * sign;
	}

	template<typename CharT>
	bool TStringToBoolean(const CharT * String, size_t Length)
	{
		return false;
	}

	template<typename CharT, typename FloatingPointT>
	FloatingPointT ToFloatingPoint(const CharT * String, size_t Size = -1)
	{
		if (Size == UIntXMax)
			Size = Length(String);

		if (!Size)
			return FloatingPointT(0);

		const CharT * end = String + Size;
		bool sign_radix = true;
		bool sign_exp = true;
		// reading integer part.
		bool read_integer = true;
		// reading exp part.
		bool read_exp = false;

		// radix = integer + decimal.
		// integer part.
		FloatingPointT integer = 0;
		// decimal part.
		FloatingPointT decimal = 0;
		// exp part.
		int32_t exp = 0;
		// use to caculate decimal.
		FloatingPointT decimal_index = FloatingPointT(1) / FloatingPointT(10);

		if (String[0] == '-')
		{
			sign_radix = false;
			++String;
		}
		else {}

		while (String < end)
		{
			CharT ch = *String++;
			if (ch == '-')
			{
				if (read_exp)
					sign_exp = false;
				else
					break;
			}
			else if (ch == '.')
			{
				read_integer = false;
			}
			else if (ch == 'E' || ch == 'e')
			{
				read_integer = false;
				read_exp = true;
			}
			else if ('0' <= ch && ch <= '9')
			{
				int32_t number = (int32_t)(ch - '0');
				if (read_exp)
				{
					exp *= 10;
					exp += number;
				}
				else if (read_integer)
				{
					integer *= 10;
					integer += number;
				}
				else
				{
					decimal += number * decimal_index;
					decimal_index /= FloatingPointT(10);
				}
			}
			else
				break;
		}

		FloatingPointT result = (FloatingPointT)(integer + decimal);
		if (!sign_radix)
			result = -result;

		if (sign_exp)
		{
			for (int32_t cnt = 0; cnt < exp; ++cnt)
				result *= 10;
		}
		else
		{
			for (int32_t cnt = 0; cnt < exp; ++cnt)
				result /= FloatingPointT(10);
		}

		return result;
	}

	enum class EError : int
	{
		Generic = std::numeric_limits<int>::min(),
		Inner,
		Eof,
		Failed,
		Nullptr,
		Unreachable,
		OutOfMemory,
		OutOfBound,
		Args,
		Exists,
		Access,
		State,
		IO,
		Broken,

		Timeout,
		Cancel,

		BadData,
		BadFormat,

		NotNow,
		NotSupported,
		NotImplemented,
		NotFound,

		InvalidOperation,

		OK = 0,
		True,
		False,
		Pendding,
	};

	CORE_API const Char * ErrorStr(EError err);

	CORE_API bool IsNan(float32_t Value);
	CORE_API bool IsNan(float64_t Value);

	template<typename T>
	concept IteraterAble = requires(T Instance)
	{
		Instance.Begin();
		Instance.End();
	};
	
	template<typename T, size_t Length>
	constexpr T * Begin(T(&Array)[Length]) noexcept { return Array; }

	template<typename T, size_t Length>
	constexpr T * End(T(&Array)[Length]) noexcept { return Array + Length; }
	
	template <typename IterT, typename T>
	IterT Find(IterT First, const IterT Last, const T & Value)
	{
		while (First != Last)
		{
			if (*First == Value)
				return First;
			++First;
		}
		return Last;
	}

	template <IteraterAble ContainerT, typename T>
	auto Find(const ContainerT & Container, const T & Value)
	{
		for (auto Iter = Container.Begin(); Iter != Container.End(); ++Iter)
		{
			if (*Iter == Value)
				return Iter;
		}
		return Container.End();
	}

	template <typename IterT, typename T>
	bool Contains(IterT First, const IterT Last, const T & Value)
	{
		while (First != Last)
		{
			if (*First == Value)
				return true;
			++First;
		}
		return false;
	}

	template <IteraterAble ContainerT, typename T>
	bool Contains(const ContainerT & Container, const T & Value)
	{
		for (auto Iter = Container.Begin(); Iter != Container.End(); ++Iter)
		{
			if (*Iter == Value)
				return true;
		}
		return false;
	}
	
	template<typename T>
	struct TDeleterDefault
	{
		TDeleterDefault() = default;
		TDeleterDefault(const TDeleterDefault &) = default;
		TDeleterDefault & operator=(const TDeleterDefault &) = default;
		~TDeleterDefault() = default;

		FORCEINLINE void operator()(T * Ptr)
		{
			delete Ptr;
		}
	};
	template<typename T>
	struct TDeleterDefault<T[]>
	{
		TDeleterDefault() = default;
		TDeleterDefault(const TDeleterDefault &) = default;
		TDeleterDefault & operator=(const TDeleterDefault &) = default;
		~TDeleterDefault() = default;

		FORCEINLINE void operator()(T * Ptr)
		{
			delete[] Ptr;
		}
	};

	template <typename T>
	struct TEqual
	{
		constexpr bool operator()(const T & Left, const T & Right) const { return Left == Right; }
	};

	template <typename T>
	struct TLess
	{
		constexpr bool operator()(const T & Left, const T & Right) const { return Left < Right; }
	};

	template<typename T>
	T Abs(const T & Value)
	{
		return Value < 0 ? -Value : Value;
	}
	
	namespace Array
	{
		template<typename CharT>
		size_t FindFirst(const CharT * String, size_t Length, const CharT & Ch)
		{
			for (size_t Index = 0; Index < Length; ++Index)
			{
				if (Ch == String[Index])
					return Index;
			}
			return NullIndex;
		}

		template<typename CharT>
		size_t FindFirst(const CharT * String, size_t Length, const CharT * Pattern, size_t PatternLength)
		{
			if (PatternLength > Length)
				return NullIndex;

			for (size_t Index = 0; Index < Length - PatternLength; ++Index)
			{
				bool Match = true;
				for (size_t PatternIndex = 0; PatternIndex < PatternLength; ++PatternIndex)
				{
					if (String[Index + PatternIndex] != Pattern[PatternIndex])
					{
						Match = false;
						break;
					}
				}

				if (Match)
					return Index;
			}

			return NullIndex;
		}

		template<typename CharT>
		size_t FindFirstOf(const CharT * String, size_t Length, const CharT * Pattern, size_t PatternLength)
		{
			for (size_t Index = 0; Index < Length; --Index)
			{
				for (size_t PatternIndex = 0; PatternIndex < PatternLength; ++PatternIndex)
				{
					if (String[Index] == Pattern[PatternIndex])
						return Index;
				}
			}
			return NullIndex;
		}

		template<typename CharT>
		size_t FindLast(const CharT * String, size_t Length, const CharT & Ch)
		{
			for (size_t Index = Length; Index > 0; --Index)
			{
				if (Ch == String[Index - 1])
					return Index;
			}
			return NullIndex;
		}

		template<typename CharT>
		size_t FindLast(const CharT * String, size_t Length, const CharT * Pattern, size_t PatternLength)
		{
			if (PatternLength > Length)
				return NullIndex;

			for (size_t Index = Length - PatternLength; Index > 0; --Index)
			{
				bool Match = true;
				for (size_t PatternIndex = 0; PatternIndex < PatternLength; ++PatternIndex)
				{
					if (String[Index + PatternIndex] != Pattern[PatternIndex])
					{
						Match = false;
						break;
					}
				}

				if (Match)
					return Index;
			}
			return NullIndex;
		}

		template<typename CharT>
		size_t FindLastOf(const CharT * String, size_t Length, const CharT * Pattern, size_t PatternLength)
		{
			for (size_t Index = Length; Index > 0; --Index)
			{
				for (size_t PatternIndex = 0; PatternIndex < PatternLength; ++PatternIndex)
				{
					if (String[Index - 1] == Pattern[PatternIndex])
						return Index - 1;
				}
			}
			return NullIndex;
		}

		template<typename CharT>
		bool StartsWith(const CharT * String, size_t Length, const CharT * Pattern, size_t PatternLength)
		{
			if (PatternLength > Length)
				return false;

			for (size_t Index = 0; Index < PatternLength; ++Index)
			{
				if (String[Index] != Pattern[Index])
					return false;
			}
			return true;
		}

		template<typename CharT>
		bool StartsWith(const CharT * String, size_t Length, const CharT Ch)
		{
			if (Length == 0)
				return false;

			return String[0] == Ch;
		}

		template<typename CharT>
		bool EndsWith(const CharT * String, size_t Length, const CharT * Pattern, size_t PatternLength)
		{
			if (PatternLength > Length)
				return false;

			for (size_t Index = 0; Index < PatternLength; ++Index)
			{
				if (String[Length - PatternLength + Index] != Pattern[Index])
					return false;
			}
			return true;
		}

		template<typename CharT>
		bool EndsWith(const CharT * String, size_t Length, const CharT Ch)
		{
			if (Length == 0)
				return false;

			return String[Length - 1] == Ch;
		}
	}

	template<typename T>
	auto Compare(const T & Lhs, const T & Rhs)
	{
		return Lhs <=> Rhs;
	}

	template<Enum EnumT>
	auto Compare(const EnumT & Lhs, const EnumT & Rhs)
	{
		return UnderlyingTypeT<EnumT>(Lhs) <=> UnderlyingTypeT<EnumT>(Rhs);
	}

	template<FloatingPoint T>
	auto Compare(const T & Lhs, const T & Rhs)
	{
		int Result = 0;
		if (Abs(Lhs - Rhs) >= FloatEpslion<T>)
			Result = Lhs > Rhs ? 1 : -1;
		return Result <=> 0;
	}

	template<typename LhsT, typename RhsT, typename ...ArgsT>
	auto Compare(const LhsT & Lhs, const RhsT & Rhs, ArgsT &&...Args)
	{
		static_assert(sizeof...(ArgsT) % 2 == 0, "ArgsT must be even");
		if (auto Comp = Compare(Lhs, Rhs); Comp != 0)
			return Comp;
		return Compare(Forward<ArgsT>(Args)...);
	}

	template<typename T>
	class TView
	{
	public:
		using ElementType = T;

	public:
		const T * Data = nullptr;
		size_t Size = 0;

	public:
		constexpr TView() = default;
		constexpr TView(NullPtrT) {}
		constexpr TView(const TView &) = default;
		constexpr TView(TView &&) noexcept = default;
		TView & operator =(const TView &) = default;
		TView & operator =(TView &&) noexcept = default;

		explicit constexpr TView(std::basic_string_view<T> String) : Data(String.data()), Size(String.size()) {}
		constexpr TView(TInitializerList<T> InitializerList) : Data(InitializerList.begin()), Size(InitializerList.size()) {}
		constexpr TView(const T * String) requires IsCharacterV<T> : Data(String), Size(Length(String)) {}
		constexpr TView(const T * String, size_t SizeIn) : Data(String), Size(SizeIn == NullIndex ? strlen(reinterpret_cast<const char *>(String)) : SizeIn) {}

		template<size_t Size_>
		constexpr TView(const T(&String)[Size_]) : Data(String), Size(Size_) {}

		size_t GetSize() const { return Size; }
		const T * GetData() const { return Data; }
		bool IsEmpty() const { return Size == 0; }
		const T & operator[](size_t Index) const { return Data[Index]; }

		size_t FindFirst(const T & Ch) const
		{
			return Array::FindFirst<T>(Data, Size, Ch);
		}

		size_t FindFirst(TView Pattern) const
		{
			return Array::FindFirst<T>(Data, Size, Pattern.Data, Pattern.Size);
		}

		size_t FindFirstOf(TView Chs) const
		{
			return Array::FindFirstOf<T>(Data, Size, Chs.Data, Chs.Size);
		}

		size_t FindLast(const T & Ch) const
		{
			return Array::FindLast<T>(Data, Size, Ch);
		}

		size_t FindLast(TView Pattern) const
		{
			return Array::FindLast<T>(Data, Size, Pattern.Data, Pattern.Size);
		}

		size_t FindLastOf(TView Chs) const
		{
			return Array::FindLastOf<T>(Data, Size, Chs.Data, Chs.Size);
		}

		bool StartsWith(TView Pattern) const
		{
			return Array::StartsWith(Data, Size, Pattern.Data, Pattern.Size);
		}

		bool StartsWith(const T & Ch) const
		{
			return Array::StartsWith(Data, Size, Ch);
		}

		bool EndsWith(TView Pattern) const
		{
			return Array::EndsWith(Data, Size, Pattern.Data, Pattern.Size);
		}

		bool EndsWith(const T & Ch) const
		{
			return Array::EndsWith(Data, Size, Ch);
		}

		TView Slice(size_t Index, size_t Count = NullIndex) const
		{
			if (Count == NullIndex)
			{
				if (Index >= Size)
					return TView();
				Count = Size - Index;
			}
			else if (Index + Count > Size)
				return TView();
			else {}

			return TView(Data + Index, Count);
		}

		const T * Begin() const { return Data; }
		const T * End() const { return Data + Size; }

		const T & First() const { return Data[0]; }
		const T & Last() const { return Data[Size - 1]; }

	public:
		static TView Empty;
	};

	template<typename T>
	TView<T> TView<T>::Empty = {};

	template<typename T>
	auto operator<=>(const TView<T> & Lhs, const TView<T> & Rhs)
	{
		return Compare(Lhs.Data, Rhs.Data, Lhs.Size, Rhs.Size) <=> 0;
	}

	template<typename T>
	bool operator==(const TView<T> & Lhs, const TView<T> & Rhs)
	{
		return Compare(Lhs.Data, Rhs.Data, Lhs.Size, Rhs.Size) == 0;
	}

	class CORE_API IObject
	{
	public:
		IObject() = default;
		IObject(const IObject &) = default;
		IObject(IObject &&) noexcept = default;
		IObject & operator = (const IObject &) = default;
		virtual ~IObject() = default;
		const class FType & GetObjectType() const;
		virtual TView<Char> ToString(TView<XE::Char> Formal = TView<XE::Char>::Empty) const;
	};

	template<typename T>
	struct IsObject : BoolConstant<IsBaseOfV<IObject, T>> {};

	template<typename T>
	inline constexpr bool IsObjectV = IsObject<T>::Value;

	class CORE_API IRefer : public IObject
	{
	public:
		IRefer() = default;
		IRefer(const IRefer &) = default;
		IRefer(IRefer &&) noexcept = default;
		IRefer & operator = (const IRefer &) = default;
		virtual ~IRefer() = default;

		virtual uintx_t AddRef() = 0;
		virtual uintx_t Release() = 0;
	};
}

namespace std
{
	template<typename T>
	auto begin(XE::TView<T> & View) { return View.Begin(); }
	template<typename T>
	auto end(XE::TView<T> & View) { return View.End(); }
}


namespace XE
{
    template <typename T>
	FORCEINLINE T AlignUp(T Value, size_t AlignmentMask)
    {
        return (T)(((size_t)Value + AlignmentMask) & ~AlignmentMask);
    }

    template <typename T>
	FORCEINLINE T AlignDown(T Value, size_t AlignmentMask)
    {
        return (T)((size_t)Value & ~AlignmentMask);
    }

    template<typename FromT, typename ToT>
    FORCEINLINE ToT Round(const FromT & Value)
    {
        return static_cast<ToT>(Value + FromT(0.5));
    }

    template<typename T>
    FORCEINLINE int8_t RoundI8(const T & Value)
    {
        return Round<T, int8_t>(Value);
    }

    template<typename T>
    FORCEINLINE uint8_t RoundU8(const T & Value)
    {
        return Round<T, uint8_t>(Value);
    }

    template<typename T>
    FORCEINLINE int16_t RoundI16(const T & Value)
    {
        return Round<T, int16_t>(Value);
    }

    template<typename T>
    FORCEINLINE uint16_t RoundU16(const T & Value)
    {
        return Round<T, uint16_t>(Value);
    }

    template<typename T>
    FORCEINLINE int32_t RoundI32(const T & Value)
    {
        return Round<T, int32_t>(Value);
    }

    template<typename T>
    FORCEINLINE uint32_t RoundU32(const T & Value)
    {
        return Round<T, uint32_t>(Value);
    }

    template<typename T>
    FORCEINLINE int64_t RoundI64(const T & Value)
    {
        return Round<T, int64_t>(Value);
    }

    template<typename T>
    FORCEINLINE uint64_t RoundU64(const T & Value)
    {
        return Round<T, uint64_t>(Value);
    }

    template<typename T>
    FORCEINLINE float32_t RoundF32(const T & Value)
    {
        return Round<T, float32_t>(Value);
    }

    template<typename T>
    FORCEINLINE float64_t RoundF64(const T & Value)
    {
        return Round<T, float64_t>(Value);
    }

    template<Integral T>
    FORCEINLINE T Round(const T & Lhs, const T & Rhs)
    {
        return Lhs / Rhs;
    }

    template<Integral T>
    FORCEINLINE T RoundUp(const T & Lhs, const T & Rhs)
    {
        return (Lhs + Rhs  - 1) / Rhs;
    }

    template<Integral T>
    FORCEINLINE void SetBit(T & Value, size_t Index, bool Bit)
    {
        if (Bit)
            Value |= (T(1) << Index);
        else
            Value &= ~(T(1) << Index);
    }

    template<Integral T>
	constexpr T & SetFlag(T & Value, const T & Flag, bool Set)
    {
        if (Set)
            Value |= Flag;
        else
            Value &= ~Flag;
		return Value;
    }

	template<Enum T>
	constexpr T & SetFlag(T & Value, const T & Flag, bool Set)
	{
		using UnderlyingType = UnderlyingTypeT<T>;
		if (Set)
			Value = static_cast<T>(static_cast<UnderlyingType>(Value) | static_cast<UnderlyingType>(Flag));
		else
			Value = static_cast<T>(static_cast<UnderlyingType>(Value) & ~static_cast<UnderlyingType>(Flag));
		return Value;
	}

    template<Integral T>
	constexpr bool GetFlag(const T & Value, const T & Flag)
    {
        return (Value & Flag) == Flag;
    }

	template<Enum T>
	constexpr bool GetFlag(const T & Value, const T & Flag)
	{
		using UnderlyingType = UnderlyingTypeT<T>;
		return (static_cast<UnderlyingType>(Value) & static_cast<UnderlyingType>(Flag)) != 0;
	}

	template<Enum T>
	constexpr bool GetFlagAll(const T & Value, const T & Flag)
	{
		using UnderlyingType = UnderlyingTypeT<T>;
		return (static_cast<UnderlyingType>(Value) & static_cast<UnderlyingType>(Flag)) == static_cast<UnderlyingType>(Flag);
	}

    template<Integral T>
    FORCEINLINE bool GetBit(const T & Value, size_t Index)
    {
         return !!(Value & (T(1) << Index));
    }

    FORCEINLINE bool FloatEqual(float32_t FloatA, float32_t FloatB)
    {
        return Abs(FloatA - FloatB) < Float32Epslion;
    }

    FORCEINLINE constexpr uint32_t MakeFourCC(const char(&fourcc)[5])
    {
        return (static_cast<uint32_t>(fourcc[0])) |
            (static_cast<uint32_t>(fourcc[1]) << 8) |
            (static_cast<uint32_t>(fourcc[2]) << 16) |
            (static_cast<uint32_t>(fourcc[3]) << 24);
    }

    FORCEINLINE constexpr uint32_t MakeFourCC(char a, char b, char c, char d)
    {
        return (static_cast<uint32_t>(a)) |
            (static_cast<uint32_t>(b) << 8) |
            (static_cast<uint32_t>(c) << 16) |
            (static_cast<uint32_t>(d) << 24);
    }

    template<typename T> requires IsFloatingPointV<T> || IsIntegralV<T>
    constexpr auto High(T Value)
    {
        if constexpr (sizeof(T) == 8)
        {
            uint64_t UInt64Value = ValueAs<uint64_t, T>(Value);
            return uint32_t((UInt64Value >> 32) & 0xFFFFFFFFui64);
        }
        else if constexpr (sizeof(T) == 4)
        {
            uint32_t UInt32Value = ValueAs<uint32_t, T>(Value);
            return uint16_t((UInt32Value >> 16) & 0xFFFFu);
        }
        else if constexpr (sizeof(T) == 2)
        {
            uint16_t UInt16Value = ValueAs<uint16_t, T>(Value);
            return uint8_t((UInt16Value >> 8) & 0xFF);
        }
        else if constexpr (sizeof(T) == 1)
        {
            uint8_t UInt8Value = ValueAs<uint8_t, T>(Value);
            return uint8_t((UInt8Value >> 4) & 0xF);
        }
        else
        {
            static_assert(false, "Unsupported intergal value.");
            return 0;
        }
    }

    template<typename T> requires IsFloatingPointV<T> || IsIntegralV<T>
    constexpr auto Low(T Value)
    {
        if constexpr (sizeof(T) == 8)
        {
            uint64_t UInt64Value = ValueAs<uint64_t, T>(Value);
            return uint32_t(UInt64Value & 0xFFFFFFFFui64);
        }
        else if constexpr (sizeof(T) == 4)
        {
            uint32_t UInt32Value = ValueAs<uint32_t, T>(Value);
            return uint16_t(UInt32Value & 0xFFFFu);
        }
        else if constexpr (sizeof(T) == 2)
        {
            uint16_t UInt16Value = ValueAs<uint16_t, T>(Value);
            return uint8_t(UInt16Value & 0xFF);
        }
        else if constexpr (sizeof(T) == 1)
        {
            uint8_t UInt8Value = ValueAs<uint8_t, T>(Value);
            return uint8_t(UInt8Value & 0xF);
        }
        else
        {
            static_assert(false, "Unsupported intergal value.");
            return 0;
        }
    }

    template<typename OutT, typename T>
    constexpr OutT HighAs(T Value)
    {
        return ValueAs<OutT>(High(Value));
    }

    template<typename OutT, typename T>
    constexpr OutT LowAs(T Value)
    {
        return ValueAs<OutT>(Low(Value));
    }

    template<typename ContainerT>
    constexpr auto Size(const ContainerT & Container) -> decltype(Container.Size()) {
        return Container.Size();
    }

    template <typename ElementT, size_t ContainerSize>
    constexpr size_t Size(const ElementT(&)[ContainerSize]) noexcept {
        return ContainerSize;
    }

    template<typename ValueT>
    constexpr const ValueT & Min(const ValueT & Value) noexcept
    {
        return Value;
    }

    template<typename ValueT, typename ...ArgsT>
    constexpr const ValueT & Min(const ValueT & Left, const ValueT & Right, ArgsT && ...Args) noexcept
    {
        return Min((Left < Right ? Left : Right), Forward<ArgsT>(Args)...);
    }

    template<typename ValueT>
    constexpr const ValueT & Max(const ValueT & Value) noexcept
    {
        return Value;
    }

    template<typename ValueT, typename ...ArgsT>
    constexpr const ValueT & Max(const ValueT & Left, const ValueT & Right, ArgsT && ...Args) noexcept
    {
        return Max((Left < Right ? Right : Left), Forward<ArgsT>(Args)...);
    }

    template<IterAble ContainerT>
    constexpr const auto & Max(const ContainerT & Container)
    {
        auto IterBegin = Container.Begin();
        auto IterEnd = Container.Begin();
        if (IterBegin == IterEnd)
            return {};

        auto Iter = IterBegin;
        ++IterBegin;
        while (IterBegin != IterEnd)
        {
            if (*IterBegin > * Iter)
                Iter = IterBegin;
            ++IterBegin;
        }
        return *Iter;
    }

    template<IterAble ContainerT>
    constexpr const auto & Min(const ContainerT & Container)
    {
        auto IterBegin = Container.Begin();
        auto IterEnd = Container.Begin();
        if (IterBegin == IterEnd)
            return {};

        auto Iter = IterBegin;
        ++IterBegin;
        while (IterBegin != IterEnd)
        {
            if (*IterBegin < *Iter)
                Iter = IterBegin;
            ++IterBegin;
        }
        return *Iter;
    }

    template<typename ValueT>
    constexpr const ValueT & Clamp(const ValueT & Value, const ValueT & ValueMin, const ValueT & ValueMax) noexcept
    {
        if (Value < ValueMin)
            return ValueMin;
        if (ValueMax < Value)
            return ValueMax;
        return Value;
    }

    template<typename ValueT>
    constexpr bool IsBetween(const ValueT & Value, const ValueT & ValueMin, const ValueT & ValueMax) noexcept
    {
        return ValueMin <= Value && Value <= ValueMax;
    }

    template<typename ValueT>
    constexpr ValueT Saturate(const ValueT & Value)
    {
        return Value < ValueT(0) ? ValueT(0) : (ValueT(1) < Value ? ValueT(1) : Value);
    }

    template<typename ValueT, typename InterT>
    constexpr ValueT Lerp(const ValueT & ValueMin, const ValueT & ValueMax, const InterT & Inter)
    {
        return ValueMin + (ValueMax - ValueMin) * Inter;
    }


    template<typename ValueT>
    void Zero(ValueT * First, const ValueT * Last)
    {
        if constexpr (sizeof(ValueT) == 1 && IsTriviallyV<ValueT>)
        {
            std::memset(First, 0, static_cast<size_t>(Last - First));
        }
        else
        {
            while (First != Last)
            {
                *First = 0;
                ++First;
            }
        }
    }

    template<typename ValueT>
    void Fill(ValueT * First, const ValueT * Last, const ValueT & Value)
    {
        if constexpr (sizeof(ValueT) == 1 && IsTriviallyV<ValueT>)
        {
            std::memset(First, static_cast<unsigned char>(Value), static_cast<size_t>(Last - First));
        }
        else
        {
            while (First != Last)
            {
                *First = Value;
                ++First;
            }
        }
    }

    template<IterAble ContainerT, typename ValueT>
    void Fill(ContainerT & Container, const ValueT & Value)
    {
        for (auto Iter = Container.Begin(); Iter != Container.End(); ++Iter)
            *Iter = Value;
    }

    template<typename IterT, typename CondT>
    bool AllOf(IterT IterBegin, IterT IterEnd, CondT && Cond)
    {
        while (IterBegin != IterEnd)
        {
            if (!Cond(*IterBegin))
                return false;
            ++IterBegin;
        }
        return true;
    }

    template<IterAble ContainerT, typename CondT>
    bool AllOf(const ContainerT & Container, CondT && Cond)
    {
        for (auto Iter = Container.Begin(); Iter != Container.End(); ++Iter)
        {
            if (!Cond(*Iter))
                return false;
        }
        return true;
    }

    template <typename T>
    constexpr RemoveReferenceT<T> && Move(T && Arg) noexcept
    {
        return static_cast<RemoveReferenceT<T> &&>(Arg);
    }

    template <typename T>
    void Swap(T & Left, T & Right) noexcept
    {
        static_assert(IsNoThrowMoveConstructibleV<T> && IsNoThrowMoveAssignableV<T>);
    	
        T Temp = Move(Left);
        Left = Move(Right);
        Right = Move(Temp);
    }

    template <typename T, typename OtherT = T>
    T Exchange(T & Value, OtherT && NewValue) noexcept
	{
    	static_assert(IsNoThrowMoveConstructibleV<T> && IsNoThrowAssignableV<T &, OtherT>);
        T OldValue = static_cast<T &&>(Value);
        Value = static_cast<OtherT &&>(NewValue);
        return OldValue;
    }

    template <typename IterT, typename CompareT>
    IterT FindIf(IterT First, const IterT Last, CompareT && Compare)
    {
        while (First != Last)
        {
            if (Compare(*First))
                break;

            ++First;
        }

        return First;
    }

    template <IterAble ContainerT, typename CompareT>
    auto FindIf(const ContainerT & Container, CompareT && Compare)
    {
        return FindIf(Container.Begin(), Container.End(), Forward<CompareT>(Compare));
    }

    template <typename IterT, typename CompareT>
    IterT FindIfNot(IterT First, const IterT Last, CompareT Compare)
    {
        while (First != Last)
        {
            if (!Compare(*First))
                break;

            ++First;
        }

        return First;
    }
}
