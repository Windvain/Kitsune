#pragma once

#include <initializer_list>
#include "Foundation/Containers/Pair.h"
#include "Foundation/Concepts/Invocable.h"

#include "Foundation/Templates/Exchange.h"
#include "Foundation/Utilities/Comparators.h"

#include "Foundation/Memory/Allocator.h"
#include "Foundation/Memory/GlobalAllocator.h"

namespace Kitsune
{
    namespace Details
    {
        enum class SetNodeColor
        {
            Red,
            Black
        };

        template<typename T>
        class SetNode
        {
        public:
            inline SetNode(T&& value, SetNodeColor color)
                : Value(value), Color(color), Parent(nullptr),
                  Children()
            {
            }

        public:
            T Value;
            SetNodeColor Color;

            SetNode<T>* Parent;
            SetNode<T>* Children[2];     // Direction: 0 -> Left, 1 -> Right.
        };

        template<typename T, typename SetType>
        class SetIterator
        {
        public:
            using ValueType = const T;
            using DifferenceType = Ptrdiff;

            using NodeType = typename SetType::NodeType;

        public:
            inline SetIterator()
                : m_Current(), m_Set()
            {
            }

            inline SetIterator(NodeType* node, const SetType* set)
                : m_Current(node), m_Set(set)
            {
            }

        public:
            inline const T& operator*() const
            {
                return m_Current->Value;
            }

            inline const NodeType* GetCurrent() const
            {
                return m_Current;
            }

        public:
            inline SetIterator& operator++()
            {
                if (m_Current == nullptr)
                    return *this;

                if (m_Current == m_Set->m_Back)
                {
                    m_Current = nullptr;
                    return *this;
                }

                if (m_Current->Children[1] != nullptr)
                {
                    m_Current = m_Current->Children[1];
                    while (m_Current->Children[0] != nullptr)
                        m_Current = m_Current->Children[0];
                }
                else
                {
                    NodeType* parent = m_Current->Parent;
                    while (parent && (parent->Children[1] == m_Current))
                    {
                        m_Current = parent;
                        parent = parent->Parent;
                    }

                    m_Current = parent;
                }

                return *this;
            }

            inline SetIterator operator++(int)
            {
                SetIterator copy = *this;
                ++(*this);

                return copy;
            }

        private:
            NodeType* m_Current;
            const SetType* m_Set;
        };

        template<typename T, typename SetType>
        inline bool operator==(const SetIterator<T, SetType>& iter1,
                               const SetIterator<T, SetType>& iter2)
        {
            return (iter1.GetCurrent() == iter2.GetCurrent());
        }
    }

    template<
        typename T,
        InvocableReturn<bool, const T&, const T&> Compare = LessFunctor<T>,
        Allocator Alloc = GlobalAllocator>
    class Set
    {
    private:
        using ThisType_ = Set<T, Compare, Alloc>;

    public:
        using ValueType = T;
        using NodeType = Details::SetNode<T>;

        using CompareType = Compare;
        using AllocatorType = Alloc;

        using Iterator = Details::SetIterator<T, ThisType_>;
        using ConstIterator = Details::SetIterator<const T, ThisType_>;

    public:
        inline Set()
            : m_Compare(), m_Allocator()
        {
        }

        inline explicit Set(const Compare& compare,
                            const Alloc& allocator = Alloc())
            : m_Compare(compare), m_Allocator(allocator)
        {
        }

        inline explicit Set(const Alloc& allocator)
            : Set(Compare(), allocator)
        {
        }

        template<ForwardIterator Iter>
        inline Set(Iter begin, Iter end,
                   const Compare& compare = Compare(),
                   const Alloc& allocator = Alloc())
            : m_Compare(compare), m_Allocator(allocator)
        {
            Insert(begin, end);
        }

        template<ForwardIterator Iter>
        inline Set(Iter begin, Iter end, const Alloc& allocator)
            : Set(begin, end, Compare(), allocator)
        {
        }

        inline Set(const Set& set)
            : m_Compare(set.m_Compare), m_Allocator(set.m_Allocator)
        {
            RecursiveCopy_(set.m_Root);
        }

        inline Set(Set&& set)
            : m_Root(Exchange(set.m_Root, nullptr)),
              m_Size(Exchange(set.m_Size, 0)),
              m_Front(Exchange(set.m_Front, nullptr)),
              m_Back(Exchange(set.m_Back, nullptr)),
              m_Compare(Move(set.m_Compare)),
              m_Allocator(Move(set.m_Allocator))
        {
        }

        inline Set(std::initializer_list<T> initList,
                   const Compare& compare = Compare(),
                   const Alloc& allocator = Alloc())
            : m_Compare(compare), m_Allocator(allocator)
        {
            Insert(initList);
        }

        inline Set(std::initializer_list<T> initList,
                   const Alloc& allocator)
            : Set(initList, Compare(), allocator)
        {
        }

        inline ~Set()
        {
            RecursiveClear_(m_Root);
        }

    public:
        inline Set& operator=(const Set& set)
        {
            if (this == &set)
                return *this;

            Clear();
            m_Allocator = set.m_Allocator;
            m_Compare = set.m_Compare;

            RecursiveCopy_(set.m_Root);
            return *this;
        }

        inline Set& operator=(Set&& set)
        {
            if (this == &set)
                return *this;

            Set(Move(set)).Swap(*this);
            return *this;
        }

        inline Set& operator=(std::initializer_list<T> initList)
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

        [[nodiscard]]
        inline Iterator GetEnd()
        {
            return Iterator();
        }

        [[nodiscard]]
        inline ConstIterator GetEnd() const
        {
            return ConstIterator();
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

        [[nodiscard]]
        inline Alloc& GetAllocator()
        {
            return m_Allocator;
        }

        [[nodiscard]]
        inline const Alloc& GetAllocator() const
        {
            return m_Allocator;
        }

    public:
        inline void Swap(Set& set)
        {
            Kitsune::Swap(m_Root, set.m_Root);
            Kitsune::Swap(m_Size, set.m_Size);

            Kitsune::Swap(m_Front, set.m_Front);
            Kitsune::Swap(m_Back, set.m_Back);

            Kitsune::Swap(m_Compare, set.m_Compare);
            Kitsune::Swap(m_Allocator, set.m_Allocator);
        }

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

    private:
        template<typename... Args>
        inline NodeType* CreateNode_(Args&&... args)
        {
            void* pointer = m_Allocator.Allocate(sizeof(NodeType), alignof(NodeType));
            T value{ Forward<Args>(args)... };

            return Memory::ConstructAt<NodeType>(
                pointer,
                Move(value),
                Details::SetNodeColor::Red);
        }

        inline void DeleteNode_(NodeType* node)
        {
            Memory::DestroyAt(node);
            m_Allocator.Free(node, sizeof(NodeType));
        }

        inline void Rotate_(NodeType* node, bool direction)
        {
            NodeType* promoted = node->Children[!direction];
            node->Children[!direction] = promoted->Children[direction];

            if (promoted->Children[direction] != nullptr)
                promoted->Children[direction]->Parent = node;

            promoted->Parent = node->Parent;
            if (node->Parent == nullptr)
                m_Root = promoted;
            else if (node == node->Parent->Children[0])
                node->Parent->Children[0] = promoted;
            else
                node->Parent->Children[1] = promoted;

            promoted->Children[direction] = node;
            node->Parent = promoted;
        }

        template<typename U>
        inline Pair<NodeType*, bool> BstInsert_(U&& value)
        {
            if (m_Root == nullptr)
            {
                m_Root = CreateNode_(Move(value));
                m_Root->Color = Details::SetNodeColor::Black;

                m_Front = m_Back = m_Root;
                ++m_Size;

                return { m_Root, true };
            }

            NodeType* parent = m_Root;
            while (true)
            {
                bool direction = m_Compare(parent->Value, value);

                // Inserted value was found in the tree, 86 it.
                if (!direction && !m_Compare(value, parent->Value))
                    return { parent, false };

                if (parent->Children[direction] != nullptr)
                    parent = parent->Children[direction];
                else
                {
                    NodeType* node = CreateNode_(Forward<U>(value));
                    parent->Children[direction] = node;
                    node->Parent = parent;

                    // Set m_Front, m_Back, and m_Size.. because the standard
                    // specifies that GetBegin() and Size() have to be retrievable
                    // in O(1).
                    if (m_Compare(node->Value, m_Front->Value))
                        m_Front = node;

                    if (m_Compare(m_Back->Value, node->Value))
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
            while ((node->Parent != nullptr) &&
                   (node->Parent->Color == Details::SetNodeColor::Red))
            {
                NodeType* grandparent = node->Parent->Parent;

                bool parentDirection = (grandparent->Children[0] != node->Parent);
                NodeType* uncle = grandparent->Children[!parentDirection];

                // Case 1: Uncle is red: Recolor parent and uncle to black, grandparent
                // to red, then jump up and repeat.
                if ((uncle != nullptr) &&
                    (uncle->Color == Details::SetNodeColor::Red))
                {
                    node->Parent->Color = Details::SetNodeColor::Black;
                    uncle->Color = Details::SetNodeColor::Black;

                    grandparent->Color = Details::SetNodeColor::Red;
                    node = grandparent;
                }
                else /* uncle == nullptr || uncle->Color == 0 */
                {
                    // Case 2.1: Uncle and child face the same way: Rotate the
                    // other way to correct the tree shape.
                    if (node == node->Parent->Children[!parentDirection])
                    {
                        node = node->Parent;
                        Rotate_(node, parentDirection);
                    }

                    // Case 2: Recolor parent to black and grandparent to red,
                    // then rotate to restore balance.
                    node->Parent->Color = Details::SetNodeColor::Black;
                    node->Parent->Parent->Color = Details::SetNodeColor::Red;

                    Rotate_(node->Parent->Parent, !parentDirection);
                }
            }

            m_Root->Color = Details::SetNodeColor::Black;
        }

    private:
        inline void RecursiveClear_(NodeType* node)
        {
            if (node == nullptr)
                return;

            RecursiveClear_(node->Children[0]);
            RecursiveClear_(node->Children[1]);

            DeleteNode_(node);
        }

        inline void RecursiveCopy_(NodeType* node)
        {
            if (node == nullptr)
                return;

            // This can be optimized, because the copied tree has no need for fixing,
            // because it is already in a state of no violations.
            //
            // Not doing it right now tho. TODO!
            Insert(node->Value);

            RecursiveCopy_(node->Children[0]);
            RecursiveCopy_(node->Children[1]);
        }

        template<typename U>
        inline Pair<Iterator, bool> InternalInsert_(U&& value)
        {
            auto [node, result] = BstInsert_(Forward<U>(value));
            if (result)
                FixInsertion_(node);

            return { Iterator(node, this), result };
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
        friend class Details::SetIterator;

    private:
        Details::SetNode<T>* m_Root = nullptr;
        Usize m_Size = 0;

        Details::SetNode<T>* m_Front = nullptr;
        Details::SetNode<T>* m_Back = nullptr;

        KITSUNE_MAYBE_OVERLAPPING Compare m_Compare;
        KITSUNE_MAYBE_OVERLAPPING Alloc m_Allocator;
    };
}
