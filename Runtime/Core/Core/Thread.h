#pragma once

#include "CoreInc.h"

namespace XE
{
	class CORE_API FThread
	{
	public:
		FThread() = default;
		FThread(const FThread & That) = delete;
		FThread(FThread && That) noexcept : Id(That.Id), Handle(That.Handle)
		{
			That.Id = 0;
			That.Handle = nullptr;
		}
		FThread & operator = (const FThread &) = delete;
		FThread & operator = (FThread && That) noexcept;
		
		~FThread()
		{
			if (Id)
			{
				Join();
				Id = 0;
			}
		}

		explicit FThread(TFunction<void()> FunctionIn);
		
		template<typename CallableT, typename... ArgsT, typename = EnableIfT<!IsSameV<RemoveCVT<RemoveReferenceT<CallableT>>, FThread>, int>>
		explicit FThread(CallableT && Callable, ArgsT &&... Args)
		{
			struct SThreadCallableArgs
			{
				SThreadCallableArgs(CallableT && CallableIn, ArgsT &&... Args) : Callable(CallableIn), Tuple(Args...)
				{
					
				}
				CallableT Callable;
				TTuple<ArgsT...> Tuple;
			};

			auto FuncEntry = [](void * Args)->uint32_t
			{
				SThreadCallableArgs * FunctionArgs = (SThreadCallableArgs *)Args;
				Apply<CallableT, TTuple<ArgsT...>>(Forward<CallableT>(FunctionArgs->Callable), Forward<TTuple<ArgsT...>>(FunctionArgs->Tuple));
				Delete(FunctionArgs);
				return 0;
			};

			SThreadCallableArgs * FunctionArgs = New<SThreadCallableArgs>(Forward<CallableT>(Callable), Forward<ArgsT>(Args)...);
			Handle = (ptr_t)_beginthreadex(nullptr, 0, FuncEntry, FunctionArgs, 0, &Id);
		}
		bool Joinable() const { return !!Handle; }
		void Join();

	protected:
		uint32_t Id = 0;
		ptr_t Handle = nullptr;
	};
}