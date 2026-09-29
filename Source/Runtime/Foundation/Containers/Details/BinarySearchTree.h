#pragma once

#include "Foundation/Containers/Pair.h"
#include "Foundation/Concepts/Invocable.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/AddressOf.h"
#include "Foundation/Memory/Allocator.h"

#include "Foundation/Templates/Exchange.h"

namespace Kitsune::Details
{
    // Keep this as a regular enum, we need to implicitly cast this to bool.
    enum BSTDirection
    {
        Left = 0,
        Right = 1
    };

    template<typename Node, typename Tree>
    class BSTIterator
    {
    private:
        using NodeType = typename Tree::NodeType;

    public:
        // `Node` is used to signify the const-ness of a BST iterator.
        using DifferenceType = std::ptrdiff_t;
        using ValueType = std::remove_reference_t<
            decltype((std::declval<Node>().Value))>;

        using MappedType = std::remove_reference_t<
            decltype(std::declval<Node>().GetMapped())>;

    public:
        inline BSTIterator()
            : m_Current(), m_Tree()
        {
        }

        inline BSTIterator(NodeType* node, const Tree* tree)
            : m_Current(node), m_Tree(tree)
        {
        }

        // Enable non-const to const conversions for this iterator.
        template<typename OtherNode>
            requires std::same_as<OtherNode, std::remove_cv_t<Node>>
        inline BSTIterator(const BSTIterator<OtherNode, Tree>& iter)
            : m_Current(iter.m_Current), m_Tree(iter.m_Tree)
        {
        }

        BSTIterator(const BSTIterator&) = default;
        BSTIterator(BSTIterator&&) = default;

        ~BSTIterator() = default;

    public:
        BSTIterator& operator=(const BSTIterator&) = default;
        BSTIterator& operator=(BSTIterator&&) = default;

        template<typename OtherNode>
            requires std::same_as<OtherNode, std::remove_cv_t<Node>>
        inline BSTIterator& operator=(const BSTIterator<Node, Tree>& iter)
        {
            m_Current = iter.m_Current;
            m_Tree = iter.m_Tree;

            return *this;
        }

    public:
        inline ValueType& operator*() const
        {
            return m_Current->Value;
        }

        inline MappedType* operator->() const
        {
            return AddressOf(m_Current->GetMapped());
        }

    public:
        inline BSTIterator& operator++()
        {
            // If the iterator has reached the end of the set, set the current
            // node to be a null pointer.
            if (m_Current == nullptr)
                return *this;

            if (m_Current == m_Tree->m_Back)
            {
                m_Current = nullptr;
                return *this;
            }

            if (m_Current->GetRight() != nullptr)
            {
                // Right child exists: The next node will be the furthermost left
                // child node connected to m_Current->Right.
                m_Current = m_Current->GetRight();
                while (m_Current->GetLeft() != nullptr)
                    m_Current = m_Current->GetLeft();
            }
            else
            {
                // Right child doesn't exist: Traverse up the tree to a node with
                // a greater value.
                NodeType* parent = m_Current->GetParent();
                while (parent && (parent->GetRight() == m_Current))
                {
                    m_Current = parent;
                    parent = parent->GetParent();
                }

                m_Current = parent;
            }

            return *this;
        }

        inline BSTIterator operator++(int)
        {
            BSTIterator copy = *this;
            ++(*this);

            return copy;
        }

    public:
        [[nodiscard]]
        inline NodeType* GetNode() const
        {
            return m_Current;
        }

    private:
        template<typename OtherNode, typename OtherTree>
        friend class BSTIterator;

    private:
        NodeType* m_Current;
        const Tree* m_Tree;
    };

    template<typename Storage, typename Tree>
    inline bool operator==(const BSTIterator<Storage, Tree>& iter1,
                           const BSTIterator<Storage, Tree>& iter2)
    {
        return (iter1.GetNode() == iter2.GetNode());
    }

    template<
        typename Node,
        typename Compare,
        Allocator Alloc>
    class BasicBinarySearchTree
    {
    private:
        using ThisType = BasicBinarySearchTree<Node, Compare, Alloc>;

    protected:
        using ValueType = typename Node::ValueType;
        using NodeType = Node;

        using KeyType = typename Node::KeyType;
        using MappedType = typename Node::MappedType;

        using CompareType = Compare;
        using AllocatorType = Alloc;

        using Iterator = BSTIterator<Node, ThisType>;
        using ConstIterator = BSTIterator<const Node, ThisType>;

        static_assert(
            InvocableReturn<Compare, bool, const KeyType&, const KeyType&>,
            "The `Compare` template parameter has to be callable with KeyType.");

    public:
        inline BasicBinarySearchTree() = default;
        inline BasicBinarySearchTree(const Compare& compare, const Alloc& allocator)
            : m_Compare(compare), m_Allocator(allocator)
        {
        }

        inline BasicBinarySearchTree(const BasicBinarySearchTree& tree)
            : m_Compare(tree.m_Compare), m_Allocator(tree.m_Allocator)
        {
            RecursiveCopy(nullptr, tree.m_Root, tree);
        }

        inline BasicBinarySearchTree(BasicBinarySearchTree&& tree)
            : m_Root(Exchange(tree.m_Root, nullptr)),
              m_Size(Exchange(tree.m_Size, 0)),
              m_Front(Exchange(tree.m_Front, nullptr)),
              m_Back(Exchange(tree.m_Back, nullptr)),
              m_Compare(Move(tree.m_Compare)),
              m_Allocator(Move(tree.m_Allocator))
        {
        }

        inline ~BasicBinarySearchTree()
        {
            RecursiveClear(m_Root);
        }

    public:
        inline BasicBinarySearchTree& operator=(const BasicBinarySearchTree& tree)
        {
            if (this != &tree)
                BasicBinarySearchTree(tree).Swap(*this);

            return *this;
        }

        inline BasicBinarySearchTree& operator=(BasicBinarySearchTree&& tree)
        {
            if (this != &tree)
                BasicBinarySearchTree(Move(tree)).Swap(*this);

            return *this;
        }

    public:
        [[nodiscard]]
        inline Iterator GetBegin()
        {
            return Iterator(m_Front, this);
        }

        [[nodiscard]]
        inline ConstIterator GetBegin() const
        {
            return ConstIterator(m_Front, this);
        }

        [[nodiscard]]
        inline Iterator GetEnd()
        {
            return Iterator(nullptr, this);
        }

        [[nodiscard]]
        inline ConstIterator GetEnd() const
        {
            return ConstIterator(nullptr, this);
        }

    public:
        [[nodiscard]] inline bool IsEmpty() const { return (Size() == 0); }
        [[nodiscard]] inline Usize Size() const { return m_Size; }

        [[nodiscard]] inline Compare GetCompare() const { return m_Compare; }

        [[nodiscard]]
        inline Alloc& GetAllocator() { return m_Allocator; }

        [[nodiscard]]
        inline const Alloc& GetAllocator() const { return m_Allocator; }

    public:
        inline void Clear()
        {
            RecursiveClear(m_Root);

            m_Root = m_Front = m_Back = nullptr;
            m_Size = 0;
        }

        inline void Swap(BasicBinarySearchTree& tree)
        {
            Kitsune::Swap(m_Root, tree.m_Root);
            Kitsune::Swap(m_Size, tree.m_Size);

            Kitsune::Swap(m_Front, tree.m_Front);
            Kitsune::Swap(m_Back, tree.m_Back);

            Kitsune::Swap(m_Compare, tree.m_Compare);
            Kitsune::Swap(m_Allocator, tree.m_Allocator);
        }

    public:
        inline bool Contains(const KeyType& key) const
        {
            return (Find(key) != GetEnd());
        }

        inline Iterator Find(const KeyType& key)
        {
            return InternalFind(key);
        }

        inline ConstIterator Find(const KeyType& key) const
        {
            return ConstIterator(InternalFind(key));
        }

    // NOTE: Classes which derive from this might change the behaviour of Insert.
    // That might cause Insert(Node&&) to work differently than the derived class's
    // Insert(...). Keep this protected!
    protected:
        inline Pair<NodeType*, bool> Insert(Node&& value)
        {
            if (m_Root == nullptr)
            {
                m_Root = CreateNode(Move(value));
                m_Size = 1;

                m_Front = m_Back = m_Root;
                return { m_Root, true };
            }

            Node* parent = m_Root;
            const KeyType& key = value.GetKey();

            while (true)
            {
                bool greaterThan = m_Compare(parent->GetKey(), key);
                if (!greaterThan && !m_Compare(key, parent->GetKey()))
                    return { parent, false };

                BSTDirection direction = greaterThan ?
                    BSTDirection::Right :
                    BSTDirection::Left;

                if (parent->GetChild(direction) != nullptr)
                    parent = parent->GetChild(direction);
                else
                {
                    Node* node = CreateNode(Move(value));

                    node->SetParent(parent);
                    parent->SetChild(direction, node);

                    if ((parent == m_Front) && (parent->GetLeft() == node))
                        m_Front = node;
                    else if ((parent == m_Back) && (parent->GetRight()) == node)
                        m_Back = node;

                    ++m_Size;
                    return { node, true };
                }
            }
        }

    private:
        inline Iterator InternalFind(const KeyType& key) const
        {
            Node* node = m_Root;
            while (node != nullptr)
            {
                if (m_Compare(key, node->GetKey()))
                    node = node->GetLeft();
                else if (m_Compare(node->GetKey(), key))
                    node = node->GetRight();
                else
                    return Iterator(node, this);
            }

            return Iterator(nullptr, this);
        }

    private:
        inline void RecursiveClear(Node* node)
        {
            if (node == nullptr)
                return;

            RecursiveClear(node->GetLeft());
            RecursiveClear(node->GetRight());

            DeleteNode(node);
        }

        inline void RecursiveCopy(
            Node* parent, Node* node,
            const BasicBinarySearchTree& tree,
            BSTDirection direction = BSTDirection::Left)
        {
            if (node == nullptr)
                return;

            Node* copy = CreateNode(*node);
            if (m_Root == nullptr)
                m_Root = copy;
            else
            {
                copy->SetParent(parent);
                parent->SetChild(direction, copy);
            }

            ++m_Size;
            if (node == tree.m_Front)
                m_Front = node;
            else if (node == tree.m_Back)
                m_Back = node;

            RecursiveCopy(copy, node->GetLeft(), tree, BSTDirection::Left);
            RecursiveCopy(copy, node->GetRight(), tree, BSTDirection::Right);
        }

    private:
        template<typename T>
        [[nodiscard]]
        inline Node* CreateNode(T&& node)
        {
            void* pointer = m_Allocator.Allocate(sizeof(Node), alignof(Node));
            return Memory::ConstructAt<Node>(pointer, Forward<T>(node));
        }

        inline void DeleteNode(Node* node)
        {
            Memory::DestroyAt(node);
            m_Allocator.Free(node, sizeof(Node));
        }

    public:
        // Should not be called by engine/client code.
        // Made public so that the compiler can generate code for range-based for loops.
        inline Iterator begin() { return GetBegin(); }
        inline ConstIterator begin() const { return GetBegin(); }

        inline Iterator end() { return GetEnd(); }
        inline ConstIterator end() const { return GetEnd(); }

    private:
        template<typename OtherNode, typename OtherTree>
        friend class BSTIterator;

    protected:
        Node* m_Root = nullptr;

        Usize m_Size = 0;
        Node* m_Front = nullptr;
        Node* m_Back = nullptr;

        KITSUNE_MAYBE_OVERLAPPING Compare m_Compare;
        KITSUNE_MAYBE_OVERLAPPING Alloc m_Allocator;
    };
}
