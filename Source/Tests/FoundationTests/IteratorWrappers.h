#pragma once

namespace Testing
{
    template<typename T>
    class ForwardIteratorWrapper
    {
    public:
        using ValueType = T;
        using DifferenceType = std::ptrdiff_t;

    public:
        ForwardIteratorWrapper() = default;
        explicit ForwardIteratorWrapper(T* ptr)
            : m_Pointer(ptr)
        {
        }

        ForwardIteratorWrapper(const ForwardIteratorWrapper&) = default;
        ForwardIteratorWrapper(ForwardIteratorWrapper&&) = default;

        ~ForwardIteratorWrapper() = default;

    public:
        ForwardIteratorWrapper& operator=(const ForwardIteratorWrapper&) = default;
        ForwardIteratorWrapper& operator=(ForwardIteratorWrapper&&) = default;

    public:
        ForwardIteratorWrapper& operator++() { ++m_Pointer; return *this; }
        ForwardIteratorWrapper operator++(int) const
        {
            ForwardIteratorWrapper it;
            return (++it);
        }

    public:
        T& operator*() const { return *m_Pointer; }
        T* operator->() const { return m_Pointer; }

        T* Pointer() const { return m_Pointer; }

    public:
        bool operator==(const ForwardIteratorWrapper& it) const
        {
            return (Pointer() == it.Pointer());
        }

    private:
        T* m_Pointer;
    };

    template<typename T>
    class BidirIteratorWrapper
    {
    public:
        using ValueType = T;
        using DifferenceType = std::ptrdiff_t;

    public:
        BidirIteratorWrapper() = default;
        explicit BidirIteratorWrapper(T* ptr)
            : m_Pointer(ptr)
        {
        }

    public:
        BidirIteratorWrapper& operator++() { ++m_Pointer; return *this; }
        BidirIteratorWrapper operator++(int) const
        {
            BidirIteratorWrapper it;
            return (++it);
        }

        BidirIteratorWrapper& operator--() { --m_Pointer; return *this; }
        BidirIteratorWrapper operator--(int) const
        {
            BidirIteratorWrapper it;
            return (--it);
        }

    public:
        T& operator*() const { return *m_Pointer; }
        T* operator->() const { return m_Pointer; }

        T* Pointer() const { return m_Pointer; }

    public:
        bool operator==(const BidirIteratorWrapper& it) const
        {
            return (Pointer() == it.Pointer());
        }

    private:
        T* m_Pointer;
    };

    template<typename T>
    class RandomAccessIteratorWrapper
    {
    public:
        using ValueType = T;
        using DifferenceType = std::ptrdiff_t;

    public:
        RandomAccessIteratorWrapper() = default;
        explicit RandomAccessIteratorWrapper(T* ptr)
            : m_Pointer(ptr)
        {
        }

    public:
        RandomAccessIteratorWrapper& operator++() { ++m_Pointer; return *this; }
        RandomAccessIteratorWrapper operator++(int) const
        {
            RandomAccessIteratorWrapper it;
            return (++it);
        }

        RandomAccessIteratorWrapper& operator--() { --m_Pointer; return *this; }
        RandomAccessIteratorWrapper operator--(int) const
        {
            RandomAccessIteratorWrapper it;
            return (--it);
        }

        RandomAccessIteratorWrapper& operator+=(DifferenceType n)
        {
            m_Pointer += n;
            return *this;
        }

        RandomAccessIteratorWrapper operator+(DifferenceType n) const
        {
            RandomAccessIteratorWrapper it;
            return (it += n);
        }

        RandomAccessIteratorWrapper& operator-=(DifferenceType n)
        {
            m_Pointer -= n;
            return *this;
        }

        RandomAccessIteratorWrapper operator-(DifferenceType n) const
        {
            RandomAccessIteratorWrapper it;
            return (it -= n);
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
        bool operator==(const RandomAccessIteratorWrapper& it) const
        {
            return (Pointer() == it.Pointer());
        }

        bool operator>(const RandomAccessIteratorWrapper& it) const
        {
            return (Pointer() > it.Pointer());
        }

        bool operator<(const RandomAccessIteratorWrapper& it) const
        {
            return (Pointer() < it.Pointer());
        }

        bool operator>=(const RandomAccessIteratorWrapper& it) const
        {
            return (Pointer() >= it.Pointer());
        }

        bool operator<=(const RandomAccessIteratorWrapper& it) const
        {
            return (Pointer() <= it.Pointer());
        }

    private:
        T* m_Pointer;
    };

    template<typename T>
    RandomAccessIteratorWrapper<T> operator+(typename RandomAccessIteratorWrapper<T>::DifferenceType n,
                                             const RandomAccessIteratorWrapper<T>& it)
    {
        RandomAccessIteratorWrapper<T> cp = it;
        return (cp += n);
    }
}
