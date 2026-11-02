#include "lighting.hlsli"

Texture2D<float4> u_texture : register(t0, space2);
SamplerState u_sampler : register(s0, space2);

cbuffer VertexUniforms : register(b0, space3)
{
    float4x4 mvp;
    float4x4 model;
    float3 cameraPosition;
    float padding1;
}

cbuffer LightUniforms : register(b1, space3)
{
    Light lights[MAX_LIGHTS];
    uint numLights;
    float3 ambient;
}

cbuffer MaterialUniforms : register(b2, space3)
{
    Material material;
}

struct Input
{
    float2 texcoord : TEXCOORD0;
    float3 diffuse : TEXCOORD1;
    float3 specular : TEXCOORD2;
};

struct Output
{
	float4 color : SV_Target0;
};

Output main(Input input)
{
	Output output;
    
    float4 albedo = u_texture.Sample(u_sampler, input.texcoord) * material.color;
    output.color = float4(albedo.rgb * input.diffuse + input.specular, albedo.a);

	return output;
}
