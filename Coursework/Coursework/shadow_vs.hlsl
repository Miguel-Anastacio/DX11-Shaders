#include "Shadow.hlsli"

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


OutputType main(InputType input)
{
    OutputType output;

      
    float3 cubeNormal[] =
    {
        float3(1.0, 0.0, 0.0),
	    float3(-1.0, 0.0, 0.0),
	    float3(0.0, 1.0, 0.0),
	    float3(0.0, -1.0, 0.0),
	    float3(0.0, 0.0, 1.0),
	    float3(0.0, 0.0, -1.0),
    };
    
    
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Calculate the position of the vertice as viewed by each light source.
    // directional light and spotlight
    for (int i = 0; i < 1; i++)
    {
        output.lightViewPos[i] = mul(input.position, worldMatrix);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
    }

   
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
        calculateLightViewMatrix(cubeNormal[i - 13], pointLightPosition[1].xyz, lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightView);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[1]);
    }
    
    
    output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    output.world_position = mul(input.position, worldMatrix);

    return output;
}