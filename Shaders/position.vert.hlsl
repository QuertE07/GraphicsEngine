struct Input
{
    float3 position : TEXCOORD0;
};

struct Output
{
    float4 Position : SV_Position;
};

Output main(Input input)
{
    Output output;
    output.Position = float4(input.position.x - 0.3f, input.position.y - 0.1f, input.position.z + 1.0f, 1.3f);

    return output;
}