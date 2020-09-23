#include "PCH.h"
#include "RHID3D12.h"

#include "RHID3D12Factory.h"

namespace XE::RHI::D3D12
{
	TSharedPtr<FRHIFactory> CreateRHIFactory()
	{
		return MakeShared<FRHID3D12Factory>();
	}
}
