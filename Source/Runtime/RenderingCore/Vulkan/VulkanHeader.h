#pragma once

#include "Foundation/Common/Predefined.h"

#if defined(KITSUNE_OS_WINDOWS)
    #define VK_USE_PLATFORM_WIN32_KHR 1
#endif

#include <vulkan/vulkan.h>                  // IWYU pragma: export
#include <vulkan/vk_enum_string_helper.h>   // IWYU pragma: export

#include "Foundation/Diagnostics/SystemException.h"

#if defined(KITSUNE_BUILD_PRODUCTION)
    #define KITSUNE_VK_THROW_IF_FAIL(functionCall, message) throw SystemException(message)

#else
    #include "Foundation/String/Format.h"

    #define KITSUNE_VK_THROW_IF_FAIL(functionCall, message)                               \
    do                                                                                    \
    {                                                                                     \
        VkResult result_ = functionCall;                                                  \
        if (result_ != VK_SUCCESS)                                                        \
            Kitsune::Details::VulkanHandleResultFailure(result_, message, #functionCall); \
    } while (false)
#endif

namespace Kitsune::Details
{
#if !defined(KITSUNE_BUILD_PRODUCTION)
    inline void VulkanHandleResultFailure(VkResult result, const StringView message,
                                          const StringView functionCall)
    {
        String formatted = Format(
            "The call {0} failed with a VkResult value of {1}.\nMessage: {2}",
            functionCall, string_VkResult(result), message);

        throw SystemException(formatted.Data());
    }
#endif
}
