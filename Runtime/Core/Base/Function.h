#pragma once

#include "Types.h"

namespace XE
{
	class ICallableStorge
	{
	public:
		virtual ~ICallableStorge() = default;
		virtual ICallableStorge * Copy() = 0;
	};
	
	template<typename CallableT, typename ...CallableParametersT>
	class TCallableStorge : public ICallableStorge
	{
	public:
		TCallableStorge(CallableT && CallableIn, CallableParametersT && ...CallableParametersIn) : Callable(CallableIn), CallableParameters(Forward<CallableParametersT>(CallableParametersIn)...){}
		TCallableStorge(const TCallableStorge & Another) : Callable(Another.Callable), CallableParameters(Another.CallableParameters) {}

		ICallableStorge * Copy() override
		{
			return New<TCallableStorge>(*this);
		}
		CallableT Callable;
		TTuple<DecayT<CallableParametersT>...> CallableParameters;
	};

	template<typename CallableT, typename ReturnT, typename... ParametersT>
	class TCallableCaller
	{
	public:
		TCallableCaller() = default;

		template<size_t ...Indices, typename ...ArgsT>
		static ReturnT CallThis(IntegerSequence<size_t, Indices...>, CallableT && Callable, TTuple<ArgsT...> & Args, ParametersT &&...Parameters)
		{
			return Invoke(Callable, (Get<Indices>(Args))..., Forward<ParametersT>(Parameters)...);
		}

		template<typename ...ArgsT>
		static ReturnT Call(const ICallableStorge * This, ParametersT &&...Parameters)
		{
			TCallableStorge<CallableT, ArgsT...> * CallableStorge = const_cast<TCallableStorge<CallableT, ArgsT...> *>(static_cast<const TCallableStorge<CallableT, ArgsT...> *>(This));
			return CallThis(IndexSequenceFor<ArgsT...>{}, Forward<CallableT>(CallableStorge->Callable), CallableStorge->CallableParameters, Forward<ParametersT>(Parameters)...);
		}
	};

	template <typename T>
	class TFunction
	{
		static_assert(FalseTypeV<T>, "TFunction does not accept non-function types as template arguments.");
	};

	template <typename ReturnT, typename... ParametersT>
	class TFunction<ReturnT(ParametersT...)>
	{
	public:
		TFunction() = default;
		auto operator <=> (const TFunction &) const noexcept = default;

		TFunction(const TFunction & Another)
		{
			if (Another.Storage)
			{
				Storage = Another.Storage->Copy();
				Caller = Another.Caller;
			}
		}
		
		TFunction(TFunction && Another) noexcept
		{
			Storage = Another.Storage;
			Caller = Another.Caller;
			Another.Storage = nullptr;
			Another.Caller = nullptr;
		}
		
		template<typename CallableT, typename ...CallableParametersT, typename = EnableIfT<!IsSameV<DecayT<CallableT>, TFunction<ReturnT(ParametersT...)>>>>
		TFunction(CallableT && Callable, CallableParametersT && ...CallableParameters)
		{
			using CallableStorgeT = TCallableStorge<CallableT, CallableParametersT...>;
			using CallableCallerT = TCallableCaller<CallableT, ReturnT, ParametersT...>;
			Storage = New<CallableStorgeT>(Forward<CallableT>(Callable), Forward<CallableParametersT>(CallableParameters)...);
			Caller = CallableCallerT::template Call<CallableParametersT...>;
		}

		~TFunction()
		{
			if (Storage)
			{
				Delete(Storage);
			}
		}

		ReturnT operator()(ParametersT...Parameters) const
		{
			return Caller(Storage, Forward<ParametersT>(Parameters)...);
		}

		TFunction & operator = (const TFunction & Another) noexcept
		{
			if (Storage)
			{
				Delete(Storage);
			}
			Storage = Another.Storage->Copy();
			Caller = Another.Caller;
			return *this;
		}

		TFunction & operator = (TFunction && Another) noexcept
		{
			if (Storage)
			{
				Delete(Storage);
			}
			
			Storage = Another.Storage;
			Caller = Another.Caller;
			Another.Storage = nullptr;
			Another.Caller = nullptr;
			return *this;
		}

		explicit operator bool() const { return !!Storage; }
		
	public:
		ICallableStorge * Storage = nullptr;
		ReturnT(*Caller)(const ICallableStorge *, ParametersT &&...) = nullptr;
	};


	template<int Index>
	struct TPlacer
	{
		TPlacer() = default;
		TPlacer(const TPlacer &) = default;
		TPlacer(TPlacer &&) noexcept = default;
	};

	constexpr TPlacer<1>  Placer1 = {};
	constexpr TPlacer<2>  Placer2 = {};
	constexpr TPlacer<3>  Placer3 = {};
	constexpr TPlacer<4>  Placer4 = {};
	constexpr TPlacer<5>  Placer5 = {};
	constexpr TPlacer<6>  Placer6 = {};
	constexpr TPlacer<7>  Placer7 = {};
	constexpr TPlacer<8>  Placer8 = {};
	constexpr TPlacer<9>  Placer9 = {};
	constexpr TPlacer<10> Placer10 = {};
	constexpr TPlacer<11> Placer21 = {};
	constexpr TPlacer<12> Placer22 = {};
	constexpr TPlacer<13> Placer23 = {};
	constexpr TPlacer<14> Placer24 = {};
	constexpr TPlacer<15> Placer25 = {};
	constexpr TPlacer<16> Placer26 = {};
	constexpr TPlacer<17> Placer27 = {};
	constexpr TPlacer<18> Placer28 = {};
	constexpr TPlacer<19> Placer29 = {};

	template<typename T>
	struct IsPlaceholder : IntegralConstant<int, 0> {};

	template <int Index>
	struct IsPlaceholder<TPlacer<Index>> : IntegralConstant<int, Index>
	{
	};

	template <typename T>
	struct IsPlaceholder<const T> : IsPlaceholder<T>::Type { };

	template <typename T>
	struct IsPlaceholder<volatile T> : IsPlaceholder<T>::Type { };

	template <typename T>
	struct IsPlaceholder<const volatile T> : IsPlaceholder<T>::Type { };

	template <typename T>
	constexpr int IsPlaceholderV = IsPlaceholder<T>::Value;

	template<typename ParameterT, typename ArgsTupleT>
	struct TParameterExtractor
	{
		auto Extract(ParameterT & Parameter, ArgsTupleT &)
		{
			return Parameter;
		}
	};

	template<int Index, typename ArgsTupleT>
	struct TParameterExtractor<const TPlacer<Index> &, ArgsTupleT>
	{
		TupleElementT<Index - 1, ArgsTupleT> Extract(const TPlacer<Index> &, ArgsTupleT & ArgsTuple)
		{
			static_assert(Index > 0, "Placer index must be greater than 0");
			return Forward<TupleElementT<Index - 1, ArgsTupleT>>(Get<Index - 1>(ArgsTuple));
		}
	};


	template<size_t ...Indices, typename CallableT, typename ParametersTupleT, typename ArgsTupleT>
	auto TBinderCaller(IntegerSequence<size_t, Indices...>, CallableT && Callable, ParametersTupleT & Parameters, ArgsTupleT && Args)
	{
		return Invoke(Callable, (TParameterExtractor<TupleElementT<Indices, ParametersTupleT>, ArgsTupleT>().Extract(Get<Indices>(Parameters), Args))...);
	}

	template<typename CallableT, typename ...ParametersT>
	struct TBinder
	{
		CallableT Callable;
		TTuple<ParametersT...> Parameters;

		TBinder(CallableT && CallableIn, ParametersT &&...ParametersIn) : Callable(Forward<CallableT>(CallableIn)), Parameters(Forward<ParametersT>(ParametersIn)...)
		{

		}

		template <typename... ArgsT>
		auto operator()(ArgsT &&... Args) const
		{
			return TBinderCaller(IndexSequenceFor<ParametersT...>{}, Callable, Parameters, ForwardAsTuple(Args...));
		}

		template <typename... ArgsT>
		auto operator()(ArgsT &&... Args)
		{
			return TBinderCaller(IndexSequenceFor<ParametersT...>{}, Callable, Parameters, ForwardAsTuple(Forward<ArgsT>(Args)...));
		}
	};

	template<typename CallableT, typename ...ParametersT>
	auto Bind(CallableT && Callable, ParametersT &&...Parameters)
	{
		return TBinder<CallableT, ParametersT...>(Forward<CallableT>(Callable), Forward<ParametersT>(Parameters)...);
	}
}