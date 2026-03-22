#pragma once

namespace Kitsune
{
    template<typename T>
    class LessFunctor
    {
    public:
        inline bool operator()(const T& lhs, const T& rhs) const
        {
            return (lhs < rhs);
        }
    };
}
