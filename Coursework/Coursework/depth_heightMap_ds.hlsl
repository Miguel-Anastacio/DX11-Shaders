// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
#include "Shadow.hlsli"
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}
struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
    float4 position : POSITION;
    float2 texCoord : CONTROL_POINT_TEXCOORD;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
};

float GetHeight(float2 uv)
{
	// sample texture to get offset
    float4 offset = texture0.SampleLevel(sampler0, uv, 0).r;
	// calculate height
    return offset * 40;
}


[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition = float3(0.0f, 0.0f, 0.0f);
    OutputType output;
 
    
    // interpolate the new vertex position from the position of the patches
    float3 v1 = lerp(patch[3].position, patch[0].position, uvwCoord.y);
    float3 v2 = lerp(patch[2].position, patch[1].position, uvwCoord.y);
    vertexPosition = lerp(v1, v2, uvwCoord.x);

    // calcultate texture uvs
    float2 u1 = lerp(patch[3].texCoord, patch[0].texCoord, uvwCoord.y);
    float2 u2 = lerp(patch[2].texCoord, patch[1].texCoord, uvwCoord.y);
    float2 texCoord = lerp(u1, u2, uvwCoord.x);
    
    // apply displacement map
    vertexPosition.y += GetHeight(texCoord);
    
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.depthPosition = output.position;
    
    return output;

}
