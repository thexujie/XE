#include "PCH.h"
#include "Allcoator.h"

namespace XE
{
#ifdef _DEBUG
	std::map<void *, size_t> * GDebugPointers = nullptr;
	//std::map<void *, size_t> * GDebugPointers = new std::map<void *, size_t>();
#endif
	
	void * Malloc(size_t Size, size_t Alignment)
	{
		assert(Size > 0);
		void * Result = nullptr;
		if (Alignment > 1)
			Result = ::_aligned_malloc(Size, Alignment);
		else
			Result = ::malloc(Size);

		Assert(Result != nullptr);
#ifdef _DEBUG
		if (GDebugPointers)
			GDebugPointers->insert_or_assign(Result, Alignment);
#endif
		return Result;
	}

	void Free(void * Pointer, size_t Alignment)
	{
		if (!Pointer)
			return;

#ifdef _DEBUG
		if (GDebugPointers)
		{
			Assert(GDebugPointers->contains(Pointer), L"Not contains");
			Assert(GDebugPointers->at(Pointer) == Alignment, L"Invalid alignment");
		}
#endif
		if (Alignment > 1)
			::_aligned_free(Pointer);
		else
			::free(Pointer);
	}
}
