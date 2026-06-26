#include <stdexcept>
#include <unordered_map>

#include "Foundation/Memory/Memory.h"

namespace Kitsune::Testing
{
    class TrackingAllocator
    {
    public:
        inline void* Allocate(
            Usize bytes,
            Usize alignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__)
        {
            void* pointer = Memory::Allocate(bytes, alignment);
            m_Allocations.insert({ pointer, bytes });

            return pointer;
        }

        inline void Free(void* pointer, Usize bytes)
        {
            if (!m_Allocations.contains(pointer))
            {
                throw std::invalid_argument(
                    "The given pointer was not allocated by this allocator instance.");
            }

            if (m_Allocations[pointer] != bytes)
            {
                throw std::invalid_argument(
                    "The byte size given for the allocation is incorrect.");
            }

            m_Allocations.erase(pointer);
            Memory::Free(pointer, bytes);
        }

    public:
        inline Usize AllocationCount() const
        {
            return m_Allocations.size();
        }

        inline Usize AllocationSize(void* pointer) const
        {
            return m_Allocations.at(pointer);
        }

        inline Usize AllocationSize() const
        {
            Usize allocations = 0;
            for (auto& [pointer_, bytes] : m_Allocations)
                allocations += bytes;

            return allocations;
        }

    public:
        inline bool operator==(const TrackingAllocator& otherAlloc) const
        {
            return (m_Allocations == otherAlloc.m_Allocations);
        }

    private:
        std::unordered_map<void*, std::size_t> m_Allocations;
    };
}
