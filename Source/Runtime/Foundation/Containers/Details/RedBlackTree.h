#pragma once

#include "BinarySearchTree.h"
#include "Foundation/Diagnostics/Assert.h"

namespace Kitsune::Details
{
    enum class RBTColor { Red, Black };

    // Base class is for Map implementations.
    template<typename Key, typename Mapped, bool IsSet = std::is_void_v<Mapped>>
    class RBTStorage
    {
    public:
        using ValueType = Pair<const Key, Mapped>;

        using KeyType = const Key;
        using MappedType = Mapped;

    public:
        [[nodiscard]]
        inline MappedType& GetMapped() { return Value.Second; }

        [[nodiscard]]
        inline const MappedType& GetMapped() const { return Value.Second; }

        [[nodiscard]]
        inline const Key& GetKey() const
        {
            return Value.First;
        }

    // Aggregate initialization, do not make private.
    public:
        ValueType Value;
        RBTColor Color;
    };

    template<typename Key, typename Mapped>
    class RBTStorage<Key, Mapped, true>
    {
    public:
        using ValueType = const Key;

        using KeyType = const Key;
        using MappedType = const Key;

    public:
        [[nodiscard]] inline const MappedType& GetMapped() const { return Value; }
        [[nodiscard]] inline const Key& GetKey() const { return Value; }

    // Aggregate initialization, do not make private.
    public:
        ValueType Value;
        RBTColor Color;
    };

    template<typename Key, typename Mapped>
    class RBTNode : public RBTStorage<Key, Mapped>
    {
    private:
        using StorageType = RBTStorage<Key, Mapped>;

    public:
        using ValueType = typename StorageType::ValueType;

        using KeyType = typename StorageType::KeyType;
        using MappedType = typename StorageType::MappedType;

    public:
        inline explicit RBTNode(ValueType&& value, RBTColor color)
            : StorageType{ Move(value), color }
        {
        }

    public:
        [[nodiscard]]
        inline RBTNode* GetChild(BSTDirection directon) const
        {
            return m_Children[directon];
        }

        [[nodiscard]] inline RBTNode* GetLeft() const { return m_Children[0]; }
        [[nodiscard]] inline RBTNode* GetRight() const { return m_Children[1]; }

    public:
        inline void SetChild(BSTDirection direction, RBTNode* node)
        {
            m_Children[direction] = node;
        }

        inline void SetLeft(RBTNode* node) { m_Children[0] = node; }
        inline void SetRight(RBTNode* node) { m_Children[1] = node; }

    public:
        inline void SetParent(RBTNode* node) { m_Parent = node; }
        [[nodiscard]] inline RBTNode* GetParent() const { return m_Parent; }

        inline RBTNode* GetGrandparent()
        {
            KITSUNE_ASSERT(
                m_Parent != nullptr,
                "Tried to get the grandparent of a node which doesn't have a "
                "parent.");

            return m_Parent->m_Parent;
        }

    private:
        RBTNode* m_Parent = nullptr;
        RBTNode* m_Children[2] = { nullptr, nullptr };
    };

    template<
        typename Key, typename Mapped,
        InvocableReturn<bool, const Key&, const Key&> Compare,
        Allocator Alloc>
    class RedBlackTree :
        public BasicBinarySearchTree<RBTNode<Key, Mapped>, Compare, Alloc>
    {
    private:
        using ThisType = RedBlackTree<Key, Mapped, Compare, Alloc>;
        using BaseType = BasicBinarySearchTree<
            RBTNode<Key, Mapped>, Compare, Alloc>;

        using NodeType = RBTNode<Key, Mapped>;

    public:
        using ValueType = typename BaseType::ValueType;

        /* KeyType and MappedType should be redefined by the inheriting container. */

        using CompareType = Compare;
        using AllocatorType = Alloc;

        using Iterator = typename BaseType::Iterator;
        using ConstIterator = typename BaseType::ConstIterator;

    public:
        inline RedBlackTree() = default;
        inline RedBlackTree(const Compare& compare, const Alloc& allocator)
            : BaseType(compare, allocator)
        {
        }

        RedBlackTree(const RedBlackTree&) = default;
        RedBlackTree(RedBlackTree&&) = default;

        ~RedBlackTree() = default;

    public:
        RedBlackTree& operator=(const RedBlackTree&) = default;
        RedBlackTree& operator=(RedBlackTree&&) = default;

    public:
        inline Pair<Iterator, bool> Insert(const ValueType& value)
        {
            return Emplace(value);
        }

        inline Pair<Iterator, bool> Insert(ValueType&& value)
        {
            return Emplace(Move(value));
        }

        template<typename... Args>
            requires std::constructible_from<ValueType, Args...>
        inline Pair<Iterator, bool> Emplace(Args&&... args)
        {
            NodeType node(ValueType(Forward<Args>(args)...), RBTColor::Red);
            auto [pointer, success] = BaseType::Insert(Move(node));

            if (success)
                FixInsertion(pointer);

            return { Iterator(pointer, this), success };
        }

    private:
        inline void FixInsertion(NodeType* node)
        {
            // If the parent of the newly inserted node is black, no properties have
            // been violated.
            while ((node->GetParent() != nullptr) &&
                   (node->GetParent()->Color == RBTColor::Red))
            {
                NodeType* grandparent = node->GetGrandparent();
                auto parentDir = (grandparent->GetLeft() == node->GetParent()) ?
                    BSTDirection::Left :
                    BSTDirection::Right;

                BSTDirection flipped = Flip(parentDir);
                NodeType* uncle = grandparent->GetChild(flipped);

                // Case 1: Uncle is red: Recolor parent and uncle to black, grandparent
                // to red, then jump up and repeat.
                if ((uncle != nullptr) && (uncle->Color == RBTColor::Red))
                {
                    node->GetParent()->Color = RBTColor::Black;
                    uncle->Color = RBTColor::Black;

                    grandparent->Color = RBTColor::Red;
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
                    node->GetParent()->Color = RBTColor::Black;
                    node->GetGrandparent()->Color = RBTColor::Red;

                    Rotate(node->GetGrandparent(), flipped);
                }
            }

            BaseType::m_Root->Color = RBTColor::Black;
        }

        inline void Rotate(NodeType* node, BSTDirection direction)
        {
            BSTDirection flipped = Flip(direction);

            NodeType* promoted = node->GetChild(flipped);
            node->SetChild(flipped, promoted->GetChild(direction));

            if (promoted->GetChild(direction) != nullptr)
                promoted->GetChild(direction)->SetParent(node);

            promoted->SetParent(node->GetParent());
            if (node->GetParent() == nullptr)
                BaseType::m_Root = promoted;
            else if (node == node->GetParent()->GetLeft())
                node->GetParent()->SetLeft(promoted);
            else
                node->GetParent()->SetRight(promoted);

            promoted->SetChild(direction, node);
            node->SetParent(promoted);
        }

        inline BSTDirection Flip(BSTDirection direction)
        {
            return BSTDirection(!bool(direction));
        }
    };
}
