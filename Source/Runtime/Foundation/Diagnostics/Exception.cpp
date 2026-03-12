#include "Foundation/Diagnostics/Exception.h"

#include "Launch/EngineLoop.h"
#include "Foundation/String/String.h"

namespace Kitsune
{
    namespace Details
    {
        // This shouldn't be put in the header file, because it will expose the
        // BasicString<T> template class.
        class ExceptionData
        {
        public:
            using AllocatorType = GlobalAllocator;

        public:
            inline ExceptionData(const char* name, const char* description)
                : Name(name), Description(description)
            {
            }

        public:
            BasicString<char, AllocatorType> Name;
            BasicString<char, AllocatorType> Description;
        };
    }

    Exception::Exception() noexcept
        : Exception("<unknown>", "")
    {
    }

    Exception::Exception(const char* name, const char* description) noexcept
    {
        using Details::ExceptionData;

        void* pointer = Memory::TryAllocate(sizeof(ExceptionData), alignof(ExceptionData));
        if (pointer == nullptr)
            return;

        m_Data = static_cast<ExceptionData*>(pointer);

        Memory::ConstructAt(static_cast<ExceptionData*>(m_Data), name, description);
        EngineLoop::GetInstance()->CaptureExceptionBacktrace();
    }

    Exception::~Exception() noexcept
    {
        if (m_Data == nullptr)
            return;

        Memory::DestroyAt(m_Data);
        Memory::Free(m_Data, sizeof(Details::ExceptionData));
    }

    const char* Exception::GetName() const noexcept
    {
        if (m_Data == nullptr)
            return "<unknown>";

        return m_Data->Name.Raw();
    }

    const char* Exception::GetDescription() const noexcept
    {
        if (m_Data == nullptr)
            return "";

        return m_Data->Description.Raw();
    }
}
