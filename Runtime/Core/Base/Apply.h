#pragma once

#include "Types.h"
#include "Tuple.h"

namespace XE
{
	template <typename CallableT, typename ...ArgsT>
	constexpr decltype(auto) Invoke(CallableT && Callable, ArgsT &&... Args)
	{
		return std::invoke(Forward<CallableT>(Callable), Forward<ArgsT>(Args)...);
	}
	
	template <typename CallableT, typename ArgsTupleT, size_t... Indices>
	constexpr decltype(auto) ApplyImpl(CallableT && Callable, ArgsTupleT && ArgsTuple, IndexSequence<Indices...>)
	{
		return Invoke(Forward<CallableT>(Callable), (Get<Indices>(ArgsTuple))...);
	}

	
	template <typename CallableT, typename ArgsTupleT>
	constexpr decltype(auto) Apply(CallableT && Callable, ArgsTupleT && ArgsTuple)
	{
		return ApplyImpl(Forward<CallableT>(Callable), Forward<ArgsTupleT>(ArgsTuple), MakeIndexSequence<TupleSizeV<RemoveReferenceT<ArgsTupleT>>>{});
	}
}