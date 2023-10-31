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
    matrix lightViewMatrix[1];
    matrix lightProjectionMatrix[2];
    float4 pointLightPosition[3];
};

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
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 world_position : TEXCOORD1;
    float4 lightViewPos[19] : TEXCOORD2;
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
     
    // u,v 
    // 0,1 is patch[0].position
    // 1,1 is patch[1].position
    // 1,0 is patch[2].position
    // 0,0 is patch[3].position
    
    // interpolate the new vertex position from the position of the patches
    float3 v1 = lerp(patch[3].position, patch[0].position, uvwCoord.y);
    float3 v2 = lerp(patch[2].position, patch[1].position, uvwCoord.y);
    vertexPosition = lerp(v1, v2, uvwCoord.x);

    // calcultate texture uvs
    float2 u1 = lerp(patch[3].texCoord, patch[0].texCoord, uvwCoord.y);
    float2 u2 = lerp(patch[2].texCoord, patch[1].texCoord, uvwCoord.y);
    output.tex = lerp(u1, u2, uvwCoord.x);
    
    // apply displacement map
    vertexPosition.y += GetHeight(output.tex);
    
    // Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(patch[0].normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    // save the world position of the vertex for light calculations;
    output.world_position = output.position;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);


    
    
    // position as viewed by the directional light
    for (int i = 0; i < 1; i++)
    {
        output.lightViewPos[i] = mul(float4(vertexPosition, 1.0f), worldMatrix);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
    }
    
    float3 cubeNormal[] =
    {
        float3(1.0, 0.0, 0.0),
	    float3(-1.0, 0.0, 0.0),
	    float3(0.0, 1.0, 0.0),
	    float3(0.0, -1.0, 0.0),
	    float3(0.0, 0.0, 1.0),
	    float3(0.0, 0.0, -1.0),
    };
    
    // position as viewed by each point light
    for (int i = 1; i < 7; i++)
    {
        output.lightViewPos[i] = mul(float4(vertexPosition, 1.0f), worldMatrix);
        matrix lightView;
        calculateLightViewMatrix(cubeNormal[i - 1], pointLightPosition[0].xyz, lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[1]);
    }
    
    for (int i = 7; i < 13; i++)
    {
        output.lightViewPos[i] = mul(float4(vertexPosition, 1.0f), worldMatrix);
        matrix lightView;
        calculateLightViewMatrix(cubeNormal[i - 7], pointLightPosition[1].xyz, lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[1]);
    }
    
    for (int i = 13; i < 19; i++)
    {
        output.lightViewPos[i] = mul(float4(vertexPosition, 1.0f), worldMatrix);
        matrix lightView;
        calculateLightViewMatrix(cubeNormal[i - 13], pointLightPosition[2].xyz, lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[1]);
    }


    return output;
}

