#pragma once

#include <cstddef>

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
        explicit ForwardIterator(T* pointer)
            : m_Pointer(pointer)
        {
        }

        ~ForwardIterator() = default;

    public:
        ForwardIterator(const ForwardIterator&) = default;
        ForwardIterator& operator=(const ForwardIterator&) = default;

    public:
        ForwardIterator& operator++()
        {
            ++m_Pointer;
            return *this;
        }

        ForwardIterator operator++(int)
        {
            ForwardIterator iter = *this;
            ++(*this);

            return iter;
        }

    public:
        T& operator*() const { return *m_Pointer; }
        T* operator->() const { return m_Pointer; }

        T* Pointer() const { return m_Pointer; }

    public:
        bool operator==(const ForwardIterator& iter) const
        {
            return (Pointer() == iter.Pointer());
        }

    private:
        T* m_Pointer;
    };

    template<typename T>
    class BidirectionalIterator
    {
    public:
        using ValueType = T;
        using DifferenceType = std::ptrdiff_t;

    public:
        BidirectionalIterator() = default;
        explicit BidirectionalIterator(T* pointer)
            : m_Pointer(pointer)
        {
        }

    public:
        BidirectionalIterator& operator++()
        {
            ++m_Pointer;
            return *this;
        }

        BidirectionalIterator operator++(int) const
        {
            BidirectionalIterator iter;
            return (++iter);
        }

        BidirectionalIterator& operator--()
        {
            --m_Pointer;
            return *this;
        }

        BidirectionalIterator operator--(int) const
        {
            BidirectionalIterator iter;
            return (--iter);
        }

    public:
        T& operator*() const { return *m_Pointer; }
        T* operator->() const { return m_Pointer; }

        T* Pointer() const { return m_Pointer; }

    public:
        bool operator==(const BidirectionalIterator& iter) const
        {
            return (Pointer() == iter.Pointer());
        }

    private:
        T* m_Pointer;
    };

    template<typename T>
    class RandomAccessIterator
    {
    public:
        using ValueType = T;
        using DifferenceType = std::ptrdiff_t;

    public:
        RandomAccessIterator() = default;
        explicit RandomAccessIterator(T* pointer)
            : m_Pointer(pointer)
        {
        }

    public:
        RandomAccessIterator& operator++()
        {
            ++m_Pointer;
            return *this;
        }

        RandomAccessIterator operator++(int) const
        {
            RandomAccessIterator iter;
            return (++iter);
        }

        RandomAccessIterator& operator--()
        {
            --m_Pointer;
            return *this;
        }

        RandomAccessIterator operator--(int) const
        {
            RandomAccessIterator iter;
            return (--iter);
        }

        RandomAccessIterator& operator+=(DifferenceType offset)
        {
            m_Pointer += offset;
            return *this;
        }

        RandomAccessIterator operator+(DifferenceType offset) const
        {
            RandomAccessIterator iter;
            return (iter += offset);
        }

        RandomAccessIterator& operator-=(DifferenceType offset)
        {
            m_Pointer -= offset;
            return *this;
        }

        RandomAccessIterator operator-(DifferenceType offset) const
        {
            RandomAccessIterator iter;
            return (iter -= offset);
        }

        T& operator[](DifferenceType index)
        {
            return m_Pointer[index];
        }

        const T& operator[](DifferenceType index) const
        {
            return m_Pointer[index];
        }

    public:
        T& operator*() const { return *m_Pointer; }
        T* operator->() const { return m_Pointer; }

        T* Pointer() const { return m_Pointer; }

    public:
        bool operator==(const RandomAccessIterator& iter) const
        {
            return (Pointer() == iter.Pointer());
        }

        bool operator>(const RandomAccessIterator& iter) const
        {
            return (Pointer() > iter.Pointer());
        }

        bool operator<(const RandomAccessIterator& iter) const
        {
            return (Pointer() < iter.Pointer());
        }

        bool operator>=(const RandomAccessIterator& iter) const
        {
            return (Pointer() >= iter.Pointer());
        }

        bool operator<=(const RandomAccessIterator& iter) const
        {
            return (Pointer() <= iter.Pointer());
        }

    private:
        T* m_Pointer;
    };

    template<typename T>
    RandomAccessIterator<T> operator+(
        std::ptrdiff_t offset, const RandomAccessIterator<T>& iter)
    {
        RandomAccessIterator<T> copy = iter;
        return (copy += offset);
    }
}
