#pragma once

#include "Types.h"
#include "Pair.h"

namespace XE
{
	enum class ERBTreeNodeColor { Red, Black };

	template<typename ItemT>
	struct TRBTreeNode
	{
		TRBTreeNode * Left = nullptr;
		TRBTreeNode * Right = nullptr;
		TRBTreeNode * Parent = nullptr;
		ItemT Item;
		ERBTreeNodeColor Color;

		TRBTreeNode() = default;
		TRBTreeNode(const TRBTreeNode &) = default;
		TRBTreeNode(TRBTreeNode &&) noexcept = default;
		TRBTreeNode & operator=(const TRBTreeNode &) = default;
 
		TRBTreeNode(const ItemT & ItemIn, ERBTreeNodeColor ColorIn = ERBTreeNodeColor::Red)
			: Left(NULL)
			, Right(NULL)
			, Parent(NULL)
			, Item(ItemIn)
			, Color(ColorIn) {}


		TRBTreeNode(ItemT && ItemIn, ERBTreeNodeColor ColorIn = ERBTreeNodeColor::Red)
			: Left(NULL)
			, Right(NULL)
			, Parent(NULL)
			, Item(Move(ItemIn))
			, Color(ColorIn) {}
	};

	template<typename ItemT, bool Const>
	class TRBTreeIterator
	{
		friend class TRBTreeIterator<ItemT, !Const>;
		
	public:
		using NodeType = ConditionalT<Const, AddConstT<TRBTreeNode<ItemT>>, TRBTreeNode<ItemT>>;
		using ItemType = ConditionalT<Const, const ItemT, ItemT>;
		
	private:
		NodeType * Node;

	public:
		TRBTreeIterator(NodeType * NodeIn = nullptr) : Node(NodeIn) {}
		TRBTreeIterator(const TRBTreeIterator & That) : Node(That.Node) {}

		NodeType * GetNode() const { return Node; }
		
		ItemType & operator*()
		{
			return Node->Item;
		}

		ItemType * operator->()
		{
			return &(operator*());
		}

		TRBTreeIterator & operator++()
		{
			Increment();
			return *this;
		}

		TRBTreeIterator operator++(int)
		{
			TRBTreeIterator temp(*this);
			Increment();
			return temp;
		}

		TRBTreeIterator & operator--()
		{
			Decrement();
			return *this;
		}

		TRBTreeIterator operator--(int)
		{
			TRBTreeIterator Temp(*this);
			Decrement();
			return Temp;
		}

		template<bool Const2>
		bool operator==(const TRBTreeIterator<ItemT, Const2> & That) const
		{
			return Node == That.Node;
		}

		template<bool Const2>
		bool operator!=(const TRBTreeIterator<ItemT, Const2> & That) const
		{
			return Node != That.Node;
		}

	private:
		void Increment()
		{
			if (Node->Right)
			{
				Node = Node->Right;
				while (Node->Left)
					Node = Node->Left;
			}
			else
			{
				NodeType * pParent = Node->Parent;
				while (pParent->Right == Node)
				{
					Node = pParent;
					pParent = Node->Parent;
				}

				// 如果树的根节点没有右孩子的情况且迭代器起始位置在根节点
				if (Node->Right != pParent)
					Node = pParent;
			}
		}

		void Decrement()
		{
			if (Node->Parent->Parent == Node && ERBTreeNodeColor::Red == Node->Color)
			{
				Node = Node->Right;
			}
			else if (Node->Left)
			{
				// 在当前节点左子树中找最大的结点
				Node = Node->Left;
				while (Node->Right)
					Node = Node->Right;
			}
			else
			{
				NodeType * pParent = Node;
				while (pParent->Left == Node)
				{
					Node = pParent;
					pParent = Node->Parent;
				}

				Node = pParent;
			}
		}
	};

	template<typename ItemT, typename ComparerT, typename ItemTraitsT>
	class TRBTree
	{
	public:
		using NodeType = TRBTreeNode<ItemT>;
		using Iterator = TRBTreeIterator<ItemT, false>;
		using ConstIterator = TRBTreeIterator<ItemT, true>;

	private:
		ComparerT Comparer;
		NodeType Anchor;
		size_t Size = 0;

	public:
		TRBTree() = default;
		~TRBTree()
		{
			Destroy();
		}

		void Destroy()
		{
			auto FunDestroyNode = [this](NodeType * Node, auto & FunSelf)
			{
				if (!Node)
					return;

				if (Node->Left)
					FunSelf(Node->Left, FunSelf);

				if (Node->Right)
					FunSelf(Node->Right, FunSelf);

				Delete(Node);
			};

			FunDestroyNode(Anchor.Parent, FunDestroyNode);
			Anchor = {};
			Size = 0;
		}

		void Reset()
		{
			Destroy();
		}

		Iterator Begin()
		{
			if (!Anchor.Left)
				return End();
			return Iterator(Anchor.Left);
		}

		ConstIterator Begin() const
		{
			if (!Anchor.Left)
				return End();
			return ConstIterator(Anchor.Left);
		}

		Iterator End()
		{
			return Iterator(const_cast<NodeType *>(&Anchor));
		}

		ConstIterator End() const
		{
			return ConstIterator(const_cast<NodeType *>(&Anchor));
		}

		template<typename KeyT>
		Iterator Find(const KeyT & Key)
		{
			NodeType * Node = LowerBound(Key);
			if (Node != End().GetNode() && !Comparer(Key, ItemTraitsT::GetKey(Node->Item)))
				return Iterator(Node);
			else
				return End();
		}
		
		template<typename KeyT>
		ConstIterator Find(const KeyT & Key) const
		{
			NodeType * Node = LowerBound(Key);
			if (Node != End().GetNode() && !Comparer(Key, ItemTraitsT::GetKey(Node->Item)))
				return ConstIterator(Node);
			else
				return End();
		}

		TPair<Iterator, bool> Insert(const ItemT & Item)
		{
			NodeType * NewNode = nullptr;
			if (Anchor.Parent == nullptr)
			{
				NewNode = New<NodeType>(Item, ERBTreeNodeColor::Black);
				NewNode->Parent = &Anchor;
				Anchor.Parent = NewNode;
				Anchor.Left = MostLeft();
				Anchor.Right = MostRight();
				++Size;
				return MakePair(Iterator(NewNode), true);
			}
			
			NodeType * CurrentNode = Anchor.Parent;
			NodeType * ParentNode = CurrentNode;
			bool IsLeft = true;
			while (CurrentNode)
			{
				if (Comparer(ItemTraitsT::GetKey(Item), ItemTraitsT::GetKey(CurrentNode->Item)))
				{
					ParentNode = CurrentNode;
					CurrentNode = CurrentNode->Left;
					IsLeft = true;
				}
				else if (Comparer(ItemTraitsT::GetKey(CurrentNode->Item), ItemTraitsT::GetKey(Item)))
				{
					ParentNode = CurrentNode;
					CurrentNode = CurrentNode->Right;
					IsLeft = false;
				}
				else
				{
					return TPair<Iterator, bool>(Iterator(CurrentNode), false);
				}
			}

			NewNode = New<NodeType>(Item, ERBTreeNodeColor::Red);
			if (IsLeft)
				ParentNode->Left = NewNode;
			else
				ParentNode->Right = NewNode;
			NewNode->Parent = ParentNode;

			ReBalance(NewNode);

			Anchor.Parent->Color = ERBTreeNodeColor::Black;
			Anchor.Left = MostLeft();
			Anchor.Right = MostRight();
			++Size;
			return MakePair(Iterator(NewNode), true);
		}

		Iterator Emplace(ItemT && Item)
		{
			NodeType * NewNode = nullptr;
			if (Anchor.Parent == nullptr)
			{
				NewNode = New<NodeType>(Item, ERBTreeNodeColor::Black);
				NewNode->Parent = &Anchor;
				Anchor.Parent = NewNode;
				Anchor.Left = MostLeft();
				Anchor.Right = MostRight();
				++Size;
				return Iterator(NewNode);
			}
			else
			{
				NodeType * CurrentNode = Anchor.Parent;
				NodeType * ParentNode = CurrentNode;
				while (CurrentNode)
				{
					if (Comparer(ItemTraitsT::GetKey(Item), ItemTraitsT::GetKey(CurrentNode->Item)))
					{
						ParentNode = CurrentNode;
						CurrentNode = CurrentNode->Left;
					}
					else if (Comparer(ItemTraitsT::GetKey(CurrentNode->Item), ItemTraitsT::GetKey(Item)))
					{
						ParentNode = CurrentNode;
						CurrentNode = CurrentNode->Right;
					}
					else
					{
						ParentNode = CurrentNode;
						CurrentNode = CurrentNode->Right;
						//ParentNode = CurrentNode;
						//CurrentNode = CurrentNode->Left;
						//break;
					}
				}
				
				NewNode = New<NodeType>(Item, ERBTreeNodeColor::Red);
				if (Comparer(ItemTraitsT::GetKey(Item), ItemTraitsT::GetKey(ParentNode->Item)))
					ParentNode->Left = NewNode;
				else
					ParentNode->Right = NewNode;
				NewNode->Parent = ParentNode;

				ReBalance(NewNode);
			}

			Anchor.Parent->Color = ERBTreeNodeColor::Black;
			Anchor.Left = MostLeft();
			Anchor.Right = MostRight();
			++Size;
			return Iterator(NewNode);
		}

		template <typename KeyT, typename ValueT>
		TPair<Iterator, bool> InsertOrAssign(const TPair<KeyT, ValueT> & Item)
		{
			NodeType * NewNode = nullptr;
			if (Anchor.Parent == nullptr)
			{
				NewNode = New<NodeType>(Item, ERBTreeNodeColor::Black);
				NewNode->Parent = &Anchor;
				Anchor.Parent = NewNode;
				Anchor.Left = MostLeft();
				Anchor.Right = MostRight();
				++Size;
				return MakePair(Iterator(NewNode), true);
			}
			
			NodeType * CurrentNode = Anchor.Parent;
			NodeType * ParentNode = CurrentNode;
			bool IsLeft = true;
			while (CurrentNode)
			{
				if (Comparer(Item.Key, CurrentNode->Item.Key))
				{
					ParentNode = CurrentNode;
					CurrentNode = CurrentNode->Left;
					IsLeft = true;
				}
				else if (Comparer(CurrentNode->Item.Key, Item.Key))
				{
					ParentNode = CurrentNode;
					CurrentNode = CurrentNode->Right;
					IsLeft = false;
				}
				else
				{
					CurrentNode->Item.Value = Item.Value;
					return TPair<Iterator, bool>(Iterator(CurrentNode), false);
				}
			}

			NewNode = New<NodeType>(Item, ERBTreeNodeColor::Red);
			if (IsLeft)
				ParentNode->Left = NewNode;
			else
				ParentNode->Right = NewNode;
			NewNode->Parent = ParentNode;

			ReBalance(NewNode);

			Anchor.Parent->Color = ERBTreeNodeColor::Black;
			Anchor.Left = MostLeft();
			Anchor.Right = MostRight();
			++Size;
			return MakePair(Iterator(NewNode), true);
		}

		template <typename KeyT>
		TPair<Iterator, bool> Access(const KeyT & Key)
		{
			NodeType * NewNode = nullptr;
			if (Anchor.Parent == nullptr)
			{
				NewNode = New<NodeType>(ItemT(Key), ERBTreeNodeColor::Black);
				NewNode->Parent = &Anchor;
				Anchor.Parent = NewNode;
				Anchor.Left = MostLeft();
				Anchor.Right = MostRight();
				++Size;
				return MakePair(Iterator(NewNode), true);
			}

			NodeType * CurrentNode = Anchor.Parent;
			NodeType * ParentNode = CurrentNode;
			bool IsLeft = true;
			while (CurrentNode)
			{
				if (Comparer(Key, CurrentNode->Item.Key))
				{
					ParentNode = CurrentNode;
					CurrentNode = CurrentNode->Left;
					IsLeft = true;
				}
				else if (Comparer(CurrentNode->Item.Key, Key))
				{
					ParentNode = CurrentNode;
					CurrentNode = CurrentNode->Right;
					IsLeft = false;
				}
				else
				{
					return TPair<Iterator, bool>(Iterator(CurrentNode), false);
				}
			}

			NewNode = New<NodeType>(ItemT(Key), ERBTreeNodeColor::Red);
			if (IsLeft)
				ParentNode->Left = NewNode;
			else
				ParentNode->Right = NewNode;
			NewNode->Parent = ParentNode;

			ReBalance(NewNode);

			Anchor.Parent->Color = ERBTreeNodeColor::Black;
			Anchor.Left = MostLeft();
			Anchor.Right = MostRight();
			++Size;
			return MakePair(Iterator(NewNode), true);
		}
		
		FORCEINLINE bool IsEmpty() const { return !!Anchor.Parent; }

		size_t GetSize() const
		{
			return Size;
		}

		void Check()
		{
			CheckNodes(Anchor.Parent);
		}

		bool IsRBTree()
		{
			if (Anchor.Parent == nullptr)
				return true;

			if (Anchor.Parent->Color == ERBTreeNodeColor::Red)
			{
				assert(false);
				return false;
			}

			size_t NumBlackNodes = 0;
			NodeType * CurrentNode = Anchor.Parent;
			while (CurrentNode)
			{
				if (CurrentNode->Color == ERBTreeNodeColor::Black)
					++NumBlackNodes;
				CurrentNode = CurrentNode->Left;
			}
			return CheckRBTree(Anchor.Parent, 0, NumBlackNodes);
		}

		void ReBalance(NodeType * CurrentNode)
		{
			while (CurrentNode->Parent && CurrentNode->Parent->Color == ERBTreeNodeColor::Red)
			{
				NodeType * ParentNode = CurrentNode->Parent;
				NodeType * GrandNode = ParentNode->Parent;
				if (ParentNode == GrandNode->Left)
				{
					NodeType * RightUncleNode = GrandNode->Right;
					if (RightUncleNode && RightUncleNode->Color == ERBTreeNodeColor::Red)
					{
						ParentNode->Color = ERBTreeNodeColor::Black;
						RightUncleNode->Color = ERBTreeNodeColor::Black;
						GrandNode->Color = ERBTreeNodeColor::Red;
						CurrentNode = GrandNode;
						ParentNode = CurrentNode->Parent;
					}
					else
					{
						if (CurrentNode == ParentNode->Right)
						{
							RotateLeft(ParentNode);
							Swap(CurrentNode, ParentNode);
						}
						GrandNode->Color = ERBTreeNodeColor::Red;
						ParentNode->Color = ERBTreeNodeColor::Black;
						RotateRight(GrandNode);
					}
				}
				else
				{
					NodeType * LeftUncleNode = GrandNode->Left;
					if (LeftUncleNode && LeftUncleNode->Color == ERBTreeNodeColor::Red)
					{
						ParentNode->Color = ERBTreeNodeColor::Black;
						LeftUncleNode->Color = ERBTreeNodeColor::Black;
						GrandNode->Color = ERBTreeNodeColor::Red;
						CurrentNode = GrandNode;
						ParentNode = CurrentNode->Parent;
					}
					else
					{
						if (CurrentNode == ParentNode->Left)
						{
							RotateRight(ParentNode);
							Swap(CurrentNode, ParentNode);
						}
						GrandNode->Color = ERBTreeNodeColor::Red;
						ParentNode->Color = ERBTreeNodeColor::Black;
						RotateLeft(GrandNode);
					}
				}
			}
		}

		template<typename KeyT>
		NodeType * LowerBound(const KeyT & Key) const
		{
			NodeType * ParentNode = Anchor.Parent;
			NodeType * CurrentNode = const_cast<NodeType *>(&Anchor);

			while (ParentNode)
			{
				if (Comparer(ItemTraitsT::GetKey(ParentNode->Item), Key))
				{
					ParentNode = ParentNode->Right;
				}
				else
				{
					CurrentNode = ParentNode;
					ParentNode = ParentNode->Left;
				}
			}

			return CurrentNode;
		}
		
		template<typename KeyT>
		NodeType * UpperBound(const KeyT & Key) const
		{
			NodeType * ParentNode = Anchor.Parent;
			NodeType * CurrentNode = const_cast<NodeType *>(&Anchor);

			while (ParentNode)
			{
				if (Comparer(Key, ItemTraitsT::GetKey(ParentNode->Item)))
				{
					CurrentNode = ParentNode;
					ParentNode = ParentNode->Left;
				}
				else
				{
					ParentNode = ParentNode->Right;
				}
			}

			return CurrentNode;
		}
		
	private:
		void RotateLeft(NodeType * CenterNode)
		{
			assert(CenterNode->Right != nullptr);
			
			NodeType * RightNode = CenterNode->Right;
			NodeType * LeftNode = CenterNode->Right->Left;
			NodeType * AncestorNode = CenterNode->Parent;

			CenterNode->Parent = RightNode;
			CenterNode->Right = LeftNode;
			if (LeftNode)
				LeftNode->Parent = CenterNode;
			RightNode->Left = CenterNode;

			if (CenterNode == Anchor.Parent)
			{
				Anchor.Parent = RightNode;
				RightNode->Parent = &Anchor;
			}
			else
			{
				if (AncestorNode->Left == CenterNode)
					AncestorNode->Left = RightNode;
				else if (AncestorNode->Right == CenterNode)
					AncestorNode->Right = RightNode;
				else
					assert(false);
				RightNode->Parent = AncestorNode;
			}
		}

		void RotateRight(NodeType * CenterNode)
		{
			assert(CenterNode->Left != nullptr);

			NodeType * LeftNode = CenterNode->Left;
			NodeType * RightNode = CenterNode->Left->Right;
			NodeType * AncestorNode = CenterNode->Parent;

			CenterNode->Parent = LeftNode;
			CenterNode->Left = RightNode;
			if (RightNode)
				RightNode->Parent = CenterNode;
			LeftNode->Right = CenterNode;

			if (CenterNode == Anchor.Parent)
			{
				Anchor.Parent = LeftNode;
				LeftNode->Parent = &Anchor;
			}
			else
			{
				if (AncestorNode->Left == CenterNode)
					AncestorNode->Left = LeftNode;
				else if (AncestorNode->Right == CenterNode)
					AncestorNode->Right = LeftNode;
				else
					assert(false);
				LeftNode->Parent = AncestorNode;
			}
		}

		void CheckNodes(NodeType * pRoot)
		{
			if (pRoot)
			{
				CheckNodes(pRoot->Left);
				CheckNodes(pRoot->Right);
			}
		}

		bool CheckRBTree(NodeType * Node, size_t NumPaths, size_t BlackCount)
		{
			if (Node == nullptr)
				return true;

			if (Node->Color == ERBTreeNodeColor::Black)
				++NumPaths;

			if (Node->Parent && Node->Color == ERBTreeNodeColor::Red && Node->Parent->Color == ERBTreeNodeColor::Red)
			{
				assert(false);
				return false;
			}

			if (Node->Left == nullptr && Node->Right == nullptr)
			{
				if (NumPaths != BlackCount)
				{
					assert(false);
					return false;
				}
			}

			bool bLeft = CheckRBTree(Node->Left, NumPaths, BlackCount);
			bool bRight = CheckRBTree(Node->Right, NumPaths, BlackCount);
			assert(bLeft && bRight);
			return bLeft && bRight;
		}

		NodeType * MostLeft()
		{
			NodeType * CurrentNode = Anchor.Parent;
			if (!CurrentNode)
				return nullptr;
			while (CurrentNode->Left)
				CurrentNode = CurrentNode->Left;
			return CurrentNode;
		}

		NodeType * MostRight()
		{
			NodeType * CurrentNode = Anchor.Parent;
			if (!CurrentNode)
				return nullptr;
			while (CurrentNode->Right)
				CurrentNode = CurrentNode->Right;
			return CurrentNode;
		}

	public:
		Iterator begin() { return Begin(); }
		ConstIterator begin() const { return Begin(); }
		Iterator end() { return End(); }
		ConstIterator end() const { return End(); }
	};
}
