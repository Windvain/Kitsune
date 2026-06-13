#pragma once

#include <cstddef>
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Testing
{
    template<typename T>
    class ForwardIterator
    {
    public:
        using ValueType = T;
        using DifferenceType = std::ptrdiff_t;

    public:
        ForwardIterator() = default;
        inline explicit ForwardIterator(T* pointer)
            : m_Pointer(pointer)
        {
        }

        ~ForwardIterator() = default;

    public:
        ForwardIterator(const ForwardIterator&) = default;
        ForwardIterator& operator=(const ForwardIterator&) = default;

    public:
        inline ForwardIterator& operator++()
        {
            ++m_Pointer;
            return *this;
        }

        inline ForwardIterator operator++(int)
        {
            ForwardIterator iter = *this;
            ++(*this);

            return iter;
        }

    public:
        inline T& operator*() const { return *m_Pointer; }
        inline T* operator->() const { return m_Pointer; }

        [[nodiscard]] inline T* Pointer() const { return m_Pointer; }
        inline operator T*() const
        {
            return m_Pointer;
        }
    public:
        inline bool operator==(const ForwardIterator& iter) const
        {
            return (Pointer() == iter.Pointer());
        }

    private:
        T* m_Pointer = nullptr;
    };

    template<typename T>
    class BidirectionalIterator
    {
    public:
        using ValueType = T;
        using DifferenceType = std::ptrdiff_t;

    public:
        BidirectionalIterator() = default;
        inline explicit BidirectionalIterator(T* pointer)
            : m_Pointer(pointer)
        {
        }

    public:
        inline BidirectionalIterator& operator++()
        {
            ++m_Pointer;
            return *this;
        }

        inline BidirectionalIterator operator++(int) const
        {
            BidirectionalIterator iter = *this;
            return (++iter);
        }

        inline BidirectionalIterator& operator--()
        {
            --m_Pointer;
            return *this;
        }

        inline BidirectionalIterator operator--(int) const
        {
            BidirectionalIterator iter = *this;
            return (--iter);
        }

    public:
        inline T& operator*() const { return *m_Pointer; }
        inline T* operator->() const { return m_Pointer; }

        [[nodiscard]] inline T* Pointer() const { return m_Pointer; }
        inline operator T*() const
        {
            return m_Pointer;
        }

    public:
        inline bool operator==(const BidirectionalIterator& iter) const
        {
            return (Pointer() == iter.Pointer());
        }

    private:
        T* m_Pointer = nullptr;
    };

    template<typename T>
    class RandomAccessIterator
    {
    public:
        using ValueType = T;
        using DifferenceType = std::ptrdiff_t;

    public:
        RandomAccessIterator() = default;
        inline explicit RandomAccessIterator(T* pointer)
            : m_Pointer(pointer)
        {
        }

    public:
        inline RandomAccessIterator& operator++()
        {
            ++m_Pointer;
            return *this;
        }

        inline RandomAccessIterator operator++(int) const
        {
            RandomAccessIterator iter = *this;
            return (++iter);
        }

        inline RandomAccessIterator& operator--()
        {
            --m_Pointer;
            return *this;
        }

        inline RandomAccessIterator operator--(int) const
        {
            RandomAccessIterator iter = *this;
            return (--iter);
        }

        inline RandomAccessIterator& operator+=(DifferenceType offset)
        {
            m_Pointer += offset;
            return *this;
        }

        inline RandomAccessIterator operator+(DifferenceType offset) const
        {
            RandomAccessIterator iter = *this;
            return (iter += offset);
        }

        inline RandomAccessIterator& operator-=(DifferenceType offset)
        {
            m_Pointer -= offset;
            return *this;
        }

        inline RandomAccessIterator operator-(DifferenceType offset) const
        {
            RandomAccessIterator iter = *this;
            return (iter -= offset);
        }

        inline DifferenceType operator-(RandomAccessIterator iterator) const
        {
            return (m_Pointer - iterator.m_Pointer);
        }

        inline T& operator[](DifferenceType index) const
        {
            return m_Pointer[index];
        }

    public:
        inline T& operator*() const { return *m_Pointer; }
        inline T* operator->() const { return m_Pointer; }

        [[nodiscard]] inline T* Pointer() const { return m_Pointer; }
        inline operator T*() const
        {
            return m_Pointer;
        }

    public:
        inline bool operator==(const RandomAccessIterator& iter) const
        {
            return (Pointer() == iter.Pointer());
        }

        inline bool operator>(const RandomAccessIterator& iter) const
        {
            return (Pointer() > iter.Pointer());
        }

        inline bool operator<(const RandomAccessIterator& iter) const
        {
            return (Pointer() < iter.Pointer());
        }

        inline bool operator>=(const RandomAccessIterator& iter) const
        {
            return (Pointer() >= iter.Pointer());
        }

        inline bool operator<=(const RandomAccessIterator& iter) const
        {
            return (Pointer() <= iter.Pointer());
        }

    private:
        T* m_Pointer = nullptr;
    };

    template<typename T>
    inline RandomAccessIterator<T> operator+(
        std::ptrdiff_t offset, const RandomAccessIterator<T>& iter)
    {
        RandomAccessIterator<T> copy = iter;
        return (copy += offset);
    }

    static_assert(
        Kitsune::ForwardIterator<ForwardIterator<int>>,
        "The test forward iterator doesn't satisfy the conditions of "
        "ForwardIterator.");

    static_assert(
        Kitsune::BidirectionalIterator<BidirectionalIterator<int>>,
        "The test bidirectional iterator doesn't satisfy the conditions of "
        "BidirectionalIterator.");

    static_assert(
        Kitsune::RandomAccessIterator<RandomAccessIterator<int>>,
        "The test random access iterator doesn't satisfy the conditions of "
        "RandomAccessIterator.");
}
