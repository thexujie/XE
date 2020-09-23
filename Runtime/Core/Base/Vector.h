#pragma once

#include "Array.h"
#include "Archive.h"
#include "Enumerable.h"

namespace XE
{
	template<typename T>
	class TVector
	{
		using AllocatorT = TAllocator<T>;

	public:
		T * Data = nullptr;
		size_t Size : 32 = 0;
		size_t Capacity : 32 = 0;

	public:
		TVector() = default;

		TVector(TEnumerable<T> && Enumerable)
		{
			for (const T & Element : Enumerable)
				Emplace(Element);
		}

		template<typename T2>
		TVector(TEnumerable<T2> && Enumerable)
		{
			for (const T2 & Element : Enumerable)
				Emplace(Element);
		}

		explicit TVector(size_t SizeIn, size_t CapacityIn = 0, EInitializeMode InitializeMode = EInitializeMode::Default)
		{
			if (CapacityIn > 0)
			{
				Capacity = CapacityIn;
				Data = AllocatorT::Alloc(CapacityIn);
			}

			if (SizeIn > 0)
				AddInitialized(SizeIn, InitializeMode);
		}

		TVector(const TVector & Array)
		{
			Size = Array.Size;
			Capacity = Array.Capacity;
			Data = Capacity > 0 ? AllocatorT::Alloc(Capacity) : nullptr;
			AllocatorT::Construct(Data, Array.Data, Size);
		}

		TVector(TVector && That) noexcept
		{
			Size = That.Size;
			Capacity = That.Capacity;
			Data = That.Data;
			That.Size = 0;
			That.Capacity = 0;
			That.Data = nullptr;
		}

		explicit TVector(TView<T> Array)
		{
			Size = Array.Size;
			Capacity = Array.Size;
			Data = Capacity > 0 ? AllocatorT::Alloc(Capacity) : nullptr;
			AllocatorT::Construct(Data, Array.Data, Size);
		}

		TVector(TInitializerList<T> InitList)
		{
			Size = static_cast<size_t>(InitList.size());
			Capacity = Size;
			Data = Capacity > 0 ? AllocatorT::Alloc(Capacity) : nullptr;
			AllocatorT::Construct(Data, InitList.begin(), Size);
		}

		~TVector()
		{
			if (Data)
			{
				AllocatorT::Destruct(Data, Size);
				AllocatorT::Free(Data);
				Size = 0;
				Capacity = 0;
				Data = nullptr;
			}
		}

		TVector(const T * Source, size_t Count)
		{
			Size = Count;
			Capacity = Count;
			Data = Capacity > 0 ? AllocatorT::Alloc(Capacity) : nullptr;
			AllocatorT::Construct(Data, Source, Size);
		}

		auto operator<=>(const TVector & Another) const noexcept
		{
			if (auto Result = Compare(Size, Another.Size); Result != 0)
				return Result;

			for (size_t Index = 0; Index < Size; ++Index)
			{
				if (auto Result = Compare(Data[Index], Another.Data[Index]); Result != 0)
					return Result;
			}
			return Compare(0, 0);
		}
		
		FORCEINLINE bool IsEmpty() const { return Size == 0; }
		FORCEINLINE size_t GetSize() const { return Size; }
		FORCEINLINE T * GetData() const { return Data; }

		T & operator[](size_t Index) { return Data[Index]; }
		const T & operator[](size_t Index) const { return Data[Index]; }
		
		FORCEINLINE const T & GetAt(size_t Index) const
		{
			if (Index >= Size)
				throw EError::OutOfBound;
			return Data[Index];
		}
		FORCEINLINE T & GetAt(size_t Index)
		{
			if (Index >= Size)
				throw EError::OutOfBound;
			return Data[Index];
		}
		FORCEINLINE const T & Front() const { return GetAt(0); }
		FORCEINLINE T & Front() { return GetAt(0); }
		FORCEINLINE const T & Back() const { return GetAt(Size - 1); }
		FORCEINLINE T & Back() { return GetAt(Size - 1); }

		T * AddInitialized(size_t Count, EInitializeMode InitializeMode = EInitializeMode::Default)
		{
			if (!Count)
				return nullptr;

			size_t Index = Size;
			Size = Size + Count;
			if (Size > Capacity)
			{
				size_t NewCapacity = Capacity > 0 ? Capacity : 1;
				while (NewCapacity < Size)
					NewCapacity *= 2;
				T * NewData = AllocatorT::Alloc(NewCapacity);
				AllocatorT::MoveCopy(NewData, Data, Index);
				AllocatorT::Destruct(Data, Index);
				AllocatorT::Free(Data);
				AllocatorT::Construct(NewData + Index, Count, InitializeMode);
				Data = NewData;
				Capacity = NewCapacity;
			}
			else
			{
				AllocatorT::Construct(Data + Index, Count, InitializeMode);
			}
			return Data + Index;
		}

		T * AddUninitialized(size_t Count)
		{
			return AddInitialized(Count, EInitializeMode::None);
		}

		void Reduce(size_t Count)
		{
			Count = Min(Size, Count);
			if (!Count)
				return;
			AllocatorT::Destruct(Data + Size - Count, Count);
			Size = Size - Count;
		}

		template<typename... ArgsT>
		T & Emplace(ArgsT &&... Args)
		{
			size_t Index = Size;
			AddUninitialized(1);
			AllocatorT::PlaceNew(Data + Index, Forward<ArgsT>(Args)...);
			return Data[Index];
		}

		T & Add(const T & Element)
		{
			return Emplace(Element);
		}

		T * Add(const T & Element, size_t Count, EInitializeMode InitializeMode = EInitializeMode::Default)
		{
			size_t Index = Size;
			AddUninitialized(Count);
			AllocatorT::Construct(Data + Index, Element, Count, InitializeMode);
			return Data + Index;
		}

		T * Add(const T * Source, size_t Count, EInitializeMode InitializeMode = EInitializeMode::Default)
		{
			size_t Index = Size;
			AddUninitialized(Count);
			AllocatorT::Construct(Data + Index, Source, Count, InitializeMode);
			return Data + Index;
		}

		T * Add(TView<T> Another, EInitializeMode InitializeMode = EInitializeMode::Default)
		{
			size_t Index = Size;
			AddUninitialized(Another.Size);
			AllocatorT::Construct(Data + Index, Another.Data, Another.Size, InitializeMode);
			return Data + Index;
		}

		T * Add(TInitializerList<T> InitList)
		{
			return Add(InitList.begin(), size_t(InitList.size()), EInitializeMode::Default);
		}

		TVector & operator =(const TVector & That)
		{
			if (That.Data == Data)
				return *this;
			
			AllocatorT::Destruct(Data, Size);
			AllocatorT::Free(Data);

			Size = That.Size;
			Capacity = That.Capacity;
			Data = Capacity > 0 ? AllocatorT::Alloc(That.Capacity) : nullptr;
			AllocatorT::Construct(Data, That.Data, That.Size);
			return *this;
		}

		TVector & operator =(TVector && That) noexcept
		{
			if (That.Data == Data)
				return *this;

			AllocatorT::Destruct(Data, Size);
			AllocatorT::Free(Data);
			
			Size = That.Size;
			Capacity = That.Capacity;
			Data = That.Data;
			That.Size = 0;
			That.Capacity = 0;
			That.Data = nullptr;
			return *this;
		}

		TVector & operator =(TInitializerList<T> InitList)
		{
			AllocatorT::Destruct(Data, Size);
			AllocatorT::Free(Data);

			Size = InitList.size();
			Capacity = Size;
			Data = Capacity > 0 ? AllocatorT::Alloc(Capacity) : nullptr;
			AllocatorT::Construct(Data, InitList.begin(), Size);
			return *this;
		}
		
		TVector & operator=(TView<T> That)
		{
			AllocatorT::Destruct(Data, Size);
			AllocatorT::Free(Data);

			Size = That.Size;
			Capacity = That.Size;
			Data = Capacity > 0 ? AllocatorT::Alloc(Capacity) : nullptr;
			AllocatorT::Construct(Data, That.Data, That.Size);
			return *this;
		}

		TVector & operator=(TEnumerable<T> && Enumerable)
		{
			AllocatorT::Destruct(Data, Size);
			AllocatorT::Free(Data);

			for (const T & Element : Enumerable)
				Emplace(Move(Element));
			
			return *this;
		}
		

		void Clear()
		{
			Resize(0);
		}
		
		void Reset()
		{
			if (Data)
			{
				AllocatorT::Destruct(Data, Size);
				AllocatorT::Free(Data);
				Size = 0;
				Capacity = 0;
				Data = nullptr;
			}
		}

		void Resize(size_t NewSize, size_t NewCapacity, EInitializeMode InitializeMode)
		{
			if (NewCapacity > NewSize)
			{
				T * NewData = AllocatorT::Alloc(NewCapacity);
				AllocatorT::MoveCopy(NewData, Data, Size);
				AllocatorT::Destruct(Data, Size);
				AllocatorT::Free(Data);
				Data = NewData;
				Capacity = NewCapacity;
			}

			if (NewSize > Size)
				AddInitialized(NewSize - Size, InitializeMode);
			else if (NewSize < Size)
				Reduce(Size - NewSize);
			else {}
		}

		void Resize(size_t NewSize, size_t NewCapacity, const T & Value, EInitializeMode InitializeMode)
		{
			if (NewCapacity > NewSize)
			{
				T * NewData = AllocatorT::Alloc(NewCapacity);
				AllocatorT::MoveCopy(NewData, Data, Size);
				AllocatorT::Destruct(Data, Size);
				AllocatorT::Free(Data);
				Data = NewData;
				Capacity = NewCapacity;
			}

			size_t OldSize = Size;
			if (NewSize > Size)
				Add(Value, NewSize - Size, InitializeMode);
			else if (NewSize < Size)
				Reduce(Size - NewSize);
			else {}
			AllocatorT::Copy(Data, Value, OldSize);
		}
		
		void Resize(size_t NewSize)
		{
			Resize(NewSize, 0, EInitializeMode::Default);
		}

		void Resize(size_t NewSize, const T & Value)
		{
			Resize(NewSize, 0, Value, EInitializeMode::Default);
		}

		void Reserve(size_t NewCapacity)
		{
			if (NewCapacity > Capacity)
			{
				T * NewData = AllocatorT::Alloc(NewCapacity);
				AllocatorT::MoveCopy(NewData, Data, Size);
				AllocatorT::Destruct(Data, Size);
				AllocatorT::Free(Data);
				Data = NewData;
				Capacity = NewCapacity;
			}
		}

		void Erase(const T * Item, size_t Count = 1)
		{
			EraseAt(Item - Data, Count);
		}

		void EraseAt(size_t Index, size_t Count = 1)
		{
			if (Index + Count >= Size)
				throw EError::OutOfBound;

			AllocatorT::Destruct(Data + Index, Count);
			AllocatorT::MoveCopy(Data + Index, Data + Index + Count, Size - Count - Index);
			Size = Size - Count;
		}

		T * Find(const T & Value) requires EqualAble<T>
		{
			for (size_t Index = 0; Index < Size; ++Index)
			{
				if (Data[Index] == Value)
					return Data + Index;
			}
			return nullptr;
		}

		template<typename ComparerT>
		T * FindBy(const ComparerT & Comparer)
		{
			for (size_t Index = 0; Index < Size; ++Index)
			{
				if (Comparer(Data[Index]))
					return Data + Index;
			}
			return nullptr;
		}

		operator TView<T>() const
		{
			return TView<T>(Data, Size);
		}

		friend FArchive & operator >>(FArchive & Archive, TVector & Vector)
		{
			if (Archive.IsInput())
			{
				uint32_t Length = Archive.Read<uint32_t>();
				Vector.Resize(Length, Length, EInitializeMode::None);
			}
			else
			{
				Archive.Write(uint32_t(Vector.GetSize()));
			}

			if constexpr (IsTriviallyConstructibleV<T>)
			{
				if (Archive.IsInput())
					Archive.Read(Vector.GetData(), Vector.GetSize() * sizeof(T));
				else
					Archive.Write(Vector.GetData(), Vector.GetSize() * sizeof(T));
			}
			else
			{
				for (uint32_t Index = 0; Index < Vector.GetSize(); ++Index)
					Archive >> Vector[Index];
			}
			return Archive;
		}

		T * Begin() const { return Data; }
		T * End() const { return Data + Size; }
	};

	template<typename T>
	struct IsVector : FalseType {};

	template<typename T>
	struct IsVector<TVector<T>> : TrueType { using Type = T; };

	template<typename T>
	inline constexpr bool IsVectorV = IsVector<T>::Value;

	template<typename T>
	struct THash<TVector<T>>
	{
		uintx_t operator()(const TVector<T> & Vector) const noexcept
		{
			uintx_t HashValue = HashJoin(Vector.GetSize());
			for (size_t Index = 0; Index < Vector.GetSize(); ++Index)
				HashValue = DEF_BASE_X * HashValue ^ HashJoin(Vector[Index]);
			return HashValue;
		}
	};

	template<typename T>
	struct THash<TView<T>>
	{
		uintx_t operator()(const TView<T> & VectorView) const noexcept
		{
			uintx_t HashValue = HashJoin(VectorView.Data);
			HashValue = DEF_BASE_X * HashValue ^ HashJoin(VectorView.Size);
			return HashValue;
		}
	};
}

namespace std
{
	template<typename T>
	auto begin(XE::TVector<T> & Vector)
	{
		return Vector.Begin();
	}

	template<typename T>
	auto end(XE::TVector<T> & Vector)
	{
		return Vector.End();
	}
	
	template<typename T>
	auto begin(const XE::TVector<T> & Vector)
	{
		return Vector.Begin();
	}

	template<typename T>
	auto end(const XE::TVector<T> & Vector)
	{
		return Vector.End();
	}
}