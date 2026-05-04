#pragma once

#include "GraphicsCore/Texture.h"
#include "GraphicsCore/ShaderModule.h"

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // Specifies when the GPU reads a new data entry from the buffer.
    enum class VertexInputRate
    {
        PerVertex,      //< The GPU advances to the next entry of the buffer after
                        //  each vertex.
        PerInstance     //< The GPU advances to the next entry of the buffer after
                        //  each instance.
    };

    // Specifies the type of the vertex attribute.
    enum class VertexType
    {
        Int32,          //< A 32-bit signed integer.
        Uint32,         //< A 32-bit unsigned integer.
        Float,          //< A single-precision (32-bit) floating-point value.
        Double,         //< A double-precision (64-bit) floating-point value.

        Vec2Int32,      //< A 2-component vector of 32-bit signed integers.
        Vec3Int32,      //< A 3-component vector of 32-bit signed integers.
        Vec4Int32,      //< A 4-component vector of 32-bit signed integers.

        Vec2Uint32,     //< A 2-component vector of 32-bit unsigned integers.
        Vec3Uint32,     //< A 3-component vector of 32-bit unsigned integers.
        Vec4Uint32,     //< A 4-component vector of 32-bit unsigned integers.

        Vec2Float,      //< A 2-component vector of single-precision floats.
        Vec3Float,      //< A 3-component vector of single-precision floats.
        Vec4Float,      //< A 4-component vector of single-precision floats.

        Vec2Double,     //< A 2-component vector of double-precision floats.
        Vec3Double,     //< A 3-component vector of double-precision floats.
        Vec4Double,     //< A 4-component vector of double-precision floats.
    };

    // Specifies what kind of geometry should be drawn by the render pipeline.
    enum class PrimitiveTopology
    {
        PointList,      //> Draw separate point primitives.
        LineList,       //> Draw separate line primitives.
        LineStrip,      //> Draw connected line primitives, where the last vertex
                        //  of the last line becomes the starting vertex for the next
                        //  line.
        TriangleList,   //> Draw separate triangle primitives.
        TriangleStrip   //> Draw connected triangle primitives, with the triangles
                        //  next to each other share an edge.
    };

    // Specifies which faces are considered the "front face".
    enum class FrontFace
    {
        Clockwise,          //> Triangles whose vertices are in a clockwise order
                            //  is considered a front face.
        CounterClockwise    //> Triangles whose vertices are in a counter-clockwise order
                            //  is considered a front face.
    };

    // Specifies which face is culled.
    enum class CullMode
    {
        None,
        Front,
        Back
    };

    // Specifies how the rasterizer should fill a face.
    enum class PolygonFillMode
    {
        Wireframe,      //< Fills only the edges and leaves the interior be. Requires
                        //  the WireframeRendering device extension to be enabled.
        Solid           //< Fills the entire interior of a face.
    };

    // Describes vertex input bindings.
    struct VertexBindingDescription
    {
        Uint32 Binding;
        Uint32 Stride;

        VertexInputRate InputRate;
    };

    // Describes vertex input attributes.
    struct VertexAttributeDescription
    {
        Uint32 Location;
        Uint32 Binding;

        VertexType Type;
        Uint32 Offset;
    };

    // Describes the vertex input given to the render pipeline.
    struct VertexInputDescription
    {
        Array<VertexBindingDescription> Bindings;
        Array<VertexAttributeDescription> Attributes;
    };

    // Contains configurations for creating a render pipeline.
    struct RenderPipelineSpecifications
    {
        PrimitiveTopology Topology;
        VertexInputDescription VertexInput;

        PolygonFillMode FillMode;
        FrontFace FrontFace;
        CullMode CullMode;

        SharedPtr<ShaderModule> VertexShader;
        SharedPtr<ShaderModule> FragmentShader;

        TextureFormat Format;
    };

    // Contains all the configurations for drawing geometry.
    class RenderPipeline : public NonCopyable
    {
    public:
        virtual ~RenderPipeline() = default;
    };
}
