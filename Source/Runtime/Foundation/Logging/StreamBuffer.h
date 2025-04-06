#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Diagnostics/OutOfRangeException.h"

namespace Kitsune
{
    template<typename T>
    class StreamBuffer
    {
    public:
        inline StreamBuffer()
            : m_Begin(nullptr), m_Pointer(nullptr), m_End(nullptr)
        {
        }

        inline StreamBuffer(T* begin, T* end)
            : m_Begin(begin), m_Pointer(begin), m_End(end)
        {
        }

    public:
        [[nodiscard]] inline T* GetBegin()   const { return m_Begin; }
        [[nodiscard]] inline T* GetCurrent() const { return m_Pointer; }
        [[nodiscard]] inline T* GetEnd()     const { return m_End; }

        [[nodiscard]] inline Ptrdiff GetSize() const { return GetEnd() - GetBegin(); }

        [[nodiscard]]
        inline Ptrdiff GetWrittenCount() const
        {
            return (GetCurrent() - GetBegin());
        }

        [[nodiscard]]
        inline Ptrdiff GetRemainingCapacity() const
        {
            return (GetEnd() - GetCurrent());
        }

    public:
        inline void BumpPointer(Ptrdiff n) { m_Pointer += n; }
        inline void SetPointer(T* ptr)     { m_Pointer = ptr; }

        [[nodiscard]]
        inline T& Front() const
        {
            if (m_Begin == nullptr)
                throw OutOfRangeException();

            return *m_Begin;
        }

        [[nodiscard]]
        inline T& Back() const
        {
            if ((m_Begin == m_Pointer) || (m_Begin == nullptr))
                throw OutOfRangeException();

            return *(m_Pointer - 1);
        }

    private:
        T* m_Begin;
        T* m_Pointer;
        T* m_End;
    };
}
