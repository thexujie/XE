#include "PCH.h"
#include "Resource.h"

namespace XE
{
	FProperty FResource::KeyProperty(Str("Key"), &FResource::Key);
	
	void Reflection(TNativeType<FResource> & NativeType)
	{
		NativeType.RegisterProperty(FResource::KeyProperty);
	}
}