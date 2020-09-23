#pragma once

#include "CoreInc.h"

namespace XE
{
	class CORE_API FEvent
	{
	public:
		FEvent() = default;
		FEvent(FStringView Name_) : Name(Name_), EventId(CreateEventId()) {}
		auto operator<=>(const FEvent & Another) const { return EventId <=> Another.EventId; }
		bool operator==(const FEvent & Another) const { return EventId == Another.EventId; }

		FString Name;
		uintx_t EventId = 0;
	private:
		static uintx_t CreateEventId();
	};

	class CORE_API FEventArgs
	{
	public:
		FEventArgs(const FEvent & Event_) : Event(Event_) {}

		const FEvent & Event;
		bool Handled = false;
	};
	
	template<typename T>
	class TDelegate;

	template<typename ReturnT, typename ...ParametersT>
	class TDelegate<ReturnT(ParametersT...)>
	{
	public:
		TDelegate() = default;
		~TDelegate() = default;

		template<typename CallableT, typename ...CallableParametersT>
		void Bind(CallableT && Callable, CallableParametersT && ...CallableParameters)
		{
			Function = TFunction<ReturnT(ParametersT...)>(Callable, Forward<CallableParametersT>(CallableParameters)...);
		}

		bool IsBound() const { return !!Function; }

		template<typename ...ArgsT>
		ReturnT Excute(ArgsT && ...Args)
		{
			if constexpr (IsSameV<ReturnT, void>)
			{
				if (!Function)
					return;
				Function(Forward<ArgsT>(Args)...);
			}
			else
			{
				if (!Function)
					return {};
				return Function(Forward<ArgsT>(Args)...);
			}
		}

		TFunction<ReturnT(ParametersT...)> Function;
	};

	enum class EReply
	{
		None = 0,
		Handled,
	};
	
	using FEventHandle = ptr_t;

	template<typename ...ParametersT>
	class TEvent
	{
	public:
		TEvent() = default;
		~TEvent() = default;

		template<typename CallableT, typename ...CallableParametersT>
		FEventHandle AddBind(CallableT && Callable, CallableParametersT && ...CallableParameters)
		{
			auto & Function = Functions.Emplace(TFunction<EReply(ParametersT...)>(Callable, Forward<CallableParametersT>(CallableParameters)...));
			return FEventHandle(Function.Storage);
		}

		void UnBind(FEventHandle EventHandle)
		{
			auto iter = FindIf(Functions, [&](auto & Function) { return FEventHandle(Function.Storage) == EventHandle; });
			Functions.Erase(iter);
		}
		
		bool IsBound() const { return !Functions.IsEmpty(); }
		
		template<typename ...ArgsT>
		EReply Excute(ArgsT && ...Args)
		{
			for (auto & Function : Functions)
			{
				assert(!!Function);
				EReply Reply = Function(Forward<ArgsT>(Args)...);
				if (Reply == EReply::Handled)
					return EReply::Handled;
			}
			return EReply::None;
		}

		TVector<TFunction<EReply(ParametersT...)>> Functions;
	};

	class CORE_API FDelegate
	{
	public:
		FDelegate() = default;
		auto operator<=>(const FDelegate &) const = default;
		bool operator==(const FDelegate &)const = default;

		FDelegate(FVariant Instance_, FMethod Method_) : Instance(Instance_), Method(Method_) {}

		FVariant Instance;
		FMethod Method;

		FVariant Invoke(TView<FVariant> Args) const { return  Method.Invoke(Instance, Args); }
	};
}
