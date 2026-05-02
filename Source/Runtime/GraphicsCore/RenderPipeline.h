#pragma once

#include "GraphicsCore/Texture.h"
#include "GraphicsCore/ShaderModule.h"

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
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

    // Contains configurations for creating a render pipeline.
    struct RenderPipelineSpecifications
    {
        PrimitiveTopology Topology;

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
