#pragma once

#include <cstring>

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Macros.h"

#if KITSUNE_HAS_BUILTIN(__builtin_FILE) && KITSUNE_HAS_BUILTIN(__builtin_LINE) && \
    KITSUNE_HAS_BUILTIN(__builtin_FUNCTION)
    #define KITSUNE_BUILTIN_FILE_() __builtin_FILE()
    #define KITSUNE_BUILTIN_LINE_() __builtin_LINE()
    #define KITSUNE_BUILTIN_FUNC_() __builtin_FUNCTION()
#else
    #define KITSUNE_BUILTIN_FILE_() "<unknown>"
    #define KITSUNE_BUILTIN_LINE_() 0
    #define KITSUNE_BUILTIN_FUNC_() "<unknown>"
#endif

namespace Kitsune
{
    class SourceLocation
    {
    public:
        SourceLocation()
            : m_FileName("<unknown>"), m_FunctionName("<unknown>"),
              m_Line(0)
        {
        }

    public:
        static SourceLocation Current(const char* file = KITSUNE_BUILTIN_FILE_(),
                                      const char* func = KITSUNE_BUILTIN_FUNC_(),
                                      Uint32 line = KITSUNE_BUILTIN_LINE_())
        {
            return SourceLocation(file, func, line);
        }

    public:
        [[nodiscard]] const char* FileName()     const { return m_FileName; }
        [[nodiscard]] const char* FunctionName() const { return m_FunctionName; }
        [[nodiscard]] Uint32 Line()              const { return m_Line; }

    private:
        SourceLocation(const char* file, const char* func, Uint32 line)
            : m_FileName(file), m_FunctionName(func), m_Line(line)
        {
        }

    private:
        const char* m_FileName;
        const char* m_FunctionName;

        Uint32 m_Line;
    };

    inline bool operator==(const SourceLocation& loc1, const SourceLocation& loc2)
    {
        return (loc1.Line() == loc2.Line()) &&
               (std::strcmp(loc1.FileName(), loc2.FileName()) == 0) &&
               (std::strcmp(loc1.FunctionName(), loc2.FunctionName()) == 0);
    }
}
