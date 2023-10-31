#include "Shadow.hlsli"

// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
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


struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
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

float3 CalcNormal(float2 uv)
{
    float tw = 256.0f;
    float val;
    texture0.GetDimensions(0, tw, tw, val);
    float uvOff = 1.0f / 200.0f;
	// calculate height north
    float heightN = GetHeight(float2(uv.x, uv.y + uvOff));
    float heightS = GetHeight(float2(uv.x, uv.y - uvOff));
    float heightW = GetHeight(float2(uv.x - uvOff, uv.y));
    float heightE = GetHeight(float2(uv.x + uvOff, uv.y));
	
    float height = GetHeight(uv);
	
    float3 tanE = normalize(float3(1.0f, heightE - height, 0.0f));
    float3 tanW = normalize(float3(-1.0f, heightW - height, 0.0f));
	
    float3 bitN = normalize(float3(0.0f, heightN - height, 1.0f));
    float3 bitS = normalize(float3(0.0f, heightS - height, -1.0f));
	
    float3 crosses = (cross(bitN, tanE) - cross(bitN, tanW) - cross(bitS, tanE) + cross(bitS, tanW)) / 4;
	
    return crosses;
	
}

OutputType main(InputType input)
{
    OutputType output;
	
	// sample texture to offset
	// displace the values according to the offset
    input.position.y += GetHeight(input.tex);
	
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // calculate the position of the vertex as viewed by each directional and spotlight light source
    for (int i = 0; i < 1; i++)
    {
        output.lightViewPos[i] = mul(input.position, worldMatrix);
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
    
    for (int i = 1; i < 7; i++)
    {
        output.lightViewPos[i] = mul(input.position, worldMatrix);
        matrix lightView;
        calculateLightViewMatrix(cubeNormal[i - 1], pointLightPosition[0].xyz, lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[1]);
    }
    
    for (int i = 7; i < 13; i++)
    {
        output.lightViewPos[i] = mul(input.position, worldMatrix);
        matrix lightView;
        calculateLightViewMatrix(cubeNormal[i - 7], pointLightPosition[1].xyz, lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[1]);
    }
    
    for (int i = 13; i < 19; i++)
    {
        output.lightViewPos[i] = mul(input.position, worldMatrix);
        matrix lightView;
        calculateLightViewMatrix(cubeNormal[i - 13], pointLightPosition[2].xyz, lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[1]);
    }
    
    
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(CalcNormal(input.tex), (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    
    // calculate world position of the vertex
    output.world_position = mul(input.position, worldMatrix);
    
    return output;
}