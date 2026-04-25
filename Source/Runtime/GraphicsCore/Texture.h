#pragma once

#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // The format of a texture.
    enum class TextureFormat
    {
        Unspecified,
        R8UnsignedNormalized,
        Rg8UnsignedNormalized,
        Rgb8UnsignedNormalized,
        Rgba8UnsignedNormalized,
        Bgr8UnsignedNormalized,
        Bgra8UnsignedNormalized,
        R8SignedNormalized,
        Rg8SignedNormalized,
        Rgb8SignedNormalized,
        Rgba8SignedNormalized,
        Bgr8SignedNormalized,
        Bgra8SignedNormalized,
        R8UnsignedInteger,
        Rg8UnsignedInteger,
        Rgb8UnsignedInteger,
        Rgba8UnsignedInteger,
        Bgr8UnsignedInteger,
        Bgra8UnsignedInteger,
        R8SignedInteger,
        Rg8SignedInteger,
        Rgb8SignedInteger,
        Rgba8SignedInteger,
        Bgr8SignedInteger,
        Bgra8SignedInteger,
        R8Srgb,
        Rg8Srgb,
        Rgb8Srgb,
        Rgba8Srgb,
        Bgr8Srgb,
        Bgra8Srgb
    };

    // The usage of a texture.
    enum class TextureUsage
    {
        Undefined,
        RenderAttachment,
        Presentation
    };

    // The type of a texture view.
    enum class TextureViewDimension
    {
        Texture1D,
        Texture1DArray,
        Texture2D,
        Texture2DArray,
        Texture3D
    };

    // Contains information used for creating a texture view.
    struct TextureViewSpecifications
    {
        TextureViewDimension Dimension;
        TextureFormat Format;
    };

    // A region in memory whose layout is interpreted as an image.
    class Texture : public NonCopyable
    {
    public:
        virtual ~Texture() = default;
    };

    // Provides a view to a texture.
    class TextureView : public NonCopyable
    {
    public:
        virtual ~TextureView() = default;
    };
}
