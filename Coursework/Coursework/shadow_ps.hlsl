#include "Shadow.hlsli"
#include "LightBuffer.hlsli"
Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture[19] : register(t1);

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
    float shadowMapBias = 0.005f;
    float4 colour = float4(0.f, 0.f, 0.f, 1.f);
    float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);

	// Calculate the projected texture coordinates.
    for (int j = 0; j < 1; j++)
    {
        float2 pTexCoord = getProjectiveCoords(input.lightViewPos[j]);
        // Shadow test. Is or isn't in shadow
        if (hasDepthData(pTexCoord))
        {
            if (attenuation[j].w != 0.0f)
            {
                shadowMapBias = 0.0005f;
            }
            else
            {
                shadowMapBias = 0.005f;
            }
            
        // Has depth map data
            if (!isInShadow(depthMapTexture[j], pTexCoord, input.lightViewPos[j], shadowMapBias))
            {
            /// is NOT in shadow, therefore light
                if (attenuation[j].w != 0.0f)
                {
                    colour += createSpotlight(input.vertexPosition.xyz, position[j].xyz, direction[j], input.normal, diffuse[j], attenuation[j]);
                }
                else
                {
                    //colour += calculateLighting(pointLightDir, input.normal, diffuse[j]);
                    colour += calculateLighting(-direction[j].xyz, input.normal, diffuse[j]);
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
        //float4 lightViewPosition = getLightViewPosition(position[2].xyz, input.vertexPosition, cubeNormal[i - 1]);
        float2 pTexCoord = getProjectiveCoords(input.lightViewPos[i]);
        shadowMapBias = 0.005f;
        // Shadow test. Is or isn't in shadow
        if (hasDepthData(pTexCoord))
        {
        // Has depth map data
            if (!isInShadow(depthMapTexture[i], pTexCoord, input.lightViewPos[i], shadowMapBias))
            {
            // is NOT in shadow, therefore light
   
                colour += calculatePointLight(input.vertexPosition.xyz, position[1], input.normal, diffuse[1], attenuation[1]);
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
   
                colour += calculatePointLight(input.vertexPosition.xyz, position[2], input.normal, diffuse[2], attenuation[2]);
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
   
                colour += calculatePointLight(input.vertexPosition.xyz, position[3], input.normal, diffuse[3], attenuation[3]);
            }
        }
    }
    
    float4 lightColour;
    //lightColour.xyz = input.normal;
    //return lightColour;
    lightColour = saturate(colour );
    return saturate(lightColour) * textureColour;
}