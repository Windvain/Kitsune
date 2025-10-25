#pragma once

#include <cstddef>

#include "Launch/IEngineLoop.h"
#include "Application/IApplication.h"

#include "Foundation/Logging/ILogger.h"
#include "Foundation/Memory/SharedPtr.h"

namespace Kitsune
{
    namespace Details
    {
        struct BackupMemoryBlock
        {
        public:
            static constexpr Usize DefaultAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;

        public:
            BackupMemoryBlock* Next;
            Usize Size;

            static_assert(((sizeof(Next) + sizeof(Size)) % DefaultAlignment) == 0,
                          "BackupMemoryBlock isn't aligned correctly.");
        };

        class BackupMemoryPool : public NonCopyable
        {
        public:
            BackupMemoryPool(Usize maxBufferSize);
            ~BackupMemoryPool();

        public:
            void* TryAllocate(Usize bytes);
            void Free(void* ptr);

        public:
            BackupMemoryBlock* FindSuitableBlock(Usize bytes) const;
            BackupMemoryBlock* SplitBlock(BackupMemoryBlock* block, Usize bytes) const;

            bool IsSplittable(BackupMemoryBlock* block, Usize bytes) const;

        private:
            void* m_Buffer;
            Usize m_BufferSize;

            BackupMemoryBlock* m_FreeList;
        };
    }

    class DefaultEngineLoop : public IEngineLoop
    {
    public:
        DefaultEngineLoop();
        ~DefaultEngineLoop();

    public:
        bool Initialize(int argc, char** argv) override;
        int Run() override;

        void Shutdown() override;
        void Notify(EngineLoopNotification notification) override;

    public:
        void Exit(int exitCode);
        [[noreturn]] void ForceExit(int exitCode);

    public:
        [[nodiscard]]
        const CommandLineArguments& GetCommandLineArguments() const
        {
            return m_CommandLineArguments;
        }

        [[nodiscard]]
        IApplication* GetApplication() const { return m_Application; }

        [[nodiscard]]
        auto& GetBackupMemoryPool() { return m_BackupMemoryPool; }

        [[nodiscard]] Array<SharedPtr<ILogger>>&       GetLoggers()       { return m_Loggers; }
        [[nodiscard]] const Array<SharedPtr<ILogger>>& GetLoggers() const { return m_Loggers; }

    public:
        [[nodiscard]]
        String GetEngineVersion() const;

    public:
        [[nodiscard]]
        inline static DefaultEngineLoop* GetInstance()
        {
            return s_Instance;
        }

    private:
        void PlatformUpdate();

    private:
        static Usize CalculateBackupMemoryPoolSize();

    private:
        static DefaultEngineLoop* s_Instance;

    private:
        IApplication* m_Application;
        CommandLineArguments m_CommandLineArguments;

        Array<SharedPtr<ILogger>> m_Loggers;
        Details::BackupMemoryPool m_BackupMemoryPool;

        bool m_ExitRequested = false;
        int m_ExitCode = 0;
    };
}
