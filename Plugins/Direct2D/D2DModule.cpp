#include "PCH.h"
#include "D2DModule.h"
#include "D2DDevice.h"


namespace XE::Direct2D
{
	
	EError FDirect2DModule::InitializeModule()
	{
		return EError::OK;
	}
	
	void FDirect2DModule::FinalizeModule()
	{
		
	}

	static FDirect2DModule GDirect2DModule;
}
