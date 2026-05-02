#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // Specifies the pixel format of a texture.
    enum class TextureFormat
    {
        Unknown,        //< Unknown format.

        // A one-component format, with an 8-bit unsigned float per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        R8Unorm,

        // A two-component format, with an 8-bit unsigned float per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        // An 8-bit Green component in byte 2.
        R8G8Unorm,

        // A four-component format, with an 8-bit unsigned float per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Blue component in byte 3.
        // An 8-bit Alpha component in byte 4.
        R8G8B8A8Unorm,

        // A three-component format, with an 8-bit unsigned float per channel.
        // Consists of:
        // An 8-bit Blue component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Red component in byte 3.
        B8G8R8Unorm,

        // A four-component format, with an 8-bit unsigned float per channel.
        // Consists of:
        // An 8-bit Blue component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Red component in byte 3.
        // An 8-bit Alpha component in byte 4.
        B8G8R8A8Unorm,

        // A one-component format, with an 8-bit signed float per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        R8Snorm,

        // A two-component format, with an 8-bit signed float per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        // An 8-bit Green component in byte 2.
        R8G8Snorm,

        // A four-component format, with an 8-bit signed float per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Blue component in byte 3.
        // An 8-bit Alpha component in byte 4.
        R8G8B8A8Snorm,

        // A three-component format, with an 8-bit signed float per channel.
        // Consists of:
        // An 8-bit Blue component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Red component in byte 3.
        B8G8R8Snorm,

        // A four-component format, with an 8-bit signed float per channel.
        // Consists of:
        // An 8-bit Blue component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Red component in byte 3.
        // An 8-bit Alpha component in byte 4.
        B8G8R8A8Snorm,

        // A one-component format, with an 8-bit unsigned integer per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        R8Uint,

        // A two-component format, with an 8-bit unsigned integer per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        // An 8-bit Green component in byte 2.
        R8G8Uint,

        // A four-component format, with an 8-bit unsigned integer per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Blue component in byte 3.
        // An 8-bit Alpha component in byte 4.
        R8G8B8A8Uint,

        // A three-component format, with an 8-bit unsigned integer per channel.
        // Consists of:
        // An 8-bit Blue component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Red component in byte 3.
        B8G8R8Uint,

        // A four-component format, with an 8-bit unsigned integer per channel.
        // Consists of:
        // An 8-bit Blue component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Red component in byte 3.
        // An 8-bit Alpha component in byte 4.
        B8G8R8A8Uint,

        // A one-component format, with an 8-bit signed integer per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        R8Sint,

        // A two-component format, with an 8-bit signed integer per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        // An 8-bit Green component in byte 2.
        R8G8Sint,

        // A four-component format, with an 8-bit signed integer per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Blue component in byte 3.
        // An 8-bit Alpha component in byte 4.
        R8G8B8A8Sint,

        // A three-component format, with an 8-bit signed integer per channel.
        // Consists of:
        // An 8-bit Blue component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Red component in byte 3.
        B8G8R8Sint,

        // A four-component format, with an 8-bit signed integer per channel.
        // Consists of:
        // An 8-bit Blue component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Red component in byte 3.
        // An 8-bit Alpha component in byte 4.
        B8G8R8A8Sint,

        // A one-component format in the sRGB encoding, with an 8-bit unsigned float
        // per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        R8Srgb,

        // A two-component format in the sRGB encoding, with an 8-bit unsigned float
        // per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        // An 8-bit Green component in byte 2.
        R8G8Srgb,

        // A four-component format in the sRGB encoding, with an 8-bit unsigned float
        // per channel.
        // Consists of:
        // An 8-bit Red component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Blue component in byte 3.
        // An 8-bit Alpha component in byte 4.
        R8G8B8A8Srgb,

        // A three-component format in the sRGB encoding, with an 8-bit unsigned float
        // per channel.
        // Consists of:
        // An 8-bit Blue component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Red component in byte 3.
        B8G8R8Srgb,

        // A four-component format in the sRGB encoding, with an 8-bit unsigned float
        // per channel.
        // Consists of:
        // An 8-bit Blue component in byte 1.
        // An 8-bit Green component in byte 2.
        // An 8-bit Red component in byte 3.
        // An 8-bit Alpha component in byte 4.
        B8G8R8A8Srgb
    };

    // Describes the intended usage and layout for a texture.
    enum class TextureLayout
    {
        Undefined,          //< The texture's layout is undefined.
        RenderTarget,       //< The texture's layout is optimized for rendering.
        Presentation        //< The texture's layout is optimized for presentation to
                            //  the screen.
    };

    // Specifies the type of texture which is stored on the GPU.
    enum class TextureType
    {
        Texture1D,          //< The texture is a one-dimensional texture.
        Texture2D,          //< The texture is a two-dimensional texture.
        Texture3D           //< The texture is a three-dimensional texture.
    };

    // Specifies how the texture is viewed/accessed.
    enum class TextureViewType
    {
        Texture1D,          //< The texture is interpreted as a 1-dimensional texture.
        Texture1DArray,     //< The texture is interpreted as an array of 1-dimensional
                            //  textures.
        Texture2D,          //< The texture is interpreted as a 2-dimensional texture.
        Texture2DArray,     //< The texture is interpreted as an array of 2-dimensional
                            //  textures.
        Texture3D           //< The texture is interpreted as a 3-dimensional texture.
    };

    // Determines how a texture's component is populated.
    enum class TextureComponentSwizzle
    {
        Zero,               //< Component is set to 0.
        One,                //< Component is set to 1 (for integer formats) or 1.0
                            //  (for floating-point formats).
        Red,                //< Component is set to the Red component.
        Green,              //< Component is set to the Green component.
        Blue,               //< Component is set to the Blue component.
        Alpha               //< Component is set to the Alpha component.
    };

    // Determines how a texture's components are populated.
    struct TextureViewComponentMapping
    {
        TextureComponentSwizzle Red;
        TextureComponentSwizzle Green;
        TextureComponentSwizzle Blue;
        TextureComponentSwizzle Alpha;
    };

    // Contains the specifications of a texture view.
    struct TextureViewSpecifications
    {
        TextureViewType Type;
        TextureFormat Format;
        TextureViewComponentMapping Mapping;
    };

    // A resource representing image data.
    class Texture : public NonCopyable
    {
    public:
        virtual ~Texture() = default;

    public:
        [[nodiscard]]
        virtual Vector2<Uint32> GetSize() const = 0;
    };

    // Provides an interpretation of a texture resource.
    class TextureView : public NonCopyable
    {
    public:
        virtual ~TextureView() = default;

    public:
        [[nodiscard]]
        virtual Texture* GetTexture() const = 0;
    };
}
