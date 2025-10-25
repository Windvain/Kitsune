#include "Launch/DefaultEngineLoop.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Logging/ConsoleLogger.h"

namespace Kitsune
{
    namespace Details
    {
        BackupMemoryPool::BackupMemoryPool(Usize maxBufferSize)
            : m_BufferSize(maxBufferSize)
        {
            KITSUNE_ASSERT(maxBufferSize > sizeof(BackupMemoryBlock),
                           "Tried to create a backup memory pool below the minimum size.");

            m_Buffer = Memory::VirtualAllocate(maxBufferSize, MemoryProtection::ReadWrite);
            m_FreeList = static_cast<BackupMemoryBlock*>(m_Buffer);

            m_FreeList->Next = nullptr;
            m_FreeList->Size = maxBufferSize - sizeof(BackupMemoryBlock);
        }

        BackupMemoryPool::~BackupMemoryPool()
        {
            Memory::VirtualFree(m_Buffer, m_BufferSize);
        }

        void* BackupMemoryPool::TryAllocate(Usize bytes)
        {
            if (bytes == 0)
                return nullptr;

            // Align bytes to default alignment.
            Usize offset = bytes % BackupMemoryBlock::DefaultAlignment;
            bytes += (offset != 0) ? BackupMemoryBlock::DefaultAlignment - offset : 0;

            BackupMemoryBlock* block = m_FreeList;
            BackupMemoryBlock* prevBlock = nullptr;

            for (; block != nullptr; prevBlock = block, block = block->Next)
            {
                if ((block->Size == bytes) || (IsSplittable(block, bytes)))
                    break;
            }

            if (block == nullptr)
                return nullptr;

            // If block size is equal to requested size, no need to split.
            if (block->Size != bytes)
            {
                Uintptr memory = reinterpret_cast<Uintptr>(block) + sizeof(BackupMemoryBlock) + bytes;
                BackupMemoryBlock* nextBlock = reinterpret_cast<BackupMemoryBlock*>(memory);

                nextBlock->Next = block->Next;
                block->Next = nextBlock;

                nextBlock->Size = block->Size - (sizeof(BackupMemoryBlock) + bytes);
                block->Size = bytes;
            }

            if (prevBlock)
                prevBlock->Next = block->Next;
            else
                m_FreeList = block->Next;

            block->Next = nullptr;
            return reinterpret_cast<void*>(reinterpret_cast<Uintptr>(block) + sizeof(BackupMemoryBlock));
        }

        void BackupMemoryPool::Free(void* ptr)
        {
            if (ptr == nullptr)
                return;

            auto* block = reinterpret_cast<BackupMemoryBlock*>(
                              reinterpret_cast<Uintptr>(ptr) - sizeof(BackupMemoryBlock));

            // Just put the block in the free list for now.
            // TODO: Add a way to combine blocks together so we reduce the speed at which memory fragments.
            block->Next = m_FreeList;
            m_FreeList = block;
        }

        bool BackupMemoryPool::IsSplittable(BackupMemoryBlock* block, Usize bytes) const
        {
            if (block->Size < bytes)
                return false;

            return (block->Size - bytes) >= (sizeof(BackupMemoryBlock) + 1);
        }
    }

    DefaultEngineLoop* DefaultEngineLoop::s_Instance = nullptr;

    DefaultEngineLoop::DefaultEngineLoop()
        : m_Application(nullptr), m_BackupMemoryPool(CalculateBackupMemoryPoolSize())
    {
        KITSUNE_ASSERT(s_Instance == nullptr,
                       "More than one instance of the default engine loop has been created.");

        s_Instance = this;
    }

    DefaultEngineLoop::~DefaultEngineLoop()
    {
        s_Instance = nullptr;
    }

    bool DefaultEngineLoop::Initialize(int argc, char** argv)
    {
        m_Loggers.PushBack(MakeShared<ConsoleLogger>());

        KITSUNE_ENGINE_INFO_FORMAT_("Initializing Kitsune Engine {0}. For the source code, visit https://github.com/Windvain/Kitsune",
                                    GetEngineVersion());

        KITSUNE_ASSERT(argc != 0, "Tried to initialize the engine loop with no arguments.");
        KITSUNE_ENGINE_INFO_FORMAT_(
            "Initializing application \"{0}\"",
            argv[0]);

        m_CommandLineArguments = CommandLineArguments(argc, argv);
        m_Application = CreateApplication(m_CommandLineArguments);

        if (m_Application == nullptr)
            return false;

        KITSUNE_ENGINE_INFO_("Successfully initialized the engine loop.");
        return true;
    }

    int DefaultEngineLoop::Run()
    {
        KITSUNE_ASSERT(m_Application != nullptr, "The engine loop has not been initialized.");

        while (!m_ExitRequested)
        {
            PlatformUpdate();
            m_Application->OnUpdate();
        }

        return m_ExitCode;
    }

    void DefaultEngineLoop::Shutdown()
    {
        if (m_Application == nullptr)
            return;

        KITSUNE_ENGINE_INFO_("Shutting down Kitsune Engine.");

        Memory::Delete(m_Application);
        m_Application = nullptr;

        // Shutting the subsystems down has to be done manually, because the destructors of
        // our member variables are called based on its initialization order.
        m_Loggers.Clear();
    }

    void DefaultEngineLoop::Notify(EngineLoopNotification notification)
    {
        KITSUNE_UNUSED(notification);
    }

    String DefaultEngineLoop::GetEngineVersion() const
    {
        return Format("v{0}.{1}.{2}",
                      KITSUNE_VERSION_MAJOR, KITSUNE_VERSION_MINOR,
                      KITSUNE_VERSION_PATCH);
    }

    Usize DefaultEngineLoop::CalculateBackupMemoryPoolSize()
    {
        // TODO: Actually calculate memory pool size based on RAM.
        return 4096;
    }
}
