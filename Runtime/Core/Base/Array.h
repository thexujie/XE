#pragma once

#include "Types.h"
#include "Allcoator.h"

namespace XE
{
	template<typename T>
	size_t AsIndex(const T & Value)
	{
		return static_cast<size_t>(Value);
	}

	template<typename T>
	void ArrayCopy(T * Dest, const T * Src, size_t Count)
	{
		for (size_t Index = 0; Index < Count; ++Index)
			Dest[Index] = Src[Index];
	}

	template<typename T, size_t Size>
	void ArrayCopy(T(&Dest)[Size], const T(&Src)[Size])
	{
		for (size_t Index = 0; Index < Size; ++Index)
			Dest[Index] = Src[Index];
	}
	
	template<typename T, size_t ArraySize, EInitializeMode InitializeMode = EInitializeMode::None>
	class TArray
	{
	public:
		using AllocatorType = TAllocator<T>;
		
	public:
		T Data[ArraySize];
		static const size_t Size = ArraySize;
		
	public:
		TArray()
		{
			AllocatorType::Construct(Data, ArraySize, InitializeMode);
		}
		~TArray() = default;
		TArray(const TArray &) = default;
		TArray(TArray &&) = default;
		TArray(TInitializerList<T> InitList)
		{
			size_t Count = Min(ArraySize, InitList.size());
			for (size_t Index = 0; Index < Count; ++Index)
				Data[Index] = InitList.begin()[Index];
		}
		TArray(EInitializeMode InitializeModeIn)
		{
			AllocatorType::Construct(Data, ArraySize, InitializeModeIn);
		}
		
		TArray & operator = (const TArray &) = default;

		TArray(const T Source[])
		{
			AllocatorType::Construct(Data, ArraySize, Source);
		}

		FORCEINLINE size_t GetSize() const { return ArraySize; }
		FORCEINLINE T * GetData() const { return Data; }

		TArray & operator=(TView<T> Another)
		{
			ArrayCopy(Data, Another.Data, Min(Size, Another.Size));
			return *this;
		}
		operator TView<T>() const
		{
			return TVectorView<T>(Data, Size);
		}
		
		T & operator[](size_t Index) { return Data[Index]; }
		const T & operator[](size_t Index) const { return Data[Index]; }
		
		const T * Begin() const { return Data; }
		const T * End() const { return Data + ArraySize; }
		T * Begin() { return Data; }
		T * End() { return Data + ArraySize; }
	};

	template<typename T, size_t ArraySize, EInitializeMode InitializeMode>
	struct THash<TArray<T, ArraySize, InitializeMode>>
	{
		uintx_t operator()(const TArray<T, ArraySize, InitializeMode> & Array) const noexcept
		{
			uintx_t HashValue = HashJoin(Array.GetSize());
			for (size_t Index = 0; Index < Array.GetSize(); ++Index)
				HashValue = DEF_BASE_X * HashValue ^ HashJoin(Array[Index]);
			return HashValue;
		}
	};
}

namespace std
{
	template<typename T, size_t Size>
	auto begin(XE::TArray<T, Size> & Array)
	{
		return Array.Begin();
	}
	
	template<typename T, size_t Size>
	auto begin(const XE::TArray<T, Size> & Array)
	{
		return Array.Begin();
	}

	template<typename T, size_t Size>
	auto end(XE::TArray<T, Size> & Array)
	{
		return Array.End();
	}

	template<typename T, size_t Size>
	auto end(const XE::TArray<T, Size> & Array)
	{
		return Array.End();
	}
}