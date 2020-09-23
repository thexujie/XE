#pragma once

#include "Config.h"
#include "Types.h"
#include "Resumable.h"
#include "Enumerable.h"
#include "System.h"
#include "Atomic.h"
#include "Random.h"
#include "Local.h"
#include "Format.h"
#include "Hash.h"
#include "Reference.h"
#include "Flag.h"
#include "Stream.h"
#include "Archive.h"

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quat.h"
#include "Matrix.h"
#include "Transform.h"
#include "Fraction.h"

#include "Allcoator.h"
#include "Atomic.h"
#include "Tuple.h"
#include "StringView.h"
#include "Array.h"
#include "List.h"
#include "Formatter.h"
#include "Log.h"

#include "Culture.h"
#include "Guid.h"
#include "Color.h"

#include "StringStream.h"
#include "Apply.h"
#include "Function.h"

#include "Pointer.h"
#include "ReferPtr.h"
#include "UniquePtr.h"
#include "SharedPtr.h"

#include "BitSet.h"
#include "Vector.h"
#include "Pair.h"
#include "String.h"
#include "RBTree.h"
#include "Set.h"
#include "Map.h"
#include "HashMap.h"
#include "LinearMap.h"
#include "LinearHashMap.h"
#include "MultiMap.h"
#include "UnorderedMap.h"

#include "Variant.h"

#include "Type.h"
#include "DataConverter.h"
#include "Converters.h"

namespace XE
{
	template<typename ...ArgsT>
	void Assert(bool Value)
	{
#ifdef _DEBUG
		assert(Value);
#endif
	}

	template<typename ...ArgsT>
	void Assert(bool Value, const wchar_t * Message)
	{
#ifdef _DEBUG
		if (!Value)
			_wassert(Message, _CRT_WIDE(__FILE__), (unsigned)(__LINE__));
#endif
	}
}
