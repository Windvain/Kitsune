#include "Foundation/Diagnostics/IException.h"

#include <cstring>
#include "Launch/DefaultEngineLoop.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/BackupMemoryAllocator.h"

#include "Foundation/Threading/Mutex.h"
#include "Foundation/Diagnostics/StackTrace.h"

namespace Kitsune
{
    struct ExceptionData
    {
        BasicString<char, BackupMemoryAllocator> Name;
        BasicString<char, BackupMemoryAllocator> Description;

        BasicStackTrace<BackupMemoryAllocator> StackTrace;
    };

    static ExceptionData* g_ExceptionData;
    static bool g_WritingToExceptionData = false;

    extern ScopedPtr<Mutex>* g_ExceptionMutex;
    extern BasicStackTrace<BackupMemoryAllocator>* g_ExceptionStackTrace;

    IException::IException() noexcept
        : IException("<unknown>", "")
    {
    }

    IException::IException(const char* name, const char* desc) noexcept
    {
        if ((DefaultEngineLoop::GetInstance() == nullptr) || (g_ExceptionMutex == nullptr))
            return;

        if (g_WritingToExceptionData || ((*g_ExceptionMutex)->TryAcquire() == 0))
            return;

        auto& memoryPool = DefaultEngineLoop::GetInstance()->GetBackupMemoryPool();
        ExceptionData* data = static_cast<ExceptionData*>(memoryPool.TryAllocate(sizeof(ExceptionData)));

        if (data == nullptr)
            return;

        // Avoid throws inside the try/catch block causing an infinite loop.
        g_WritingToExceptionData = true;

        try
        {
            Memory::ConstructAt(&data->Name, name);
            Memory::ConstructAt(&data->Description, desc);

            Memory::ConstructAt(&data->StackTrace, BasicStackTrace<BackupMemoryAllocator>::Current());
        }
        catch (...)
        {
            memoryPool.Free(data);
            data = nullptr;
        }

        g_WritingToExceptionData = false;
        g_ExceptionData = data;

        if (data != nullptr)
            g_ExceptionStackTrace = &data->StackTrace;
    }

    IException::~IException() noexcept
    {
        if ((DefaultEngineLoop::GetInstance() == nullptr) || g_WritingToExceptionData)
            return;

        auto& memoryPool = DefaultEngineLoop::GetInstance()->GetBackupMemoryPool();
        if (g_ExceptionData != nullptr)
        {
            Memory::DestroyAt(g_ExceptionData);
            memoryPool.Free(g_ExceptionData);

            g_ExceptionData = nullptr;
            g_ExceptionStackTrace = nullptr;
        }

        (*g_ExceptionMutex)->Release();
    }

    const char* IException::GetName() const noexcept
    {
        if (g_ExceptionData == nullptr)
            return "<unknown>";

        return g_ExceptionData->Name.Raw();
    }

    const char* IException::GetDescription() const noexcept
    {
        if (g_ExceptionData == nullptr)
            return "None";

        return g_ExceptionData->Description.Raw();
    }
}
