#pragma once

#include <type_traits>

#include "Foundation/Common/Types.h"
#include "Foundation/Templates/Move.h"

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
        inline ReverseIterator()
            : m_Current()
        {
        }

        inline explicit ReverseIterator(Iter iterator)
            : m_Current(Move(iterator))
        {
        }

        template<typename OtherIter>
            requires std::is_convertible_v<const OtherIter&, Iter>
        inline ReverseIterator(const ReverseIterator<OtherIter>& other)
            : m_Current(other.m_Current)
        {
        }

    public:
        template<typename OtherIter>
            requires std::is_convertible_v<const OtherIter&, Iter> &&
                     std::assignable_from<Iter&, const OtherIter&>
        inline ReverseIterator& operator=(const ReverseIterator<OtherIter>& other)
        {
            m_Current = other.m_Current;
            return *this;
        }

    public:
        inline ValueType& operator*() const
        {
            Iter temp = m_Current;
            return *(--temp);
        }

        inline ValueType* operator->() const
        {
            if constexpr (std::is_pointer_v<Iter>)
                return (m_Current - 1);
            else
            {
                auto curr = m_Current;
                return (--curr).operator->();
            }
        }

    public:
        inline ValueType& operator[](Index index) const
            requires RandomAccessIterator<Iter>
        {
            return *(m_Current - index - 1);
        }

    public:
        inline ReverseIterator& operator++()
        {
            --m_Current;
            return *this;
        }

        inline ReverseIterator& operator--()
        {
            ++m_Current;
            return *this;
        }

        inline ReverseIterator operator++(int)
        {
            ReverseIterator temp = *this;
            --m_Current;

            return temp;
        }

        inline ReverseIterator operator--(int)
        {
            ReverseIterator temp = *this;
            ++m_Current;

            return temp;
        }

        inline ReverseIterator operator+(DifferenceType offset) const
            requires RandomAccessIterator<Iter>
        {
            return ReverseIterator(m_Current - offset);
        }

        inline ReverseIterator& operator+=(DifferenceType offset)
            requires RandomAccessIterator<Iter>
        {
            m_Current -= offset;
            return *this;
        }

        inline ReverseIterator operator-(DifferenceType offset) const
            requires RandomAccessIterator<Iter>
        {
            return ReverseIterator(m_Current + offset);
        }

        inline ReverseIterator& operator-=(DifferenceType offset)
            requires RandomAccessIterator<Iter>
        {
            m_Current += offset;
            return *this;
        }

    public:
        [[nodiscard]]
        inline Iter GetBase() const
        {
            return m_Current;
        }

    private:
        template<BidirectionalIterator U>
        friend class ReverseIterator;

        Iter m_Current;
    };

    template<BidirectionalIterator Iter>
    inline bool operator==(const ReverseIterator<Iter>& iter1,
                           const ReverseIterator<Iter>& iter2)
    {
        return (iter1.GetBase() == iter2.GetBase());
    }

    template<BidirectionalIterator Iter>
    inline bool operator!=(const ReverseIterator<Iter>& iter1,
                           const ReverseIterator<Iter>& iter2)
    {
        return (iter1.GetBase() != iter2.GetBase());
    }

    template<RandomAccessIterator Iter>
    inline bool operator>=(const ReverseIterator<Iter>& iter1,
                           const ReverseIterator<Iter>& iter2)
    {
        return (iter1.GetBase() <= iter2.GetBase());
    }

    template<RandomAccessIterator Iter>
    inline bool operator<=(const ReverseIterator<Iter>& iter1,
                           const ReverseIterator<Iter>& iter2)
    {
        return (iter1.GetBase() >= iter2.GetBase());
    }

    template<RandomAccessIterator Iter>
    inline bool operator>(const ReverseIterator<Iter>& iter1,
                          const ReverseIterator<Iter>& iter2)
    {
        return (iter1.GetBase() < iter2.GetBase());
    }

    template<RandomAccessIterator Iter>
    inline bool operator<(const ReverseIterator<Iter>& iter1,
                          const ReverseIterator<Iter>& iter2)
    {
        return (iter1.GetBase() > iter2.GetBase());
    }

    template<RandomAccessIterator Iter>
    inline ReverseIterator<Iter> operator+(
        typename IteratorTraits<Iter>::DifferenceType offset,
        const ReverseIterator<Iter>& it)
    {
        return ReverseIterator<Iter>(it.GetBase() - offset);
    }

    template<RandomAccessIterator Iter1, RandomAccessIterator Iter2>
    inline typename IteratorTraits<Iter2>::DifferenceType operator-(
        const ReverseIterator<Iter1>& iter1,
        const ReverseIterator<Iter2>& iter2)
    {
        return (iter2.GetBase() - iter1.GetBase());
    }
}
