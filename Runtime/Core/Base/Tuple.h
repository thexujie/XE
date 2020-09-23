#pragma once

#include "Types.h"

namespace XE
{
	template<size_t Index, typename T>
	struct TupleValue
	{
		TupleValue() = default;
		TupleValue(const TupleValue &) = default;
		TupleValue(TupleValue &&) = default;
		TupleValue & operator=(TupleValue && Another) = default;
		TupleValue & operator=(const TupleValue & Another) = default;
		auto operator <=>(const TupleValue &) const = default;

		template<typename T2>
		explicit TupleValue(T2 && ValueIn) : Value(Forward<T2>(ValueIn)) {}

		explicit TupleValue(const T & ValueIn) : Value(ValueIn) {}

		template <typename = EnableIfT<IsMoveConstructibleV<T>>>
		explicit TupleValue(T && ValueIn) : Value(Forward<T>(ValueIn)) {}

		template<typename T2>
		TupleValue & operator=(T2 && AnotherValue) { Value = Forward<T2>(AnotherValue); return *this; }
		template<typename T2>
		TupleValue & operator=(const T2 & AnotherValue) { Value = AnotherValue; return *this; }

		T Value;
	};

	template <typename Indices, typename... TypesT>
	struct TupleStorage;

	template<typename ...TypesT>
	class TTuple;

	template <size_t Index, typename TupleT>
	struct TupleElement {};

	template<size_t Index, typename HeadT, typename ...TailT>
	struct TupleElement<Index, TTuple<HeadT, TailT...>> : TupleElement<Index - 1, TTuple<TailT...>> { };

	template<typename HeadT, typename ...TailT>
	struct TupleElement<0, TTuple<HeadT, TailT...>>
	{
		using Type = HeadT;
	};

	template <size_t Index, typename TupleT>
	using TupleElementT = typename TupleElement<Index, TupleT>::Type;

	template <size_t... Indices, typename... TypesT>
	struct TupleStorage<IntegerSequence<size_t, Indices...>, TypesT...> : public TupleValue<Indices, TypesT>...
	{
	public:
		using TupleType = TTuple<TypesT...>;

	public:
		TupleStorage() = default;
		TupleStorage(TupleStorage && Another) = default;
		TupleStorage(const TupleStorage & Another) = default;
		TupleStorage & operator=(TupleStorage && Another) = default;
		TupleStorage & operator=(const TupleStorage & Another) = default;
		auto operator <=>(const TupleStorage &) const = default;

		TupleStorage(XE::IntegerSequence<size_t, Indices...>, TypesT &&... Args) : TupleValue<Indices, TypesT>(Forward<TypesT>(Args))...
		{
		}

		template<typename ...Args2T>
		TupleStorage(XE::IntegerSequence<size_t, Indices...>, Args2T &&... Args) : TupleValue<Indices, TypesT>(Forward<Args2T>(Args))...
		{
		}

		template <typename TupleStorage2T>
		TupleStorage(TupleStorage2T && Another)
			: TupleValue<Indices, TypesT>(Forward<TupleElementT<Indices, typename TupleStorage2T::TupleType>>(Forward<TupleStorage2T>(Another).template Get<Indices>()))...
		{
		}

		template <typename TupleStorage2T>
		TupleStorage & operator=(TupleStorage2T && Another)
		{
			Swallow(TupleValue<Indices, TypesT>::operator=(Forward<TupleElementT<Indices, typename TupleStorage2T::TupleType>>(Forward<TupleStorage2T>(Another).template Get<Indices>()))...);
			return *this;
		}

		template<size_t Index>
		TupleElementT<Index, TTuple<TypesT...>> & Get() &
		{
			using ElementT = TupleElementT<Index, TTuple<TypesT... >>;
			return static_cast<TupleValue<Index, ElementT> &>(*this).Value;
		}


		template<size_t Index>
		const TupleElementT<Index, TTuple<TypesT...>> & Get() const &
		{
			using ElementT = TupleElementT<Index, TTuple<TypesT... >>;
			return static_cast<const TupleValue<Index, ElementT> &>(*this).Value;
		}


		template<size_t Index>
		TupleElementT<Index, TTuple<TypesT...>> && Get() &&
		{
			using ElementT = TupleElementT<Index, TTuple<TypesT... >>;
			return static_cast<TupleValue<Index, ElementT> &&>(*this).Value;
		}
	};


	template<typename ...TypesT>
	class TTuple
	{
	public:
		using TupleStorageT = TupleStorage<MakeIndexSequence<sizeof...(TypesT)>, TypesT...>;

	public:
		TTuple() = default;
		auto operator <=>(const TTuple &) const = default;
		
		TTuple(TypesT &&...Args) : Storage(MakeIndexSequence<sizeof...(TypesT)>{}, Forward<TypesT>(Args)...) {}

		template<typename ...Args2T>
		TTuple(Args2T &&...Args) : Storage(MakeIndexSequence<sizeof...(Args2T)>{}, Forward<Args2T>(Args)...) {}

		//template <typename Tuple2T>
		//TTuple(Tuple2T && Another) : Storage(Forward<RemoveCVRefT<typename Tuple2T::TupleStorageT> >(Another.Storage))
		//{
		//}

		template <typename ...Args2T>
		TTuple & operator=(TTuple<Args2T...> && Another)
		{
			Storage.operator=(Forward<typename RemoveCVRefT<TTuple<Args2T...>>::TupleStorageT >(Another.Storage));
			return *this;
		}

		TupleStorageT Storage;
	};

	template<typename ...TypesT>
	constexpr bool IsTupleV = false;

	template<typename ...TypesT>
	constexpr bool IsTupleV<TTuple<TypesT...>> = true;
	
	template<>
	class TTuple<>
	{
	public:
		TTuple() = default;
		auto operator <=>(const TTuple &) const = default;
	};

	template <size_t Index, typename... TypesT>
	inline TupleElementT<Index, TTuple<TypesT...>> & Get(TTuple<TypesT...> & Tuple)
	{
		return Tuple.Storage.template Get<Index>();
	}

	template <size_t Index, typename... TypesT>
	inline const TupleElementT<Index, TTuple<TypesT...>> & Get(const TTuple<TypesT...> & Tuple)
	{
		return Tuple.Storage.template Get<Index>();
	}

	template <size_t Index, typename... TypesT>
	inline TupleElementT<Index, TTuple<TypesT...>> && Get(TTuple<TypesT...> && Tuple)
	{
		return Tuple.Storage.template Get<Index>();
	}

	template<typename TupleT>
	struct TupleSize;

	template <typename... TypesT>
	struct TupleSize<TTuple<TypesT...>> : IntegralConstant<size_t, sizeof...(TypesT)>
	{
	};

	template <typename TupleT>
	constexpr size_t TupleSizeV = TupleSize<TupleT>::Value;

	template <typename... ArgsT>
	FORCEINLINE TTuple<DecayT<ArgsT>...> MakeTuple(ArgsT &&... Args)
	{
		return TTuple<DecayT<ArgsT>...>(Forward<ArgsT>(Args)...);
	}

	struct FIgnore
	{
		template<typename T>
		constexpr const FIgnore & operator=(const T &) const noexcept { return *this; }
	};
	inline constexpr FIgnore Ignore{};

	template<typename ...ArgsT>
	constexpr TTuple<ArgsT &...> Tie(ArgsT &... Args) noexcept
	{
		return TTuple<ArgsT &...>(Args...);
	}

	template<typename ...ArgsT>
	constexpr TTuple<ArgsT &&...> ForwardAsTuple(ArgsT &&... Args) noexcept
	{
		return TTuple<ArgsT &&...>(Forward<ArgsT>(Args)...);
	}
}


// for structure binding
namespace std
{
	template <size_t Index, typename... TypesT>
	inline XE::TupleElementT<Index, XE::TTuple<TypesT...>> & get(XE::TTuple<TypesT...> & Tuple)
	{
		return Tuple.Storage.template Get<Index>();
	}

	template <size_t Index, typename... TypesT>
	inline const XE::TupleElementT<Index, XE::TTuple<TypesT...>> & get(const XE::TTuple<TypesT...> & Tuple)
	{
		return Tuple.Storage.template Get<Index>();
	}

	template <size_t Index, typename... TypesT>
	inline XE::TupleElementT<Index, XE::TTuple<TypesT...>> && get(XE::TTuple<TypesT...> && Tuple)
	{
		return Move(Tuple.Storage).template Get<Index>();
	}

	template <typename... TypesT>
	struct tuple_size<XE::TTuple<TypesT...>> : std::integral_constant<size_t, sizeof...(TypesT)>
	{
	};

	template <size_t Index, typename TupleT>
	struct tuple_element_helper
	{
		using type = typename XE::TupleElement<Index, TupleT>::Type;
	};

	template <size_t Index, typename... TypesT>
	struct tuple_element<Index, XE::TTuple<TypesT...>> : public tuple_element_helper<Index, XE::TTuple<TypesT...>>
	{
	};
}
