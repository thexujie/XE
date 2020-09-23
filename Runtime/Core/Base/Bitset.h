#pragma once

#include "Types.h"

namespace XE
{
	class CORE_API FBitSet
	{
		using UnitT = uintx_t;
		static constexpr size_t UnitSize = sizeof(UnitT) * 8;
		using AllocatorT = TAllocator<UnitT>;
		
	public:
		FBitSet() = default;
		FBitSet(const FBitSet & That)
		{
			Data = AllocatorT::Alloc(RoundUp(That.Capacity, UnitSize));
			Size = That.Size;
			ArrayCopy(Data, That.Data, RoundUp(Size, UnitSize));
		}
		
		~FBitSet()
		{
			if (Data)
			{
				AllocatorT::Free(Data);
				Data = nullptr;
			}
			Size = 0;
			Capacity = 0;
		}
		
		FBitSet(FBitSet && That) noexcept
		{
			Data = That.Data;
			Size = That.Size;
			Capacity = That.Capacity;
			That.Data = nullptr;
			That.Size = 0;
			That.Capacity = 0;
		}

		FBitSet(size_t NewSize, bool Value)
		{
			Resize(NewSize, Value);
		}
		
		FBitSet(size_t NewSize, size_t NewCapacity, bool Value)
		{
			Resize(NewSize, NewCapacity, Value);
		}

		void Resize(size_t NewSize, bool Value)
		{
			if (NewSize >= Capacity)
			{
				size_t NewCapacity = Capacity ? Capacity : 1;
				while (NewCapacity < NewSize)
					NewCapacity *= 2;

				AllocatorT::Free(Data);
				Data = AllocatorT::Alloc(NewCapacity / UnitSize);
				Capacity = NewCapacity;
			}

			Size = NewSize;
			SetAt(0, Size, Value);
		}

		void Resize(size_t NewSize, size_t NewCapacity, bool Value)
		{
			if (NewCapacity && NewCapacity >= NewSize)
				NewCapacity = RoundUp(NewCapacity, UnitSize) * UnitSize;
			if (NewCapacity && NewCapacity >= NewSize)
			{
				AllocatorT::Free(Data);
				Data = AllocatorT::Alloc(NewCapacity / UnitSize);
				Capacity = NewCapacity;
			}

			Size = NewSize;
			SetAt(0, Size, Value);
		}
		
		size_t GetSize() const { return Size; }
		bool IsEmpty() const { return Size == 0; }

		void Add(bool Value)
		{
			if (Size + 1 >= Capacity)
			{
				size_t NewCapacity = RoundUp(Max<size_t>(Capacity * 2, UnitSize), UnitSize) * UnitSize;
				while (NewCapacity < Size + 1)
					NewCapacity *= 2;

				UnitT * NewData = AllocatorT::Alloc(RoundUp(NewCapacity, UnitSize));
				ArrayCopy(NewData, Data, RoundUp(Size, UnitSize));

				AllocatorT::Free(Data);
				Data = NewData;
				Capacity = NewCapacity;
			}

			size_t Index = Size++;
			SetAt(Index, Value);
		}

		void SetAt(size_t Index, bool Value)
		{
			if (Index >= Size)
				throw EError::OutOfBound;

			UnitT & Unit = Data[Index / UnitSize];
			size_t BitIndex = Index % UnitSize;
			SetBit(Unit, BitIndex, Value);
		}

		void SetAt(size_t Index, size_t Count, bool Value)
		{
			if (Index + Count > Size)
				throw EError::OutOfBound;

			size_t UnitHeadIndex = Index / UnitSize;
			size_t UnitTailIndex = (Index + Count) / UnitSize;

			size_t BitHeadIndex = Index % UnitSize;
			size_t BitTailIndex = (Index + Count) % UnitSize;
			if (UnitHeadIndex == UnitTailIndex)
			{
				UnitT Mask = ~((UnitT(1) << BitHeadIndex) - 1);
				Mask &= ((UnitT(1) << BitTailIndex) - 1);

				if (Value)
					Data[UnitHeadIndex] |= Mask;
				else
					Data[UnitHeadIndex] &= ~Mask;
				return;
			}
			
			if (BitHeadIndex)
			{
				if (Value)
					Data[UnitHeadIndex] |= ~((UnitT(1) << BitHeadIndex) - 1);
				else
					Data[UnitHeadIndex] &= ((UnitT(1) << BitHeadIndex) - 1);
				++UnitHeadIndex;
			}
			
			for (size_t UnitIndex = UnitHeadIndex; UnitIndex < UnitTailIndex; ++UnitIndex)
			{
				Data[UnitIndex] = Value ? UnitT(-1) : 0;
			}

			if (BitTailIndex)
			{
				if (Value)
					Data[UnitTailIndex] |= ((UnitT(1) << BitTailIndex) - 1);
				else
					Data[UnitTailIndex] &= ~((UnitT(1) << BitTailIndex) - 1);
			}
		}

		bool GetAt(size_t Index) const
		{
			UnitT & Unit = Data[Index / UnitSize];
			size_t BitIndex = Index % UnitSize;
			return GetBit(Unit, BitIndex);
		}

		bool operator [](size_t Index) const { return GetAt(Index); }
		
		UnitT * Data = nullptr;
		size_t Size = 0;
		size_t Capacity = 0;
	};
}
