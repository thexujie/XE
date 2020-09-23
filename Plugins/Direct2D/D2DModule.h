#pragma once

#include "D2DInc.h"


namespace XE::Direct2D
{
	class FDirect2DModule : public IModule
	{
	public:
		FDirect2DModule() = default;

		EError InitializeModule() override;
		void FinalizeModule() override;
	};
}
