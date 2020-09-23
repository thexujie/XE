#pragma once

#include "Types.h"

namespace XE
{
	inline void * Memcpy(void * Dst, const void * Src, size_t Count)
	{
		return memcpy(Dst, Src, Count);
	}

	inline void * Memset(void * Dst, uint8_t Value, size_t Count)
	{
		return memset(Dst, Value, Count);
	}

	inline int Memcmp(const void * Lhs, const void * Rhs, size_t Count)
	{
		return memcmp(Lhs, Rhs, Count);
	}

	inline void * Memzero(void * Dst, size_t Count)
	{
		return memset(Dst, 0, Count);
	}

	inline void * MemMove(void * Dst, void * Src, size_t Size)
	{
		return memmove(Dst, Src, Size);
	}

	template<typename T, size_t Size>
	inline void * Memzero(T (&Dst)[Size])
	{
		return memset(Dst, 0, sizeof(T) * Size);
	}
	
	enum class EInitializeMode
	{
		Default = 0,
		Construct,
		Zero,
		None,
	};

	CORE_API void * Malloc(size_t Size, size_t Alignment = 0);
	CORE_API void Free(void * Pointer, size_t Alignment = 0);

	template<typename T, typename ...ArgsT>
	T * New(ArgsT &&...Args)
	{
		T * Pointer = static_cast<T *>(Malloc(sizeof(T), alignof(T)));
		new (Pointer) T(Forward<ArgsT>(Args)...);
		return Pointer;
	}

	template<typename T>
	void Delete(T *& Pointer)
	{
		Pointer->~T();
		Free(Pointer, alignof(T));
		Pointer = nullptr;
	}

	template<typename T>
	void DeleteUnSafe(T * Pointer)
	{
		Pointer->~T();
		Free(Pointer, alignof(T));
	}

	template<typename T>
	class TAllocator
	{
	public:
		TAllocator() = default;
		TAllocator(const TAllocator & That) = delete;
		TAllocator(TAllocator && That) = delete;

		static T * Alloc()
		{
			return static_cast<T *>(Malloc(sizeof(T), alignof(T)));
		}

		static T * Alloc(size_t Count)
		{
			return static_cast<T *>(Malloc(sizeof(T) * Count, alignof(T)));
		}

		static void Free(T * Pointer)
		{
			XE::Free(static_cast<void *>(Pointer), alignof(T));
		}

		template<typename ...ArgsT>
		static T * New(ArgsT &&... Args)
		{
			T * Pointer = Alloc();
			new (Pointer) T(Forward<ArgsT>(Args)...);
			return Pointer;
		}

		static void Delete(T * Pointer)
		{
			Pointer->~T();
			Free(Pointer);
		}

		template<typename ...ArgsT>
		static void PlaceNew(T * Dest, ArgsT &&... Args)
		{
			new (Dest) T(Forward<ArgsT>(Args)...);
		}

		static constexpr void Construct(T * Dest, size_t Count, EInitializeMode InitializeMode = EInitializeMode::Default)
		{
			switch (InitializeMode)
			{
			case EInitializeMode::Default:
				if constexpr (IsTriviallyConstructibleV<T>)
					Memset(Dest, 0, sizeof(T) * Count);
				else
				{
					for (size_t Index = 0; Index < Count; ++Index)
						new (Dest + Index) T();
				}
				break;
			case EInitializeMode::Construct:
				for (size_t Index = 0; Index < Count; ++Index)
					new (Dest + Index) T();
				break;
			case EInitializeMode::Zero:
				Memset(Dest, 0, sizeof(T) * Count);
				break;
			default:
				break;
			}
		}

		static void Construct(T * Dest, const T * Source, size_t Count, EInitializeMode InitializeMode = EInitializeMode::Default)
		{
			switch (InitializeMode)
			{
			case EInitializeMode::Default:
				if constexpr (IsTriviallyConstructibleV<T>)
					Memcpy(Dest, Source, sizeof(T) * Count);
				else
				{
					for (size_t Index = 0; Index < Count; ++Index)
						new (Dest + Index) T(Source[Index]);
				}
				break;
			case EInitializeMode::Construct:
				for (size_t Index = 0; Index < Count; ++Index)
					new (Dest + Index) T(Source[Index]);
				break;
			case EInitializeMode::Zero:
				Memcpy(Dest, Source, sizeof(T) * Count);
				break;
			default:
				break;
			}
		}

		static void Construct(T * Dest, const T & Source, size_t Count, EInitializeMode InitializeMode = EInitializeMode::Default)
		{
			switch (InitializeMode)
			{
			case EInitializeMode::Default:
				if constexpr (IsTriviallyConstructibleV<T>)
				{
					for (size_t Index = 0; Index < Count; ++Index)
						Memcpy(Dest + Index, &Source, sizeof(T));
				}
				else
				{
					for (size_t Index = 0; Index < Count; ++Index)
						new (Dest + Index) T(Source);
				}
				break;
			case EInitializeMode::Construct:
				for (size_t Index = 0; Index < Count; ++Index)
					new (Dest + Index) T(Source);
				break;
			case EInitializeMode::Zero:
				for (size_t Index = 0; Index < Count; ++Index)
					Memcpy(Dest + Index, &Source, sizeof(T));
				break;
			default:
				break;
			}
		}

		static void Destruct(const T * Source, size_t Count)
		{
			if constexpr (!IsTriviallyConstructibleV<T>)
			{
				for (size_t Index = 0; Index < Count; ++Index)
					(Source + Index)->~T();
			}
		}

		static void MoveCopy(T * Dest, T * Source, size_t Count)
		{
			if constexpr (IsTriviallyMoveConstructibleV<T>)
			{
				if (static_cast<size_t>(Abs(Dest - Source)) < Count)
					MemMove(Dest, Source, sizeof(T) * Count);
				else
					Memcpy(Dest, Source, sizeof(T) * Count);
			}
			else
			{
				for (size_t Index = 0; Index < Count; ++Index)
					new (Dest + Index) T(Move(Source[Index]));
			}
		}
		static void MoveCopy(T * Dest, T * Source)
		{
			MoveCopy(Dest, Source, 1);
		}

		static void Copy(T * Dest, const T & Value, size_t Count)
		{
			if constexpr (IsTriviallyMoveConstructibleV<T>)
			{
				for (size_t Index = 0; Index < Count; ++Index)
					Memcpy(Dest + Index, &Value, sizeof(T));
			}
			else
			{
				for (size_t Index = 0; Index < Count; ++Index)
					Dest[Index] = Value;
			}
		}

		static void Copy(T * Dest, const T * Source, size_t Count)
		{
			if constexpr (IsTriviallyMoveConstructibleV<T>)
			{
				Memcpy(Dest, Source, sizeof(T) * Count);
			}
			else
			{
				for (size_t Index = 0; Index < Count; ++Index)
					Dest[Index] = Source[Index];
			}
		}
		
		static void Copy(T * Dest, const T * Source)
		{
			Copy(Dest, Source, 1);
		}

		static void Copy(T * Dest, const T & Source)
		{
			Copy(Dest, Source, 1);
		}
	};
}