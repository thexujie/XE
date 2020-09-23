#pragma once

#include "CoreInc.h"

namespace XE
{
	class CORE_API IModule
	{
	public:
		IModule() = default;
		virtual ~IModule() = default;

		virtual EError InitializeModule() = 0;
		virtual void FinalizeModule() = 0;
		
	private:
		FString Name;
	};
}
