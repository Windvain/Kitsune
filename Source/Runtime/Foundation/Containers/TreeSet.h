#pragma once

#include <initializer_list>
#include "Foundation/Algorithms/Equal.h"

#include "Foundation/Containers/RBTree.h"

namespace Kitsune
{
    template<
        typename T,
        InvocableReturn<bool, const T&, const T&> Compare = LessFunctor<T>,
        Allocator Alloc = GlobalAllocator>
    class TreeSet : public RBTree<T, void, Compare, Alloc>
    {
    private:
        using BaseType = RBTree<T, void, Compare, Alloc>;

    public:
        using ValueType = T;
        using CompareType = Compare;
        using AllocatorType = Alloc;

        using Iterator = typename BaseType::Iterator;
        using ConstIterator = typename BaseType::ConstIterator;

    public:
        inline TreeSet() = default;
        inline explicit TreeSet(const Compare& compare,
                                const Alloc& allocator = Alloc())
            : BaseType(compare, allocator)
        {
        }

        inline explicit TreeSet(const Alloc& allocator)
            : TreeSet(Compare(), allocator)
        {
        }

        template<ForwardIterator Iter>
        inline TreeSet(
            Iter begin, Iter end,
            const Compare& compare = Compare(),
            const Alloc& allocator = Alloc())
            : BaseType(compare, allocator)
        {
            Insert(begin, end);
        }

        template<ForwardIterator Iter>
        inline TreeSet(Iter begin, Iter end, const Alloc& allocator)
            : TreeSet(begin, end, Compare(), allocator)
        {
        }

        inline TreeSet(const TreeSet&) = default;
        inline TreeSet(TreeSet&& set) = default;

        inline TreeSet(
            std::initializer_list<T> initList,
            const Compare& compare = Compare(),
            const Alloc& allocator = Alloc())
            : BaseType(compare, allocator)
        {
            Insert(initList);
        }

        inline TreeSet(std::initializer_list<T> initList, const Alloc& allocator)
            : TreeSet(initList, Compare(), allocator)
        {
        }

        inline ~TreeSet() = default;

    public:
        inline TreeSet& operator=(const TreeSet&) = default;
        inline TreeSet& operator=(TreeSet&&) = default;

        inline TreeSet& operator=(std::initializer_list<T> initList)
        {
            BaseType::Clear();
            Insert(initList);

            return *this;
        }

    public:
        inline Pair<Iterator, bool> Insert(const T& value)
        {
            return BaseType::Insert(value);
        }

        inline Pair<Iterator, bool> Insert(T&& value)
        {
            return BaseType::Insert(Move(value));
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
    };

    template<
        typename T,
        InvocableReturn<bool, const T&, const T&> Compare,
        Allocator Alloc>
    inline bool operator==(const TreeSet<T, Compare, Alloc>& treeSet1,
                           const TreeSet<T, Compare, Alloc>& treeSet2)
    {
        if (treeSet1.Size() != treeSet2.Size())
            return false;

        return Algorithms::Equal(
            treeSet1.GetBegin(), treeSet1.GetEnd(),
            treeSet2.GetBegin());
    }
}
