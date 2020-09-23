#pragma once

#include "Types.h"

namespace XE
{
	class CORE_API FReferencer
	{
	public:
		FReferencer() = default;
		virtual ~FReferencer() = default;
		virtual void DestroyPointer() = 0;
		
		int32_t SharedReferenceCount = 1;
		int32_t WeakReferenceCount = 1;
	};
	
	template <typename T, typename DeleterT>
	class TSharedReferencer : public FReferencer
	{
	public:
		TSharedReferencer(T * PointerIn, DeleterT && Deleter)
			: Deleter(Move(Deleter))
			, Pointer(PointerIn)
		{
		}

		void DestroyPointer() override
		{
			Deleter(Pointer);
		}

		// Non-copyable
		TSharedReferencer(const TSharedReferencer &) = delete;
		TSharedReferencer & operator=(const TSharedReferencer &) = delete;

	private:
		DeleterT Deleter;
		T * Pointer;
	};

	class CORE_API FSharedController
	{
	public:
		FSharedController() = default;
		FSharedController(FReferencer * SharedReferencerIn) : Referencer(SharedReferencerIn) {}
		FSharedController(const FSharedController & SharedControllerIn) : Referencer(SharedControllerIn.Referencer)
		{
			AddSharedReference();
		}
		FSharedController(FSharedController && SharedControllerIn) noexcept : Referencer(SharedControllerIn.Referencer)
		{
			SharedControllerIn.Referencer = nullptr;
		}

		int32_t GetSharedReferenceCount() const { return Referencer ? Referencer->SharedReferenceCount : 0; }
		int32_t GetWeakReferenceCount() const { return Referencer ? Referencer->WeakReferenceCount : 0; }
		bool IsUnique() const
		{
			return GetSharedReferenceCount() == 1;
		}
		
		~FSharedController()
		{
			ReleaseSharedReference();
		}

		void AddSharedReference()
		{
			if (Referencer)
				Atomics::IncFetch(Referencer->SharedReferenceCount);
		}
		
		void ReleaseSharedReference()
		{
			if (Referencer)
			{
				assert(Referencer->SharedReferenceCount > 0);
				if (Atomics::DecFetch(Referencer->SharedReferenceCount) == 0)
				{
					Referencer->DestroyPointer();
					ReleaseWeakReference();
				}
				Referencer = nullptr;
			}
		}

		void ReleaseWeakReference()
		{
			if (Referencer)
			{
				assert(Referencer->WeakReferenceCount > 0);
				if (Atomics::DecFetch(Referencer->WeakReferenceCount) == 0)
				{
					delete Referencer;
					Referencer = nullptr;
				}
			}
		}

		void Reset()
		{
			if (Referencer)
			{
				ReleaseSharedReference();
				Referencer = nullptr;
			}
		}

		FSharedController & operator = (const FSharedController & SharedControllerIn)
		{
			if (Referencer != SharedControllerIn.Referencer)
			{
				ReleaseSharedReference();
				Referencer = SharedControllerIn.Referencer;
				if (Referencer)
					AddSharedReference();
			}
			return *this;
		}

		FReferencer * GetReferencer() const { return Referencer; }
		
	public:
		FReferencer * Referencer = nullptr;
	};
	
	template<typename T>
	class TSharedPtr
	{
		template<typename ThatT> friend class TSharedPtr;

	public:
		TSharedPtr() = default;
		TSharedPtr(std::nullptr_t) {}
		explicit TSharedPtr(T * ObjectIn) : Pointer(ObjectIn), Controller(new TSharedReferencer(ObjectIn, TDeleterDefault<T>())) {}
		
		TSharedPtr(const TSharedPtr & That) : Pointer(That.Pointer), Controller(That.Controller) {}
		template<typename ThatT, typename = EnableIfT<IsConvertibleToV<ThatT *, T *>>>
		TSharedPtr(const TSharedPtr<ThatT> & That) : Pointer(That.Pointer), Controller(That.Controller) {}
		
		TSharedPtr(TSharedPtr && That) noexcept : Pointer(That.Pointer), Controller(That.Controller)
		{
			That.Pointer = nullptr;
			That.Controller.Reset();
		}

		template<typename ThatT, typename = EnableIfT<IsConvertibleToV<ThatT *, T *>>>
		TSharedPtr(TSharedPtr<ThatT> && That) noexcept : Pointer(That.Pointer), Controller(That.Controller)
		{
			That.Pointer = nullptr;
			That.Controller.Reset();
		}
		
		template<typename DeleterT>
		TSharedPtr(T * ObjectIn, DeleterT && Deleter) : Pointer(ObjectIn), Controller(new TSharedReferencer(Pointer, Move(Deleter))) {}
		TSharedPtr(T * ObjectIn, FReferencer * ReferencerIn) : Pointer(ObjectIn), Controller(ReferencerIn)
		{
			Controller.AddSharedReference();
		}
		~TSharedPtr() = default;

		FORCEINLINE bool IsValid() const
		{
			return Pointer != nullptr;
		}
		FORCEINLINE T * Get() const { return Pointer; }
		FORCEINLINE T & Value() const { return *Pointer; }

		FORCEINLINE void Reset()
		{
			Pointer = nullptr;
			Controller = nullptr;
		}

		FORCEINLINE FReferencer * GetReferencer() const { return Controller.GetReferencer(); }
		FORCEINLINE int32_t GetSharedReferenceCount() const { return Controller.GetSharedReferenceCount(); }
		FORCEINLINE int32_t GetWeakReferenceCount() const { return Controller.GetWeakReferenceCount(); }
		
		FORCEINLINE T & operator*() const
		{
			assert(IsValid());
			return *Pointer;
		}

		FORCEINLINE T * operator->() const
		{
			assert(IsValid());
			return Pointer;
		}

		FORCEINLINE TSharedPtr & operator = (std::nullptr_t)
		{
			Reset();
			return *this;
		}
		
		FORCEINLINE TSharedPtr & operator = (const TSharedPtr & SharedPtrIn)
		{
			Pointer = SharedPtrIn.Pointer;
			Controller = SharedPtrIn.Controller;
			return *this;
		}

		T & operator[](size_t Index) { return Pointer[Index]; }
		const T & operator[](size_t Index) const { return Pointer[Index]; }
		explicit operator bool() const { return !!Pointer; }
		
		FORCEINLINE FSharedController & GetController() { return Controller; }
		FORCEINLINE const FSharedController & GetController() const { return Controller; }
		
	private:
		T * Pointer = nullptr;
		FSharedController Controller;
	};

	template <typename T, typename... ArgsT>
	FORCEINLINE TSharedPtr<T> MakeShared(ArgsT &&... Args) requires NegationV<IsArray<T>>
	{
		return TSharedPtr<T>(new T(Forward<ArgsT>(Args)...));
	}

	template<typename T2, typename T>
	TSharedPtr<T2> StaticPointerCast(const TSharedPtr<T> & Pointer)
	{
		return TSharedPtr<T2>(static_cast<T2 *>(Pointer.Get()), Pointer.GetReferencer());
	}

	template<typename T2, typename T>
	TSharedPtr<T2> ReinterpretPointerCast(const TSharedPtr<T> & Pointer)
	{
		return TSharedPtr<T2>(reinterpret_cast<T2 *>(Pointer.Get()), Pointer.GetReferencer());
	}
	
	class CORE_API FWeakController
	{
	public:
		FWeakController() = default;
		FWeakController(FReferencer * ReferencerIn) : Referencer(ReferencerIn)
		{
			if (ReferencerIn)
				AddWeakReference();
		}
		FWeakController(const FWeakController & WeakControllerIn) : Referencer(WeakControllerIn.Referencer)
		{
			if (WeakControllerIn.Referencer)
				AddWeakReference();
		}
		FWeakController(FWeakController && SharedControllerIn) noexcept : Referencer(SharedControllerIn.Referencer)
		{
			SharedControllerIn.Referencer = nullptr;
		}
		FReferencer * GetReferencer() const { return Referencer; }
		
		int32_t GetSharedReferenceCount() const { return Referencer ? Referencer->SharedReferenceCount : 0; }
		int32_t GetWeakReferenceCount() const { return Referencer ? Referencer->WeakReferenceCount : 0; }

		~FWeakController()
		{
			ReleaseWeakReference();
		}

		void AddWeakReference()
		{
			assert(Referencer != nullptr);
			Atomics::IncFetch(Referencer->WeakReferenceCount);
		}

		void ReleaseWeakReference()
		{
			if (Referencer)
			{
				assert(Referencer->WeakReferenceCount > 0);
				if (Atomics::DecFetch(Referencer->WeakReferenceCount) == 0)
				{
					delete Referencer;
					Referencer = nullptr;
				}
			}
		}

		void Reset()
		{
			if (Referencer)
			{
				ReleaseWeakReference();
				Referencer = nullptr;
			}
		}

		FWeakController & operator = (const FWeakController & SharedControllerIn)
		{
			if (Referencer != SharedControllerIn.Referencer)
			{
				ReleaseWeakReference();
				Referencer = SharedControllerIn.Referencer;
				if (Referencer)
					AddWeakReference();
			}
			return *this;
		}
	private:
		FReferencer * Referencer = nullptr;
	};

	template<typename T>
	class TWeakPtr
	{
	public:
		TWeakPtr() = default;
		FORCEINLINE TWeakPtr(const TWeakPtr & WeakPtrIn)
			: Pointer(WeakPtrIn.Pointer)
			, Controller(WeakPtrIn.Controller)
		{ }

		FORCEINLINE TWeakPtr(TWeakPtr && WeakPtrIn) noexcept
			: Pointer(WeakPtrIn.Pointer)
			, Controller(Move(WeakPtrIn.Controller))
		{
			WeakPtrIn.Pointer = nullptr;
		}

		FORCEINLINE TWeakPtr(const TSharedPtr<T> & SharedPtrIn)
			: Pointer(SharedPtrIn.Get())
			, Controller(SharedPtrIn.GetController().GetReferencer())
		{
			
		}
		
		void Reset()
		{
			Pointer = nullptr;
			Controller = nullptr;
		}

		TSharedPtr<T> Lock() const
		{
			return TSharedPtr<T>(Pointer, Controller.GetReferencer());
		}

		TWeakPtr & operator = (std::nullptr_t) { Reset(); return *this; }
		TWeakPtr & operator = (const TWeakPtr & WeakPtrIn)
		{
			Reset();
			Pointer = WeakPtrIn.Pointer;
			Controller = WeakPtrIn.Controller;
			return *this;
		}
		TWeakPtr & operator = (const TSharedPtr<T> & SharedPtrIn)
		{
			Reset();
			Pointer = SharedPtrIn.Get();
			Controller = FWeakController(SharedPtrIn.GetController().GetReferencer());
			return *this;
		}
		
	private:
		T * Pointer = nullptr;
		FWeakController Controller;
	};
}