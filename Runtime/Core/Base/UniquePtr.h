#pragma once

#include "Types.h"
#include "Pointer.h"

namespace XE
{
	template<typename T, typename DeleterT = TDeleterDefault<T>>
	class TUniquePtr
	{
		template <typename ThatT, typename ThatDeleterT>
		friend class TUniquePtr;
		
	private:
		T * Pointer = nullptr;
		DeleterT Deleter;

	public:
		FORCEINLINE TUniquePtr() = default;
		TUniquePtr(const TUniquePtr &) = delete;
		TUniquePtr & operator = (const TUniquePtr &) = delete;
		FORCEINLINE TUniquePtr(TUniquePtr && That) noexcept : Pointer(That.Pointer), Deleter(Move(That.Deleter))
		{
			That.Pointer = nullptr;
		}

		FORCEINLINE TUniquePtr(T * PointerIn, DeleterT DeleterIn = DeleterT()) : Pointer(PointerIn), Deleter(DeleterIn) { }
		TUniquePtr(NullPtrT) : Pointer(nullptr) { }
		FORCEINLINE TUniquePtr & operator = (NullPtrT) noexcept
		{
			T * OldPointer = Pointer;
			Pointer = nullptr;
			Deleter(OldPointer);
			return *this;
		}
		
		template <typename ThatT, typename ThatDeleterT>
		FORCEINLINE TUniquePtr(TUniquePtr<ThatT, ThatDeleterT> && That) noexcept
			: Pointer(That.Data), Deleter(Move(That.Deleter))
		{
			That.Data = nullptr;
		}
		
		~TUniquePtr()
		{
			Deleter(Pointer);
		}
		
		FORCEINLINE T * Get() const { return Pointer; }
		DeleterT & GetDeleter() { return Deleter; }
		const DeleterT & GetDeleter() const { return Deleter; }
		FORCEINLINE bool IsValid() const { return Pointer != nullptr; }
		
		FORCEINLINE T * Release()
		{
			T * Result = Pointer;
			Pointer = nullptr;
			return Result;
		}
		
		FORCEINLINE void Reset(T * PointerIn = nullptr)
		{
			T * OldPointer = Pointer;
			Pointer = PointerIn;
			Deleter(OldPointer);
		}

		FORCEINLINE explicit operator bool() const { return !!Pointer; }
		FORCEINLINE T & operator*() const { return *Pointer; }
		FORCEINLINE T * operator->() const { return Pointer; }

		FORCEINLINE TUniquePtr & operator=(TUniquePtr && That) noexcept
		{
			if (this != &That)
			{
				T * OldPointer = Pointer;
				Pointer = That.Pointer;
				That.Pointer = nullptr;
				Deleter(OldPointer);
				Deleter = Move(That.Deleter);
			}
			return *this;
		}
		
		template <typename ThatT, typename ThatDeleterT>
		FORCEINLINE TUniquePtr operator=(TUniquePtr<ThatT, ThatDeleterT> && That)
		{
			T * OldPointer = Pointer;
			Pointer = That.Data;
			That.Data = nullptr;
			Deleter(OldPointer);
			Deleter = Move(That.Deleter);
			return *this;
		}
	};


	template<typename T, typename DeleterT>
	class TUniquePtr<T[], DeleterT>
	{
		template <typename ThatT, typename ThatDeleterT>
		friend class TUniquePtr;
		
	private:
		T * Pointer = nullptr;
		DeleterT Deleter;

	public:
		TUniquePtr(const TUniquePtr &) = delete;
		TUniquePtr & operator = (const TUniquePtr &) = delete;

	public:
		FORCEINLINE TUniquePtr() : Pointer(nullptr)
		{
		}
		
		TUniquePtr(std::nullptr_t) : Pointer(nullptr)
		{
		}

		template<typename ThatT, typename = EnableIfT<IsConvertibleToV<ThatT, T>>>
		FORCEINLINE TUniquePtr(ThatT * PointerIn) : Pointer(PointerIn)
		{
		}

		FORCEINLINE TUniquePtr(TUniquePtr && That) noexcept
			: Pointer(That.Pointer), Deleter(Move(That.Deleter))
		{
			That.Pointer = nullptr;
		}

		template <typename ThatT, typename ThatDeleterT>
		FORCEINLINE TUniquePtr(TUniquePtr<ThatT, ThatDeleterT> && That) noexcept
			: Pointer(That.Data), Deleter(Move(That.Deleter))
		{
			That.Data = nullptr;
		}

		~TUniquePtr()
		{
			Deleter(Pointer);
		}

		FORCEINLINE T * Get() const { return Pointer; }
		DeleterT & GetDeleter() { return Deleter; }
		const DeleterT & GetDeleter() const { return Deleter; }
		FORCEINLINE bool IsValid() const { return Pointer != nullptr; }

		FORCEINLINE T * Release()
		{
			T * Result = Pointer;
			Pointer = nullptr;
			return Result;
		}

		FORCEINLINE void Reset(T * PointerIn = nullptr)
		{
			T * OldPointer = Pointer;
			Pointer = PointerIn;
			Deleter(OldPointer);
		}

		FORCEINLINE T & operator*() const { return *Pointer; }
		FORCEINLINE T * operator->() const { return Pointer; }

		FORCEINLINE TUniquePtr & operator = (std::nullptr_t)
		{
			T * OldPointer = Pointer;
			Pointer = nullptr;
			Deleter(OldPointer);
			return *this;
		}

		FORCEINLINE TUniquePtr & operator=(TUniquePtr && That) noexcept
		{
			if (this != &That)
			{
				T * OldPointer = Pointer;
				Pointer = That.Pointer;
				That.Pointer = nullptr;
				Deleter(OldPointer);
				Deleter = Move(That.Deleter);
			}
			return *this;
		}

		template <typename ThatT, typename ThatDeleterT>
		FORCEINLINE TUniquePtr operator=(TUniquePtr<ThatT, ThatDeleterT> && That)
		{
			T * OldPointer = Pointer;
			Pointer = That.Data;
			That.Data = nullptr;
			Deleter(OldPointer);
			Deleter = Move(That.Deleter);
			return *this;
		}

		FORCEINLINE T & operator[](size_t Index) const
		{
			return Pointer[Index];
		}
	};

	template <typename LhsT, typename RhsT>
	FORCEINLINE bool operator==(const TUniquePtr<LhsT> & Lhs, const TUniquePtr<RhsT> & Rhs)
	{
		return Lhs.Get() == Rhs.Get();
	}
	
	template <typename T>
	FORCEINLINE bool operator==(const TUniquePtr<T> & Lhs, const TUniquePtr<T> & Rhs)
	{
		return Lhs.Get() == Rhs.Get();
	}
	
	template <typename LhsT, typename RhsT>
	FORCEINLINE bool operator!=(const TUniquePtr<LhsT> & Lhs, const TUniquePtr<RhsT> & Rhs)
	{
		return Lhs.Get() != Rhs.Get();
	}
	
	template <typename T>
	FORCEINLINE bool operator!=(const TUniquePtr<T> & Lhs, const TUniquePtr<T> & Rhs)
	{
		return Lhs.Get() != Rhs.Get();
	}
	
	template <typename T>
	FORCEINLINE bool operator==(const TUniquePtr<T> & Lhs, std::nullptr_t)
	{
		return !Lhs.IsValid();
	}
	
	template <typename T>
	FORCEINLINE bool operator==(std::nullptr_t, const TUniquePtr<T> & Rhs)
	{
		return !Rhs.IsValid();
	}
	
	template <typename T>
	FORCEINLINE bool operator!=(const TUniquePtr<T> & Lhs, std::nullptr_t)
	{
		return Lhs.IsValid();
	}
	
	template <typename T>
	FORCEINLINE bool operator!=(std::nullptr_t, const TUniquePtr<T> & Rhs)
	{
		return Rhs.IsValid();
	}

	template <typename T, typename... ArgsT>
	FORCEINLINE EnableIfT<!IsArrayV<T>, TUniquePtr<T>> MakeUnique(ArgsT &&... Args)
	{
		return TUniquePtr<T, TDeleterDefault<T>>(new T(Forward<ArgsT>(Args)...), TDeleterDefault<T>());
	}

	template <typename T>
	FORCEINLINE EnableIfT<IsArrayV<T>, TUniquePtr<T>> MakeUnique(size_t Size)
	{
		using ThisT = std::remove_all_extents_t<T>;
		return TUniquePtr<T>(new ThisT[Size]());
	}
	
}
