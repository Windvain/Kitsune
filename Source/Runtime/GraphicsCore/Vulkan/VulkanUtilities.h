#pragma once

#include "Foundation/Common/Predefined.h"

#if defined(KITSUNE_OS_WINDOWS)
    #define VK_USE_PLATFORM_WIN32_KHR 1
#endif

// https://github.com/clangd/clangd/issues/1085
// Maybe it'll get fixed in later Clang updates?
//
// IWYU pragma: begin_export
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
// IWYU pragma: end_export

#include "Foundation/Maths/Rect2.h"
#include "Foundation/Maths/Vector2.h"

#include "Foundation/Diagnostics/SystemException.h"

#if defined(KITSUNE_BUILD_PRODUCTION)
    #define KITSUNE_VK_THROW_IF_FAIL(functionCall, message) \
        if ((functionCall) != VK_SUCCESS)                   \
            throw SystemException(message)
#else
    #define KITSUNE_VK_THROW_IF_FAIL(functionCall, message) \
    do                                                      \
    {                                                       \
        VkResult result_ = functionCall;                    \
        if (result_ < 0)                                    \
        {                                                   \
            Kitsune::Details::VulkanHandleResultFailure(    \
                result_, message, #functionCall);           \
        }                                                   \
    } while (false)
#endif

namespace Kitsune::Details
{
#if !defined(KITSUNE_BUILD_PRODUCTION)
    inline void VulkanHandleResultFailure(VkResult result, StringView message,
                                          StringView functionCall)
    {
        throw SystemException(
            "Call to {0} resulted in a VkResult of {1}. Message: '{2}'",
            functionCall, string_VkResult(result), message);
    }
#endif

    [[nodiscard]]
    inline Vector2<Uint32> ToVector2_(const VkExtent2D& extent)
    {
        return { extent.width, extent.height };
    }

    [[nodiscard]]
    inline VkExtent2D ToVkExtent2D_(const Vector2<Uint32>& vector)
    {
        return { vector.X, vector.Y };
    }

    [[nodiscard]]
    inline Rect2<Uint32> ToRect2_(const VkRect2D& rectangle)
    {
        return {
            {
                static_cast<Uint32>(rectangle.offset.x),
                static_cast<Uint32>(rectangle.offset.y)
            },
            { rectangle.extent.width, rectangle.extent.height },
        };
    }

    [[nodiscard]]
    inline VkRect2D ToVkRect2D_(const Rect2<Uint32>& vector)
    {
        return {
            .offset = {
                static_cast<Int32>(vector.Position.X),
                static_cast<Int32>(vector.Position.Y)
            },
            .extent = { vector.Size.X, vector.Size.Y }
        };
    }
}
