#pragma once

#include <initializer_list>

#include "Foundation/Containers/Pair.h"
#include "Foundation/Concepts/Invocable.h"

#include "Foundation/Algorithms/Equal.h"
#include "Foundation/Templates/Exchange.h"
#include "Foundation/Utilities/Comparators.h"

#include "Foundation/Memory/AddressOf.h"
#include "Foundation/Memory/Allocator.h"
#include "Foundation/Memory/GlobalAllocator.h"

namespace Kitsune
{
    namespace Details
    {
        enum TreeSetNodeDirection { Left, Right };
        enum class TreeSetNodeColor { Red, Black };

        KITSUNE_FORCEINLINE
        TreeSetNodeDirection FlipNodeDirection_(TreeSetNodeDirection direction)
        {
            return (direction == TreeSetNodeDirection::Left) ?
                TreeSetNodeDirection::Right :
                TreeSetNodeDirection::Left;
        }

        template<typename T>
        class TreeSetNode
        {
        public:
            template<typename U>
            inline TreeSetNode(U&& value, TreeSetNodeColor color)
                : m_Value(Forward<U>(value)), m_Color(color)
            {
            }

        public:
            [[nodiscard]]
            inline TreeSetNode* GetChild(TreeSetNodeDirection direction) const
            {
                return m_Children[direction];
            }

            [[nodiscard]]
            inline TreeSetNode* GetLeftChild() const
            {
                return m_Children[TreeSetNodeDirection::Left];
            }

            [[nodiscard]]
            inline TreeSetNode* GetRightChild() const
            {
                return m_Children[TreeSetNodeDirection::Right];
            }

        public:
            inline void SetChild(TreeSetNodeDirection direction, TreeSetNode* node)
            {
                m_Children[direction] = node;
            }

            inline void SetLeftChild(TreeSetNode* node)
            {
                SetChild(TreeSetNodeDirection::Left, node);
            }

            inline void SetRightChild(TreeSetNode* node)
            {
                SetChild(TreeSetNodeDirection::Right, node);
            }

            inline void SetParent(TreeSetNode* node)
            {
                m_Parent = node;
            }

        public:
            [[nodiscard]]
            inline TreeSetNode* GetParent() const
            {
                return m_Parent;
            }

            [[nodiscard]]
            inline TreeSetNode* GetGrandparent() const
            {
                return m_Parent->m_Parent;
            }

        public:
            [[nodiscard]] inline TreeSetNodeColor GetColor() const { return m_Color; }
            [[nodiscard]] inline const T& GetValue() const { return m_Value; }

            inline void SetColor(TreeSetNodeColor color) { m_Color = color; }

        private:
            T m_Value;
            TreeSetNodeColor m_Color;

            TreeSetNode* m_Parent = nullptr;
            TreeSetNode* m_Children[2] = { nullptr, nullptr };
        };

        template<typename T, typename SetType>
        class TreeSetIterator
        {
        private:
            using NodeType_ = typename SetType::NodeType;

        public:
            using ValueType = const T;
            using DifferenceType = Ptrdiff;

        public:
            inline TreeSetIterator()
                : m_Current(), m_Set()
            {
            }

            inline TreeSetIterator(NodeType_* node, const SetType* set)
                : m_Current(node), m_Set(set)
            {
            }

        public:
            inline const T& operator*() const { return m_Current->GetValue(); }
            inline const T* operator->() const
            {
                return AddressOf(m_Current->GetValue());
            }

            [[nodiscard]]
            inline const NodeType_* GetNode() const
            {
                return m_Current;
            }

        public:
            inline TreeSetIterator& operator++()
            {
                // If the iterator has reached the end of the set, set the current
                // node to be a null pointer.
                if (m_Current == nullptr)
                    return *this;

                if (m_Current == m_Set->m_Back)
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
                    NodeType_* parent = m_Current->GetParent();
                    while (parent && (parent->GetRightChild() == m_Current))
                    {
                        m_Current = parent;
                        parent = parent->GetParent();
                    }

                    m_Current = parent;
                }

                return *this;
            }

            inline TreeSetIterator operator++(int)
            {
                TreeSetIterator copy = *this;
                ++(*this);

                return copy;
            }

        private:
            NodeType_* m_Current;
            const SetType* m_Set;
        };

        template<typename T, typename SetType>
        inline bool operator==(const TreeSetIterator<T, SetType>& iter1,
                               const TreeSetIterator<T, SetType>& iter2)
        {
            return (iter1.GetNode() == iter2.GetNode());
        }
    }

    template<
        typename T,
        InvocableReturn<bool, const T&, const T&> Compare = LessFunctor<T>,
        Allocator Alloc = GlobalAllocator>
    class TreeSet
    {
    private:
        using ThisType_ = TreeSet<T, Compare, Alloc>;

    public:
        using ValueType = T;
        using NodeType = Details::TreeSetNode<T>;

        using CompareType = Compare;
        using AllocatorType = Alloc;

        using Iterator = Details::TreeSetIterator<T, ThisType_>;
        using ConstIterator = Details::TreeSetIterator<const T, ThisType_>;

    public:
        inline TreeSet()
            : m_Compare(), m_Allocator()
        {
        }

        inline explicit TreeSet(const Compare& compare,
                                const Alloc& allocator = Alloc())
            : m_Compare(compare), m_Allocator(allocator)
        {
        }

        inline explicit TreeSet(const Alloc& allocator)
            : TreeSet(Compare(), allocator)
        {
        }

        template<ForwardIterator Iter>
        inline TreeSet(Iter begin, Iter end,
                   const Compare& compare = Compare(),
                   const Alloc& allocator = Alloc())
            : m_Compare(compare), m_Allocator(allocator)
        {
            Insert(begin, end);
        }

        template<ForwardIterator Iter>
        inline TreeSet(Iter begin, Iter end, const Alloc& allocator)
            : TreeSet(begin, end, Compare(), allocator)
        {
        }

        inline TreeSet(const TreeSet& set)
            : m_Compare(set.m_Compare), m_Allocator(set.m_Allocator)
        {
            RecursiveCopy_(nullptr, set.m_Root, set);
        }

        inline TreeSet(TreeSet&& set)
            : m_Root(Exchange(set.m_Root, nullptr)),
              m_Size(Exchange(set.m_Size, 0)),
              m_Front(Exchange(set.m_Front, nullptr)),
              m_Back(Exchange(set.m_Back, nullptr)),
              m_Compare(Move(set.m_Compare)),
              m_Allocator(Move(set.m_Allocator))
        {
        }

        inline TreeSet(std::initializer_list<T> initList,
                   const Compare& compare = Compare(),
                   const Alloc& allocator = Alloc())
            : m_Compare(compare), m_Allocator(allocator)
        {
            Insert(initList);
        }

        inline TreeSet(std::initializer_list<T> initList,
                   const Alloc& allocator)
            : TreeSet(initList, Compare(), allocator)
        {
        }

        inline ~TreeSet()
        {
            RecursiveClear_(m_Root);
        }

    public:
        inline TreeSet& operator=(const TreeSet& set)
        {
            if (this == &set)
                return *this;

            Clear();

            m_Allocator = set.m_Allocator;
            m_Compare = set.m_Compare;

            RecursiveCopy_(nullptr, set.m_Root, set);
            return *this;
        }

        inline TreeSet& operator=(TreeSet&& set)
        {
            if (this == &set)
                return *this;

            TreeSet(Move(set)).Swap(*this);
            return *this;
        }

        inline TreeSet& operator=(std::initializer_list<T> initList)
        {
            Clear();
            Insert(initList);

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

        [[nodiscard]] inline Iterator GetEnd() { return Iterator(); }
        [[nodiscard]] inline ConstIterator GetEnd() const { return ConstIterator(); }

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

    public:
        inline void Clear()
        {
            RecursiveClear_(m_Root);

            m_Root = m_Front = m_Back = nullptr;
            m_Size = 0;
        }

        inline Pair<Iterator, bool> Insert(const T& value)
        {
            return InternalInsert_(value);
        }

        inline Pair<Iterator, bool> Insert(T&& value)
        {
            return InternalInsert_(Move(value));
        }

        template<ForwardIterator Iter>
        inline void Insert(Iter begin, Iter end)
        {
            for (; begin != end; ++begin)
                Insert(*begin);
        }

        inline void Insert(std::initializer_list<T> initList)
        {
            for (auto iter = initList.begin(); iter != initList.end(); ++iter)
                Insert(*iter);
        }

        inline void Swap(TreeSet& otherSet)
        {
            Kitsune::Swap(m_Root, otherSet.m_Root);
            Kitsune::Swap(m_Size, otherSet.m_Size);

            Kitsune::Swap(m_Front, otherSet.m_Front);
            Kitsune::Swap(m_Back, otherSet.m_Back);

            Kitsune::Swap(m_Compare, otherSet.m_Compare);
            Kitsune::Swap(m_Allocator, otherSet.m_Allocator);
        }

    private:
        template<typename U>
        [[nodiscard]]
        inline NodeType* CreateNode_(U&& value, Details::TreeSetNodeColor color)
        {
            void* pointer = m_Allocator.Allocate(sizeof(NodeType), alignof(NodeType));
            return Memory::ConstructAt<NodeType>(pointer, Forward<U>(value), color);
        }

    private:
        template<typename U>
        [[nodiscard]]
        inline Pair<Iterator, bool> InternalInsert_(U&& value)
        {
            auto [node, result] = BstInsert_(Forward<U>(value));
            if (result)
                FixInsertion_(node);

            return { Iterator(node, this), result };
        }

        template<typename U>
        [[nodiscard]]
        inline Pair<NodeType*, bool> BstInsert_(U&& value)
        {
            if (m_Root == nullptr)
            {
                m_Root = CreateNode_(
                    Forward<U>(value),
                    Details::TreeSetNodeColor::Black);

                ++m_Size;
                m_Front = m_Back = m_Root;
                return { m_Root, true };
            }

            NodeType* parent = m_Root;
            while (true)
            {
                bool greaterThan = m_Compare(parent->GetValue(), value);

                // Inserted value was found in the tree, 86 it.
                // if (value == parent->Value) btw.
                if (!greaterThan && !m_Compare(value, parent->GetValue()))
                    return { parent, false };

                Details::TreeSetNodeDirection direction = greaterThan ?
                    Details::TreeSetNodeDirection::Right :
                    Details::TreeSetNodeDirection::Left;

                if (parent->GetChild(direction) != nullptr)
                    parent = parent->GetChild(direction);
                else
                {
                    NodeType* node = CreateNode_(
                        Forward<U>(value),
                        Details::TreeSetNodeColor::Red);

                    parent->SetChild(direction, node);
                    node->SetParent(parent);

                    // Set m_Front, m_Back, and m_Size. because the standard
                    // specifies that GetBegin() and Size() have to be retrievable
                    // in O(1).
                    if ((parent == m_Front) && (parent->GetLeftChild() == node))
                        m_Front = node;
                    else if ((parent == m_Back) && (parent->GetRightChild()) == node)
                        m_Back = node;

                    ++m_Size;
                    return { node, true };
                }
            }
        }

        inline void FixInsertion_(NodeType* node)
        {
            // If the parent of the newly inserted node is black, no properties have
            // been violated.
            while ((node->GetParent() != nullptr) &&
                   (node->GetParent()->GetColor() == Details::TreeSetNodeColor::Red))
            {
                NodeType* grandparent = node->GetGrandparent();

                auto parentDirection =
                    (grandparent->GetLeftChild() == node->GetParent()) ?
                    Details::TreeSetNodeDirection::Left :
                    Details::TreeSetNodeDirection::Right;

                auto flipped = Details::FlipNodeDirection_(parentDirection);
                NodeType* uncle = grandparent->GetChild(flipped);

                // Case 1: Uncle is red: Recolor parent and uncle to black, grandparent
                // to red, then jump up and repeat.
                if ((uncle != nullptr) &&
                    (uncle->GetColor() == Details::TreeSetNodeColor::Red))
                {
                    node->GetParent()->SetColor(Details::TreeSetNodeColor::Black);
                    uncle->SetColor(Details::TreeSetNodeColor::Black);

                    grandparent->SetColor(Details::TreeSetNodeColor::Red);
                    node = grandparent;
                }
                else /* uncle == nullptr || uncle->Color == 0 */
                {

                    // Case 2.1: Uncle and child face the same way: Rotate the
                    // other way to correct the tree shape.
                    if (node == node->GetParent()->GetChild(flipped))
                    {
                        node = node->GetParent();
                        Rotate_(node, parentDirection);
                    }

                    // Case 2: Recolor parent to black and grandparent to red,
                    // then rotate to restore balance.
                    node->GetParent()->SetColor(Details::TreeSetNodeColor::Black);
                    node->GetGrandparent()->SetColor(Details::TreeSetNodeColor::Red);

                    Rotate_(node->GetGrandparent(), flipped);
                }
            }

            m_Root->SetColor(Details::TreeSetNodeColor::Black);
        }

        inline void Rotate_(NodeType* node, Details::TreeSetNodeDirection direction)
        {
            auto flipped = Details::FlipNodeDirection_(direction);

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

        inline void RecursiveClear_(NodeType* node)
        {
            if (node == nullptr)
                return;

            RecursiveClear_(node->GetLeftChild());
            RecursiveClear_(node->GetRightChild());

            Memory::DestroyAt(node);
            m_Allocator.Free(node, sizeof(NodeType));
        }

        inline void RecursiveCopy_(
            NodeType* parent, NodeType* node, const TreeSet& set,
            Details::TreeSetNodeDirection direction =
                Details::TreeSetNodeDirection::Left)
        {
            if (node == nullptr)
                return;

            NodeType* copy = CreateNode_(
                node->GetValue(), Details::TreeSetNodeColor::Black);

            if (m_Root == nullptr)
                m_Root = copy;
            else
            {
                copy->SetColor(node->GetColor());
                copy->SetParent(parent);

                parent->SetChild(direction, copy);
            }

            ++m_Size;
            if (node == set.m_Front)
                m_Front = node;
            else if (node == set.m_Back)
                m_Back = node;

            RecursiveCopy_(
                copy, node->GetLeftChild(),
                set, Details::TreeSetNodeDirection::Left);

            RecursiveCopy_(
                copy, node->GetRightChild(),
                set, Details::TreeSetNodeDirection::Right);
        }

    public:
        // Should not be called by engine/client code.
        // Made public so that the compiler can generate code for range-based for loops.
        inline Iterator begin() { return GetBegin(); }
        inline ConstIterator begin() const { return GetBegin(); }

        inline Iterator end() { return GetEnd(); }
        inline ConstIterator end() const { return GetEnd(); }

    private:
        template<typename U, typename SetType>
        friend class Details::TreeSetIterator;

    private:
        NodeType* m_Root = nullptr;

        Usize m_Size = 0;
        NodeType* m_Front = nullptr;
        NodeType* m_Back = nullptr;

        KITSUNE_MAYBE_OVERLAPPING Compare m_Compare;
        KITSUNE_MAYBE_OVERLAPPING Alloc m_Allocator;
    };

    template<
        typename T, InvocableReturn<bool, const T&, const T&> Compare, Allocator Alloc>
    bool operator==(const TreeSet<T, Compare, Alloc>& treeSet1,
                    const TreeSet<T, Compare, Alloc>& treeSet2)
    {
        if (treeSet1.Size() != treeSet2.Size())
            return false;

        return Algorithms::Equal(
            treeSet1.GetBegin(), treeSet1.GetEnd(), treeSet2.GetBegin());
    }
}
