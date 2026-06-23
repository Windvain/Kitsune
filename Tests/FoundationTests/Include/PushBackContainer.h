#include <vector>

namespace Kitsune::Testing
{
    template<typename T>
    class PushBackContainer
    {
    public:
        using ValueType = T;

        using Iterator = T*;
        using ConstIterator = const T*;

    public:
        PushBackContainer() = default;
        PushBackContainer(const PushBackContainer&) = default;

        PushBackContainer& operator=(const PushBackContainer&) = default;

    public:
        [[nodiscard]]
        inline Iterator GetBegin()
        {
            return m_Vector.data();
        }

        [[nodiscard]]
        inline ConstIterator GetBegin() const
        {
            return m_Vector.data();
        }

        [[nodiscard]]
        inline Iterator GetEnd()
        {
            return m_Vector.data() + m_Vector.size();
        }

        [[nodiscard]]
        inline ConstIterator GetEnd() const
        {
            return m_Vector.data() + m_Vector.size();
        }

    public:
        inline void PushBack(int element)
        {
            m_Vector.push_back(element);
        }

    public:
        [[nodiscard]]
        inline std::size_t Size() const
        {
            return m_Vector.size();
        }

        [[nodiscard]]
        inline bool IsEmpty() const
        {
            return m_Vector.empty();
        }

        [[nodiscard]]
        inline std::vector<T>& GetVector()
        {
            return m_Vector;
        }

        [[nodiscard]]
        inline const std::vector<T>& GetVector() const
        {
            return m_Vector;
        }

        inline void Swap(PushBackContainer& container)
        {
            m_Vector.swap(container.m_Vector);
        }

    public:
        inline bool operator==(const PushBackContainer& container) const
        {
            return m_Vector == container.m_Vector;
        }

    private:
        std::vector<T> m_Vector;
    };
}
