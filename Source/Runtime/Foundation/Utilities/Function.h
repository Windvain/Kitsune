#pragma once

#include <cstring>
#include <type_traits>

#include "Foundation/Templates/Swap.h"
#include "Foundation/Templates/Exchange.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Utilities/BadCallException.h"

namespace Kitsune
{
    template<typename Func>
    class Function;

    template<typename Return, typename... Args>
    class Function<Return(Args...)>
    {
    private:
        typedef void (*CreateFunction)(void*, void*);
        typedef Return (*InvokeFunction)(void*, Args&&...);
        typedef void (*DestroyFunction)(void*);

    public:
        inline Function() = default;
        inline Function(std::nullptr_t) = delete;

        template<typename Func>
            requires ((std::is_invocable_r_v<Return, Func, Args...>) &&
                      (!std::is_same_v<std::remove_cvref_t<Func>, Function<Return(Args...)>>))
        inline Function(Func&& func)
            : m_Size(sizeof(std::remove_cvref_t<Func>)),
              m_Pointer(Memory::Allocate(m_Size)),

              m_CreateFunction(reinterpret_cast<CreateFunction>(StaticCreateFunction<Func>)),
              m_InvokeFunction(reinterpret_cast<InvokeFunction>(StaticInvokeFunction<Func>)),
              m_DestroyFunction(reinterpret_cast<DestroyFunction>(StaticDestroyFunction<Func>))
        {
            using FuncType = std::remove_cvref_t<Func>;
            Memory::ConstructAt(static_cast<FuncType*>(m_Pointer), Forward<Func>(func));
        }

        inline Function(const Function& func)
            : m_Size(func.m_Size),
              m_CreateFunction(func.m_CreateFunction),
              m_InvokeFunction(func.m_InvokeFunction),
              m_DestroyFunction(func.m_DestroyFunction)
        {
            if (func.m_Size == 0)
                return;

            m_Pointer = Memory::Allocate(m_Size);
            std::memcpy(m_Pointer, func.m_Pointer, m_Size);
        }

        inline Function(Function&& func)
            : m_Size(Exchange(func.m_Size, 0)),
              m_Pointer(Exchange(func.m_Pointer, nullptr)),
              m_CreateFunction(Exchange(func.m_CreateFunction, nullptr)),
              m_InvokeFunction(Exchange(func.m_InvokeFunction, nullptr)),
              m_DestroyFunction(Exchange(func.m_DestroyFunction, nullptr))
        {
        }

        inline ~Function()
        {
            if (m_Pointer != nullptr)
            {
                m_DestroyFunction(m_Pointer);
                Memory::Free(m_Pointer);
            }
        }

    public:
        inline Function& operator=(const Function& func)
        {
            if (m_Pointer != func.m_Pointer)
                Function(func).Swap(*this);

            return *this;
        }

        inline Function& operator=(Function&& func)
        {
            if (m_Pointer != func.m_Pointer)
                Function(Move(func)).Swap(*this);

            return *this;
        }

        inline Function& operator=(std::nullptr_t) = delete;

        template<typename Func>
        inline Function& operator=(Func&& func)
        {
            Function(Forward<Func>(func)).Swap(*this);
            return *this;
        }

    public:
        inline explicit operator bool() const
        {
            return (m_Pointer != nullptr);
        }

        inline Return operator()(Args... args) const
        {
            if (m_Pointer == nullptr)
                throw BadCallException();

            return m_InvokeFunction(m_Pointer, Forward<Args>(args)...);
        }

    public:
        [[nodiscard]]
        inline void* Target() const
        {
            return m_Pointer;
        }

        inline void Swap(Function& func)
        {
            Kitsune::Swap(m_Pointer, func.m_Pointer);
            Kitsune::Swap(m_Size, func.m_Size);

            Kitsune::Swap(m_CreateFunction, func.m_CreateFunction);
            Kitsune::Swap(m_InvokeFunction, func.m_InvokeFunction);
            Kitsune::Swap(m_DestroyFunction, func.m_DestroyFunction);
        }

    private:
        template<typename Func>
        static inline void StaticCreateFunction(Func* dest, Func* source)
        {
            Memory::ConstructAt(dest, Move(*source));
        }

        template<typename Func>
        static inline Return StaticInvokeFunction(Func* func, Args&&... args)
        {
            return (*func)(Forward<Args>(args)...);
        }

        template<typename Func>
        static inline void StaticDestroyFunction(Func* func)
        {
            Memory::DestroyAt(func);
        }

    private:
        Usize m_Size = 0;
        void* m_Pointer = nullptr;

        CreateFunction m_CreateFunction = nullptr;
        InvokeFunction m_InvokeFunction = nullptr;
        DestroyFunction m_DestroyFunction = nullptr;
    };
}
