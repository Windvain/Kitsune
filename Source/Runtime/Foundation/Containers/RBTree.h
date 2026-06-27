#pragma once

#include "Foundation/Containers/Pair.h"
#include "Foundation/Concepts/Invocable.h"

#include "Foundation/Memory/AddressOf.h"
#include "Foundation/Memory/Allocator.h"
#include "Foundation/Memory/GlobalAllocator.h"

#include "Foundation/Templates/Exchange.h"
#include "Foundation/Utilities/Comparators.h"

namespace Kitsune
{
    namespace Details
    {
        enum RBTNodeDirection
        {
            Left = 0,
            Right = 1
        };

        enum class RBTNodeColor
        {
            Red,
            Black
        };

        template<bool IsSet, typename T>
        inline static const auto& GetRBTNodeKey(const T& storage)
        {
            if constexpr (IsSet)
                return storage;
            else
                return storage.First;
        }

        template<bool IsSet, typename T>
        inline static decltype(auto) GetRBTNodeValue(T& storage)
        {
            if constexpr (IsSet)
                return storage;
            else
                return storage.Second;
        }

        template<typename T, bool IsSet>
        class RBTNode
        {
        public:
            template<typename... Args>
            inline RBTNode(RBTNodeColor color, Args&&... args)
                : m_Storage(Forward<Args>(args)...), m_Color(color)
            {
            }

        public:
            [[nodiscard]]
            inline RBTNode* GetChild(RBTNodeDirection direction) const
            {
                return m_Children[direction];
            }

            [[nodiscard]]
            inline RBTNode* GetLeftChild() const
            {
                return m_Children[RBTNodeDirection::Left];
            }

            [[nodiscard]]
            inline RBTNode* GetRightChild() const
            {
                return m_Children[RBTNodeDirection::Right];
            }

        public:
            inline void SetChild(RBTNodeDirection direction, RBTNode* node)
            {
                m_Children[direction] = node;
            }

            inline void SetLeftChild(RBTNode* node)
            {
                SetChild(RBTNodeDirection::Left, node);
            }

            inline void SetRightChild(RBTNode* node)
            {
                SetChild(RBTNodeDirection::Right, node);
            }

        public:
            inline void SetParent(RBTNode* node)
            {
                m_Parent = node;
            }

            [[nodiscard]]
            inline RBTNode* GetParent() const
            {
                return m_Parent;
            }

            [[nodiscard]]
            inline RBTNode* GetGrandparent() const
            {
                return m_Parent->m_Parent;
            }

        public:
            [[nodiscard]] inline T& GetStorage() { return m_Storage; }
            [[nodiscard]] inline const T& GetStorage() const { return m_Storage; }

            [[nodiscard]]
            inline const auto& GetKey() const
            {
                return GetRBTNodeKey<IsSet>(m_Storage);
            }

            [[nodiscard]]
            inline const auto& GetValue() const
            {
                return GetRBTNodeValue<IsSet>(m_Storage);
            }

            [[nodiscard]]
            inline auto& GetValue()
            {
                return GetRBTNodeValue<IsSet>(m_Storage);
            }

        public:
            [[nodiscard]] inline RBTNodeColor GetColor() const { return m_Color; }
            inline void SetColor(RBTNodeColor color) { m_Color = color; }

        private:
            T m_Storage;
            RBTNodeColor m_Color;

            RBTNode* m_Parent = nullptr;
            RBTNode* m_Children[2] = { nullptr, nullptr };
        };

        template<typename Storage, typename Tree>
        class RBTIterator
        {
        private:
            using NodeType = typename Tree::NodeType;

        public:
            using ValueType = Storage;
            using DifferenceType = std::ptrdiff_t;

        public:
            inline RBTIterator()
                : m_Current(), m_Tree()
            {
            }

            inline RBTIterator(NodeType* node, const Tree* tree)
                : m_Current(node), m_Tree(tree)
            {
            }

        public:
            inline Storage& operator*() const
            {
                return m_Current->GetStorage();
            }

            inline auto* operator->() const
            {
                return AddressOf(m_Current->GetValue());
            }

        public:
            inline RBTIterator& operator++()
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

                if (m_Current->GetRightChild() != nullptr)
                {
                    // Right child exists: The next node will be the furthermost left
                    // child node connected to m_Current->Right.
                    m_Current = m_Current->GetRightChild();
                    while (m_Current->GetLeftChild() != nullptr)
                        m_Current = m_Current->GetLeftChild();
                }
                else
                {
                    // Right child doesn't exist: Traverse up the tree to a node with
                    // a greater value.
                    NodeType* parent = m_Current->GetParent();
                    while (parent && (parent->GetRightChild() == m_Current))
                    {
                        m_Current = parent;
                        parent = parent->GetParent();
                    }

                    m_Current = parent;
                }

                return *this;
            }

            inline RBTIterator operator++(int)
            {
                RBTIterator copy = *this;
                ++(*this);

                return copy;
            }

        public:
            [[nodiscard]]
            inline NodeType* GetNodeHandle() const
            {
                return m_Current;
            }

        private:
            NodeType* m_Current;
            const Tree* m_Tree;
        };

        template<typename Storage, typename Tree>
        inline bool operator==(const RBTIterator<Storage, Tree>& iter1,
                               const RBTIterator<Storage, Tree>& iter2)
        {
            return (iter1.GetNodeHandle() == iter2.GetNodeHandle());
        }
    }

    template<
        typename Key, typename Value,
        InvocableReturn<bool, const Key&, const Key&> Compare = LessFunctor<Key>,
        Allocator Alloc = GlobalAllocator,
        bool IsSet = std::is_same_v<Value, void>>
    class RBTree
    {
    private:
        using ThisType = RBTree<Key, Value, Compare, Alloc, IsSet>;

    // Keep this `protected` access modifier, because this class will be inherited
    // by either TreeSet<T> or TreeMap<T>, and they have different type-aliases.
    protected:
        using StorageType = std::conditional_t<IsSet, const Key, Pair<const Key, Value>>;
        using NodeType = Details::RBTNode<StorageType, IsSet>;

        using CompareType = Compare;
        using AllocatorType = Alloc;

        using Iterator = Details::RBTIterator<StorageType, ThisType>;
        using ConstIterator = Details::RBTIterator<const StorageType, ThisType>;

    public:
        inline RBTree() = default;
        inline explicit RBTree(const Compare& compare, const Alloc& allocator)
            : m_Compare(compare), m_Allocator(allocator)
        {
        }

        inline RBTree(const RBTree& tree)
            : m_Compare(tree.m_Compare), m_Allocator(tree.m_Allocator)
        {
            RecursiveCopy(nullptr, tree.m_Root, tree);
        }

        inline RBTree(RBTree&& tree)
            : m_Root(Exchange(tree.m_Root, nullptr)),
              m_Size(Exchange(tree.m_Size, 0)),
              m_Front(Exchange(tree.m_Front, nullptr)),
              m_Back(Exchange(tree.m_Back, nullptr)),
              m_Compare(Move(tree.m_Compare)),
              m_Allocator(Move(tree.m_Allocator))
        {
        }

        inline ~RBTree()
        {
            RecursiveClear(m_Root);
        }

    public:
        inline RBTree& operator=(const RBTree& tree)
        {
            if (this == &tree)
                return *this;

            Clear();

            m_Allocator = tree.m_Allocator;
            m_Compare = tree.m_Compare;

            RecursiveCopy(nullptr, tree.m_Root, tree);
            return *this;
        }

        inline RBTree& operator=(RBTree&& tree)
        {
            if (this != &tree)
                RBTree(Move(tree)).Swap(*this);

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
        [[nodiscard]]
        inline bool IsEmpty() const
        {
            return (Size() == 0);
        }

        [[nodiscard]]
        inline Usize Size() const
        {
            return m_Size;
        }

        [[nodiscard]] inline Alloc& GetAllocator() { return m_Allocator; }
        [[nodiscard]] inline const Alloc& GetAllocator() const { return m_Allocator; }

        [[nodiscard]]
        inline Compare GetCompare() const
        {
            return m_Compare;
        }

    public:
        inline void Clear()
        {
            RecursiveClear(m_Root);

            m_Root = m_Front = m_Back = nullptr;
            m_Size = 0;
        }

        inline void Swap(RBTree& tree)
        {
            Kitsune::Swap(m_Root, tree.m_Root);
            Kitsune::Swap(m_Size, tree.m_Size);

            Kitsune::Swap(m_Front, tree.m_Front);
            Kitsune::Swap(m_Back, tree.m_Back);

            Kitsune::Swap(m_Compare, tree.m_Compare);
            Kitsune::Swap(m_Allocator, tree.m_Allocator);
        }

    public:
        inline Pair<Iterator, bool> Insert(const StorageType& value)
        {
            return Emplace(value);
        }

        inline Pair<Iterator, bool> Insert(StorageType&& value)
        {
            return Emplace(Move(value));
        }

        template<typename... Args>
            requires std::constructible_from<StorageType, Args...>
        inline Pair<Iterator, bool> Emplace(Args&&... args)
        {
            auto [node, success] = BSTEmplace(Forward<Args>(args)...);
            if (success)
                FixInsertion(node);

            return { Iterator(node, this), success };
        }

    public:
        inline bool Contains(const Key& key) const
        {
            return (Find(key) != GetEnd());
        }

        inline Iterator Find(const Key& key)
        {
            return Iterator(InternalFind(key), this);
        }

        inline ConstIterator Find(const Key& key) const
        {
            return ConstIterator(InternalFind(key), this);
        }

    private:
        inline NodeType* InternalFind(const Key& key) const
        {
            NodeType* node = m_Root;
            while (node != nullptr)
            {
                if (m_Compare(key, node->GetKey()))
                    node = node->GetLeftChild();
                else if (m_Compare(node->GetKey(), key))
                    node = node->GetRightChild();
                else
                    return node;
            }

            return nullptr;
        }

    private:
        template<typename... Args>
        inline Pair<NodeType*, bool> BSTEmplace(Args&&... args)
        {
            if (m_Root == nullptr)
                return BSTFirstEmplace(Forward<Args>(args)...);

            return BSTRestInsert(StorageType(Forward<Args>(args)...));
        }

        template<typename... Args>
        [[nodiscard]]
        inline Pair<NodeType*, bool> BSTFirstEmplace(Args&&... args)
        {
            m_Root = CreateNode(Details::RBTNodeColor::Black, Forward<Args>(args)...);
            ++m_Size;

            m_Front = m_Back = m_Root;
            return { m_Root, true };
        }

        [[nodiscard]]
        inline Pair<NodeType*, bool> BSTRestInsert(StorageType&& value)
        {
            using namespace Details;

            NodeType* parent = m_Root;
            const Key& key = Details::GetRBTNodeKey<IsSet>(value);

            while (true)
            {
                bool greaterThan = m_Compare(parent->GetKey(), key);
                if (!greaterThan && !m_Compare(key, parent->GetKey()))
                    return { parent, false };

                RBTNodeDirection direction = greaterThan ?
                    RBTNodeDirection::Right :
                    RBTNodeDirection::Left;

                if (parent->GetChild(direction) != nullptr)
                    parent = parent->GetChild(direction);
                else
                {
                    NodeType* node = CreateNode(RBTNodeColor::Red, Move(value));
                    node->SetParent(parent);
                    parent->SetChild(direction, node);

                    if ((parent == m_Front) && (parent->GetLeftChild() == node))
                        m_Front = node;
                    else if ((parent == m_Back) && (parent->GetRightChild()) == node)
                        m_Back = node;

                    ++m_Size;
                    return { node, true };
                }
            }
        }

        inline void FixInsertion(NodeType* node)
        {
            using namespace Details;

            // If the parent of the newly inserted node is black, no properties have
            // been violated.
            while ((node->GetParent() != nullptr) &&
                   (node->GetParent()->GetColor() == RBTNodeColor::Red))
            {
                NodeType* grandparent = node->GetGrandparent();
                auto parentDir = (grandparent->GetLeftChild() == node->GetParent()) ?
                    RBTNodeDirection::Left :
                    RBTNodeDirection::Right;

                Details::RBTNodeDirection flipped = Flip(parentDir);
                NodeType* uncle = grandparent->GetChild(flipped);

                // Case 1: Uncle is red: Recolor parent and uncle to black, grandparent
                // to red, then jump up and repeat.
                if ((uncle != nullptr) && (uncle->GetColor() == RBTNodeColor::Red))
                {
                    node->GetParent()->SetColor(RBTNodeColor::Black);
                    uncle->SetColor(RBTNodeColor::Black);

                    grandparent->SetColor(RBTNodeColor::Red);
                    node = grandparent;
                }
                else /* uncle == nullptr || uncle->Color == 0 */
                {
                    // Case 2.1: Uncle and child face the same way: Rotate the
                    // other way to correct the tree shape.
                    if (node == node->GetParent()->GetChild(flipped))
                    {
                        node = node->GetParent();
                        Rotate(node, parentDir);
                    }

                    // Case 2: Recolor parent to black and grandparent to red,
                    // then rotate to restore balance.
                    node->GetParent()->SetColor(RBTNodeColor::Black);
                    node->GetGrandparent()->SetColor(RBTNodeColor::Red);

                    Rotate(node->GetGrandparent(), flipped);
                }
            }

            m_Root->SetColor(RBTNodeColor::Black);
        }

        inline void Rotate(NodeType* node, Details::RBTNodeDirection direction)
        {
            Details::RBTNodeDirection flipped = Flip(direction);

            NodeType* promoted = node->GetChild(flipped);
            node->SetChild(flipped, promoted->GetChild(direction));

            if (promoted->GetChild(direction) != nullptr)
                promoted->GetChild(direction)->SetParent(node);

            promoted->SetParent(node->GetParent());
            if (node->GetParent() == nullptr)
                m_Root = promoted;
            else if (node == node->GetParent()->GetLeftChild())
                node->GetParent()->SetLeftChild(promoted);
            else
                node->GetParent()->SetRightChild(promoted);

            promoted->SetChild(direction, node);
            node->SetParent(promoted);
        }

    private:
        inline void RecursiveClear(NodeType* node)
        {
            if (node == nullptr)
                return;

            RecursiveClear(node->GetLeftChild());
            RecursiveClear(node->GetRightChild());

            Memory::DestroyAt(node);
            m_Allocator.Free(node, sizeof(NodeType));
        }

        inline void RecursiveCopy(
            NodeType* parent, NodeType* node,
            const RBTree& tree,
            Details::RBTNodeDirection direction = Details::RBTNodeDirection::Left)
        {
            using namespace Details;
            if (node == nullptr)
                return;

            NodeType* copy = CreateNode(RBTNodeColor::Black, node->GetStorage());
            if (m_Root == nullptr)
                m_Root = copy;
            else
            {
                copy->SetColor(node->GetColor());
                copy->SetParent(parent);

                parent->SetChild(direction, copy);
            }

            ++m_Size;
            if (node == tree.m_Front)
                m_Front = node;
            else if (node == tree.m_Back)
                m_Back = node;

            RecursiveCopy(copy, node->GetLeftChild(), tree, RBTNodeDirection::Left);
            RecursiveCopy(copy, node->GetRightChild(), tree, RBTNodeDirection::Right);
        }

    private:
        template<typename... Args>
        [[nodiscard]]
        inline NodeType* CreateNode(Details::RBTNodeColor color, Args&&... args)
        {
            void* pointer = m_Allocator.Allocate(sizeof(NodeType), alignof(NodeType));
            return Memory::ConstructAt<NodeType>(
                pointer,
                color,
                Forward<Args>(args)...);
        }

        inline auto Flip(Details::RBTNodeDirection direction)
        {
            return Details::RBTNodeDirection(!bool(direction));
        }

    public:
        // Should not be called by engine/client code.
        // Made public so that the compiler can generate code for range-based for loops.
        inline Iterator begin() { return GetBegin(); }
        inline ConstIterator begin() const { return GetBegin(); }

        inline Iterator end() { return GetEnd(); }
        inline ConstIterator end() const { return GetEnd(); }

    private:
        template<typename Storage, typename Tree>
        friend class Details::RBTIterator;

    private:
        NodeType* m_Root = nullptr;

        Usize m_Size = 0;
        NodeType* m_Front = nullptr;
        NodeType* m_Back = nullptr;

        KITSUNE_MAYBE_OVERLAPPING Compare m_Compare;
        KITSUNE_MAYBE_OVERLAPPING Alloc m_Allocator;
    };
}
