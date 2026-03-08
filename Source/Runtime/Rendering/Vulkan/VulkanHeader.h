#pragma once

#include "Foundation/Common/Predefined.h"

#if defined(KITSUNE_OS_WINDOWS)
    #define VK_USE_PLATFORM_WIN32_KHR 1
#endif

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include "Foundation/String/Format.h"
#include "Foundation/Diagnostics/SystemException.h"

#define KITSUNE_VK_THROW_IF_FAIL(functionCall, message)                          \
    do                                                                           \
    {                                                                            \
        VkResult result_ = functionCall;                                         \
        if (result_ != VK_SUCCESS)                                               \
            Kitsune::VulkanHandleResultFailure(result_, message, #functionCall); \
    } while (false)

namespace Kitsune
{
    inline void VulkanHandleResultFailure(VkResult result, const StringView message,
                                          const StringView functionCall)
    {
        String formatted = Format(
            "The call {0} failed with a VkResult value of {1}.\nMessage: {2}",
            functionCall, string_VkResult(result), message);

        throw SystemException(formatted.Data());
    }
}
