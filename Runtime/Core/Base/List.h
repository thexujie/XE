#pragma once

#include "Types.h"

namespace XE
{
	template<typename T>
	class TListNode
	{
	public:
		TListNode() = default;
		TListNode(const T & Value_) : Value(Value_) {}
		TListNode(T && Value_) : Value(Value_) {}
		
		T Value;
		TListNode * Next = nullptr;
	};

	template<typename T, bool Const>
	class TListNodeIterator
	{
		using ListNodeType = ConditionalT<Const, TListNode<T>, const TListNode<T>>;

		TListNodeIterator() = default;
		TListNodeIterator(ListNodeType * ListNode_) : ListNode(ListNode_) {}
		
		TListNodeIterator & operator ++(int)
		{
			if (ListNode)
				ListNode = ListNode->Next;
			return *this;
		}

		T & operator *() { return ListNode->Value; }
		const T & operator *() const { return ListNode->Value; }

		T * operator->() { return &(ListNode->Value); }
		const T * operator->() const { return &(ListNode->Value); }
		
	public:
		ListNodeType * ListNode = nullptr;
	};
	
	
	template<typename T>
	class TList
	{
		using ListNodeT = TListNode<T>;
		using Iterator = TListNodeIterator<T, false>;
		using ConstIterator = TListNodeIterator<T, true>;
		
	public:
		TList() = default;
		
		Iterator Add(const T & Value)
		{
			if (!Root)
			{
				Root = TAllocator<ListNodeT>::New();
				return Root;
			}
			else
			{
				ListNodeT * Curr = Root;
				while (Curr->Next)
					Curr = Curr->Next;

				Curr->Next = TAllocator<ListNodeT>::New();
				return Curr->Next;
			}
		}

		Iterator Begin() { return Root; }
		ConstIterator Begin() const { return Root; }
		Iterator End() { return nullptr; }
		ConstIterator End() const { return nullptr; }

	public:
		ListNodeT * Root = nullptr;
	};
}

namespace std
{
	template<typename T>
	auto begin(XE::TList<T> & Vector)
	{
		return Vector.Begin();
	}

	template<typename T>
	auto end(XE::TList<T> & Vector)
	{
		return Vector.End();
	}

	template<typename T>
	auto begin(const XE::TList<T> & Vector)
	{
		return Vector.Begin();
	}

	template<typename T>
	auto end(const XE::TList<T> & Vector)
	{
		return Vector.End();
	}
}
