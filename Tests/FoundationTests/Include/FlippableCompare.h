#include <memory>
#include <utility>

namespace Kitsune::Testing
{
    template<typename T>
    class FlippableCompare
    {
    public:
        inline explicit FlippableCompare(bool greaterThan = false)
            : m_Flip(greaterThan)
        {
        }

        inline FlippableCompare(const FlippableCompare&) = default;
        inline FlippableCompare(FlippableCompare&& compare)
            : m_Flip(std::exchange(compare.m_Flip, false))
        {
        }

        inline FlippableCompare& operator=(const FlippableCompare&) = default;
        inline FlippableCompare& operator=(FlippableCompare&& compare)
        {
            m_Flip = std::exchange(compare.m_Flip, false);
            return *this;
        }

    public:
        inline bool operator()(const T& lhs, const T& rhs) const
        {
            return m_Flip ? (lhs > rhs) : (lhs < rhs);
        };

        [[nodiscard]]
        inline bool IsFlipped() const
        {
            return m_Flip;
        }

    private:
        bool m_Flip;
    };

    template<typename T>
    class FlippableCompare<std::shared_ptr<T>>
    {
    public:
        inline explicit FlippableCompare(bool greaterThan = false)
            : m_Flip(greaterThan)
        {
        }

        inline FlippableCompare(const FlippableCompare&) = default;
        inline FlippableCompare(FlippableCompare&& compare)
            : m_Flip(std::exchange(compare.m_Flip, false))
        {
        }

        inline FlippableCompare& operator=(const FlippableCompare&) = default;
        inline FlippableCompare& operator=(FlippableCompare&& compare)
        {
            m_Flip = std::exchange(compare.m_Flip, false);
            return *this;
        }

    public:
        inline bool operator()(const std::shared_ptr<T>& lhs,
                               const std::shared_ptr<T>& rhs)
        {
            return m_Flip ? (*lhs > *rhs) : (*lhs < *rhs);
        };

        [[nodiscard]]
        inline bool IsFlipped() const
        {
            return m_Flip;
        }

    private:
        bool m_Flip;
    };

    template<typename T>
    inline bool operator==(
        const FlippableCompare<T>& comp1, const FlippableCompare<T>& comp2)
    {
        return (comp1.IsFlipped() == comp2.IsFlipped());
    }
}
