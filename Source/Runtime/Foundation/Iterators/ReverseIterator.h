#pragma once

#include <type_traits>
#include "Foundation/Templates/Move.h"

#include "Foundation/Memory/AddressOf.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune
{
    template<BidirectionalIterator Iter>
    class ReverseIterator
    {
    public:
        using ValueType = IteratorTraits<Iter>::ValueType;
        using DifferenceType = IteratorTraits<Iter>::DifferenceType;

        using IteratorType = Iter;

    public:
        ReverseIterator() : Current() { /* ... */ }
        explicit ReverseIterator(Iter it)
            : Current(Move(it)) { /* ... */ }

        template<typename UIter>
            requires std::is_convertible_v<const UIter&, Iter>
        ReverseIterator(const ReverseIterator<UIter>& other)
            : Current(other.Current)
        {
        }

    public:
        template<typename UIter>
            requires std::is_convertible_v<const UIter&, Iter> &&
                     std::assignable_from<Iter&, const UIter&>
        ReverseIterator& operator=(const ReverseIterator<UIter>& other)
        {
            Current = other.Current;
            return *this;
        }


    public:
        ValueType& operator*() const { Iter temp = Current; return *(--temp); }
        ValueType* operator->() const
        {
            if constexpr (std::is_pointer_v<Iter>)
                return (Current - 1);
            else
            {
                auto curr = Current;
                return (--curr).operator->();
            }
        }

    public:
        ValueType& operator[](Index index) const
            requires RandomAccessIterator<Iter> { return *(Current - index - 1); }

    public:
        ReverseIterator& operator++() { --Current; return *this; }
        ReverseIterator& operator--() { ++Current; return *this; }

        ReverseIterator operator++(int) { ReverseIterator temp = *this; --Current; return temp; }
        ReverseIterator operator--(int) { ReverseIterator temp = *this; ++Current; return temp; }

        ReverseIterator operator+(DifferenceType offset) const
            requires RandomAccessIterator<Iter> { return ReverseIterator(Current - offset); }

        ReverseIterator& operator+=(DifferenceType offset)
            requires RandomAccessIterator<Iter> { Current -= offset; return *this; }

        ReverseIterator operator-(DifferenceType offset) const
            requires RandomAccessIterator<Iter> { return ReverseIterator(Current + offset); }

        ReverseIterator& operator-=(DifferenceType offset)
            requires RandomAccessIterator<Iter> { Current += offset; return *this; }

    public:
        [[nodiscard]] Iter GetBase() const { return Current; }

    protected:
        template<BidirectionalIterator U>
        friend class ReverseIterator;

        Iter Current;
    };

    template<BidirectionalIterator It>
    bool operator==(const ReverseIterator<It>& it1, const ReverseIterator<It>& it2)
    {
        return (it1.GetBase() == it2.GetBase());
    }

    template<BidirectionalIterator It>
    bool operator!=(const ReverseIterator<It>& it1, const ReverseIterator<It>& it2)
    {
        return (it1.GetBase() != it2.GetBase());
    }

    template<RandomAccessIterator It>
    bool operator>=(const ReverseIterator<It>& it1, const ReverseIterator<It>& it2)
    {
        return (it1.GetBase() <= it2.GetBase());
    }

    template<RandomAccessIterator It>
    bool operator<=(const ReverseIterator<It>& it1, const ReverseIterator<It>& it2)
    {
        return (it1.GetBase() >= it2.GetBase());
    }

    template<RandomAccessIterator It>
    bool operator>(const ReverseIterator<It>& it1, const ReverseIterator<It>& it2)
    {
        return (it1.GetBase() < it2.GetBase());
    }

    template<RandomAccessIterator It>
    bool operator<(const ReverseIterator<It>& it1, const ReverseIterator<It>& it2)
    {
        return (it1.GetBase() > it2.GetBase());
    }

    template<RandomAccessIterator It>
    ReverseIterator<It> operator+(typename IteratorTraits<It>::DifferenceType offset,
                                  const ReverseIterator<It>& it)
    {
        return ReverseIterator<It>(it.GetBase() - offset);
    }

    template<RandomAccessIterator It1, RandomAccessIterator It2>
    auto operator-(const ReverseIterator<It1>& it1, const ReverseIterator<It2>& it2)
        -> decltype(it2.GetBase() - it1.GetBase())
    {
        return (it2.GetBase() - it1.GetBase());
    }
}
