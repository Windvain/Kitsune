#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Macros.h"

#include "Foundation/String/StringView.h"

#if KITSUNE_HAS_BUILTIN(__builtin_FILE) || defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_INTERNAL_BUILTIN_FILE() __builtin_FILE()
#else
    #define KITSUNE_INTERNAL_BUILTIN_FILE() "<unknown>"
#endif

#if KITSUNE_HAS_BUILTIN(__builtin_LINE) || defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_INTERNAL_BUILTIN_LINE() __builtin_LINE()
#else
    #define KITSUNE_INTERNAL_BUILTIN_LINE() 0
#endif

// MSVC defines both __builtin_FUNCSIG() and __builtin_FUNCTION().
// __builtin_FUNCSIG is preferred here because it returns the entire function
// signature, not just its name.
#if defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_INTERNAL_BUILTIN_FUNC() __builtin_FUNCSIG()
#elif KITSUNE_HAS_BUILTIN(__builtin_FUNCTION)
    #define KITSUNE_INTERNAL_BUILTIN_FUNC() __builtin_FUNCTION()
#else
    #define KITSUNE_INTERNAL_BUILTIN_FUNC() "<unknown>"
#endif

namespace Kitsune
{
    // Contains information about the line, file, and function in which
    // a function call happens.
    class SourceLocation
    {
    public:
        inline SourceLocation()
            : m_FileName("<unknown>"),
              m_FunctionName("<unknown>"),
              m_Line(0)
        {
        }

    public:
        // Obtains the source information at the current location. Do not call
        // this function with any arguments.
        inline static SourceLocation Current(
            const char* file = KITSUNE_INTERNAL_BUILTIN_FILE(),
            const char* func = KITSUNE_INTERNAL_BUILTIN_FUNC(),
            Uint32 line = KITSUNE_INTERNAL_BUILTIN_LINE())
        {
            return SourceLocation(file, func, line);
        }

    public:
        [[nodiscard]]
        inline StringView FileName() const
        {
            return m_FileName;
        }

        [[nodiscard]]
        inline StringView FunctionName() const
        {
            return m_FunctionName;
        }

        [[nodiscard]]
        inline Uint32 Line() const
        {
            return m_Line;
        }

    public:
        inline bool operator==(const SourceLocation& location) const
        {
            return (m_Line == location.m_Line) &&
                   (m_FileName == location.m_FileName) &&
                   (m_FunctionName == location.m_FunctionName);
        }

    private:
        inline SourceLocation(const char* fileName, const char* functionName,
                              Uint32 line)
            : m_FileName(fileName),
              m_FunctionName(functionName),
              m_Line(line)
        {
        }

    private:
        StringView m_FileName;
        StringView m_FunctionName;

        Uint32 m_Line;
    };
}

#undef KITSUNE_INTERNAL_BUILTIN_FILE
#undef KITSUNE_INTERNAL_BUILTIN_LINE
#undef KITSUNE_INTERNAL_BUILTIN_FUNC
