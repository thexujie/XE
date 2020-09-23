#include "PCH.h"
#include "Visual.h"

#include "Control.h"

namespace XE::UI
{
	void Reflection(TNativeType<UVisual> & NativeType)
	{
	}

	FStringView UVisual::ToString(FStringView Formal) const
	{
		return Format(Str("{} {{{}}}"), GetObjectType().GetShortName(), RenderRect);
	}
	
}
