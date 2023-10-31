#include "Shadow.hlsli"
#include "LightBuffer.hlsli"
// 2 textures - one is the displacement map the other is the texture applied to the displacement map
Texture2D shaderTexture [2] : register(t0);
// 1 map for the directional light 6 for the point light;
Texture2D depthMapTexture[19] : register(t2);

SamplerState diffuseSampler : register(s0);
SamplerState shadowSampler : register(s1);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 vertexPosition : TEXCOORD1;
    float4 lightViewPos[19] : TEXCOORD2;

};

float GetHeight(float2 uv)
{
	// sample texture to get offset
    float4 offset = shaderTexture[0].SampleLevel(diffuseSampler, uv, 0).r;
	// calculate height
    return offset * 40;
}

float3 CalcNormal(float2 uv)
{
    float tw = 256.0f;
    float val;
    shaderTexture[0].GetDimensions(0, tw, tw, val);
    float uvOff = 3.0f / tw;
    float worldStep = 200.0f * uvOff;
    float heightN = GetHeight(float2(uv.x, uv.y + uvOff));
    float heightS = GetHeight(float2(uv.x, uv.y - uvOff));
    float heightW = GetHeight(float2(uv.x - uvOff, uv.y));
    float heightE = GetHeight(float2(uv.x + uvOff, uv.y));
	
    float height = GetHeight(uv);
	
	
    float3 tanE = normalize(float3(1.0f * worldStep, heightE - height, 0.0f));
    float3 tanW = normalize(float3(-1.0f * worldStep, heightW - height, 0.0f));
	
    float3 bitN = normalize(float3(0.0f, heightN - height, 1.0f * worldStep));
    float3 bitS = normalize(float3(0.0f, heightS - height, -1.0f * worldStep));
	
    float3 crosses = (cross(bitN, tanE) + cross(tanE, bitS) + cross(bitS, tanW) + cross(tanW,bitN)) / 4;
	
    return crosses;
	
}

// experiment - do calculation of light View position in pixel shader for the point light
/*
float4 getLightViewPosition(float3 lightPosition, float4 vertexPosition, float3 normal)
{
    
    float4 lightViewPos = vertexPosition;
    matrix lightView;
    calculateLightViewMatrix(normal, lightPosition, lightView);
    lightViewPos = mul(lightViewPos, lightView);
    lightViewPos = mul(lightViewPos, lightProjectionMatrix);
    
    return lightViewPos;
}*/


bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
	// Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}


float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    float shadowMapBias = 0.005f;
    float4 colour = float4(0.f, 0.f, 0.f, 1.f);

	// Sample the height map texture;
    textureColour = shaderTexture[1].Sample(diffuseSampler, input.tex);

    for (int j = 0; j < 1; j++)
    {
        // Calculate the projected texture coordinates.
        float2 pTexCoord = getProjectiveCoords(input.lightViewPos[j]);
	
        // Shadow test. Is or isn't in shadow
        if (hasDepthData(pTexCoord))
        {
        // Has depth map data
            if (attenuation[j].w != 0.0f)
            {
                shadowMapBias = 0.00005f;
            }
            else
            {
                shadowMapBias = 0.005f;
            }
            
            if (!isInShadow(depthMapTexture[j], pTexCoord, input.lightViewPos[j], shadowMapBias))
            {
            // is NOT in shadow, therefore light
                if(attenuation[j].w != 0.0f)
                {
                   colour += createSpotlight(input.vertexPosition.xyz, position[j].xyz, direction[j], CalcNormal(input.tex), diffuse[j], attenuation[j]);
                }
                else
                {             
                   colour += calculateLighting(-direction[j].xyz, CalcNormal(input.tex), diffuse[j]);
                }
            }
        }
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
        //float4 lightViewPosition = getLightViewPosition(position[2].xyz, input.vertexPosition, cubeNormal[i - 2]);
        float2 pTexCoord = getProjectiveCoords(input.lightViewPos[i]);
        shadowMapBias = 0.005f;
        // Shadow test. Is or isn't in shadow
        if (hasDepthData(pTexCoord))
        {
        // Has depth map data
            if (!isInShadow(depthMapTexture[i], pTexCoord, input.lightViewPos[i], shadowMapBias))
            {
            // is NOT in shadow, therefore light
   
                colour += calculatePointLight(input.vertexPosition.xyz, position[1], CalcNormal(input.tex), diffuse[1], attenuation[1]);
            }
        }
    }
    
    for (int i = 7; i < 13; i++)
    {
        float2 pTexCoord = getProjectiveCoords(input.lightViewPos[i]);
        shadowMapBias = 0.005f;
        // Shadow test. Is or isn't in shadow
        if (hasDepthData(pTexCoord))
        {
        // Has depth map data
            if (!isInShadow(depthMapTexture[i], pTexCoord, input.lightViewPos[i], shadowMapBias))
            {
            // is NOT in shadow, therefore light
                colour += calculatePointLight(input.vertexPosition.xyz, position[2], CalcNormal(input.tex), diffuse[2], attenuation[2]);
            }
        }
    }
    
    for (int i = 13; i < 19; i++)
    {
        float2 pTexCoord = getProjectiveCoords(input.lightViewPos[i]);
        shadowMapBias = 0.005f;
        // Shadow test. Is or isn't in shadow
        if (hasDepthData(pTexCoord))
        {
        // Has depth map data
            if (!isInShadow(depthMapTexture[i], pTexCoord, input.lightViewPos[i], shadowMapBias))
            {
            // is NOT in shadow, therefore light
                colour += calculatePointLight(input.vertexPosition.xyz, position[3], CalcNormal(input.tex), diffuse[3], attenuation[3]);
            }
        }
    }
    
    
    float4 lightColour = saturate(colour);
    return saturate(lightColour) * textureColour;
  
}