#pragma once

#include <initializer_list>
#include "Foundation/Algorithms/Equal.h"

#include "Foundation/Containers/RBTree.h"
#include "Foundation/Diagnostics/OutOfRangeException.h"

namespace Kitsune
{
    template<
        typename Key, typename Value,
        InvocableReturn<bool, const Key&, const Key&> Compare = LessThanFunctor<Key>,
        Allocator Alloc = GlobalAllocator>
    class TreeMap : public RBTree<Key, Value, Compare, Alloc>
    {
    private:
        using BaseType = RBTree<Key, Value, Compare, Alloc>;
        static_assert(!std::is_void_v<Value>, "The MappedType should not be void.");

    public:
        using KeyType = Key;
        using MappedType = Value;
        using ValueType = typename BaseType::ValueType;

        using CompareType = Compare;
        using AllocatorType = Alloc;

        using Iterator = typename BaseType::Iterator;
        using ConstIterator = typename BaseType::ConstIterator;

    public:
        // The compiler couldn't find these functions from the base class.
        // Bring them over to this scope.
        using BaseType::Insert, BaseType::Clear, BaseType::Find;
        using BaseType::GetBegin, BaseType::GetEnd;

    public:
        inline TreeMap() = default;
        inline explicit TreeMap(const Compare& compare,
                                const Alloc& allocator = Alloc())
            : BaseType(compare, allocator)
        {
        }

        inline explicit TreeMap(const Alloc& allocator)
            : TreeMap(Compare(), allocator)
        {
        }

        template<ForwardIterator Iter>
        inline TreeMap(
            Iter begin, Iter end,
            const Compare& compare = Compare(),
            const Alloc& allocator = Alloc())
            : BaseType(compare, allocator)
        {
            Insert(begin, end);
        }

        template<ForwardIterator Iter>
        inline TreeMap(Iter begin, Iter end, const Alloc& allocator)
            : TreeMap(begin, end, Compare(), allocator)
        {
        }

        inline TreeMap(const TreeMap&) = default;
        inline TreeMap(TreeMap&& set) = default;

        inline TreeMap(
            std::initializer_list<ValueType> initList,
            const Compare& compare = Compare(),
            const Alloc& allocator = Alloc())
            : BaseType(compare, allocator)
        {
            Insert(initList);
        }

        inline TreeMap(std::initializer_list<ValueType> initList, const Alloc& allocator)
            : TreeMap(initList, Compare(), allocator)
        {
        }

        inline ~TreeMap() = default;

    public:
        inline TreeMap& operator=(const TreeMap&) = default;
        inline TreeMap& operator=(TreeMap&&) = default;

        inline TreeMap& operator=(std::initializer_list<ValueType> initList)
        {
            Clear();
            Insert(initList);

            return *this;
        }

    public:
        inline MappedType& operator[](const Key& key)
        {
            Iterator iter = Find(key);
            if (iter == GetEnd())
                throw OutOfRangeException();

            return (*iter).Second;
        }

        inline const Value& operator[](const Key& key) const
        {
            ConstIterator iter = Find(key);
            if (iter == GetEnd())
                throw OutOfRangeException();

            return (*iter).Second;
        }

    public:
        template<ForwardIterator Iter>
        inline void Insert(Iter begin, Iter end)
        {
            for (; begin != end; ++begin)
                Insert(*begin);
        }

        inline void Insert(std::initializer_list<ValueType> initList)
        {
            for (auto iter = initList.begin(); iter != initList.end(); ++iter)
                Insert(*iter);
        }

    public:
        template<typename T>
        inline Pair<Iterator, bool> InsertOrAssign(const Key& key, T&& value)
        {
            return InternalInsertOrAssign(key, Forward<T>(value));
        }

        template<typename T>
        inline Pair<Iterator, bool> InsertOrAssign(Key&& key, T&& value)
        {
            return InternalInsertOrAssign(Move(key), Forward<T>(value));
        }

    public:
        inline void Swap(TreeMap& map)
        {
            // Hide any signatures from RBTree.
            return BaseType::Swap(map);
        }

    private:
        template<class K, class T>
        inline Pair<Iterator, bool> InternalInsertOrAssign(K&& key, T&& value)
        {
            auto iter = Find(key);
            if (iter == GetEnd())
                return Insert({ Forward<K>(key), Forward<T>(value) });
            else
            {
                (*iter).Second = Forward<T>(value);
                return { iter, false };
            }
        }
    };

    template<
        typename Key, typename T,
        InvocableReturn<bool, const T&, const T&> Compare,
        Allocator Alloc>
    inline bool operator==(const TreeMap<Key, T, Compare, Alloc>& map1,
                           const TreeMap<Key, T, Compare, Alloc>& map2)
    {
        if (map1.Size() != map2.Size())
            return false;

        return Algorithms::Equal(
            map1.GetBegin(), map1.GetEnd(),
            map2.GetBegin());
    }
}
