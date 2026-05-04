struct VertexInput
{
    float2 Position;
    float4 Color;
};

struct VertexOutput
{
    float4 Color;
    float4 Position : SV_Position;
};

[shader("vertex")]
VertexOutput VertexMain(VertexInput input)
{
    VertexOutput output;
    output.Position = float4(input.Position, 0.0, 1.0);
    output.Color = input.Color;
    return output;
}

[shader("fragment")]
float4 FragmentMain(VertexOutput input) : SV_Target
{
    return input.Color;
}
