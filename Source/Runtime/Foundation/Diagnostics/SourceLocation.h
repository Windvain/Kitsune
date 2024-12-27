#pragma once

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
        SourceLocation() = default;

    public:
        static SourceLocation Current(const char* file = KITSUNE_BUILTIN_FILE_(),
                                      const char* func = KITSUNE_BUILTIN_FUNC_(),
                                      Int32 line = KITSUNE_BUILTIN_LINE_())
        {
            return SourceLocation(file, func, line);
        }

    public:
        const char* FileName() const { return m_FileName; }
        const char* FunctionName() const { return m_FunctionName; }
        Int32 Line() const { return m_Line; }

    private:
        SourceLocation(const char* file, const char* func, Int32 line)
            : m_FileName(file), m_FunctionName(func), m_Line(line)
        {
        }

    private:
        const char* m_FileName;
        const char* m_FunctionName;

        Int32 m_Line;
    };
}
