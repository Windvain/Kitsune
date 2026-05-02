#pragma once

#include "Foundation/Common/Macros.h"

KITSUNE_PUSH_COMPILER_WARNINGS()
KITSUNE_IGNORE_MSVC_WARNING(4201)   // Nonstandard extension used: Nameless struct/union.

KITSUNE_IGNORE_CLANG_WARNING(-Wgnu-anonymous-struct)
KITSUNE_IGNORE_CLANG_WARNING(-Wnested-anon-types)

namespace Kitsune
{
    template<typename T>
    class Color4
    {
    public:
        inline Color4()
            : R(), G(), B(), A()
        {
        }

        inline Color4(
            const T& paramR, const T& paramG, const T& paramB,
            const T& paramA)
            : R(paramR), G(paramG), B(paramB), A(paramA)
        {
        }

    public:
        union
        {
            struct { T R, G, B, A; };
            T Data[4];
        };
    };
}

KITSUNE_POP_COMPILER_WARNINGS()
