#include "PCH.h"
#include "Material.h"

#include "RenderFactory.h"

namespace XE::Render
{
	FMaterialFactory & FMaterialFactory::Instance()
	{
		static FMaterialFactory MaterialFactoryInstance;
		return MaterialFactoryInstance;
	}
}
