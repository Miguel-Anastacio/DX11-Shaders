Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);


cbuffer BloomBuffer : register (b0)
{
    float cullCutOff;
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 colour : Color;
};


float4 main(InputType input) : SV_TARGET
{
    // Determine the floating point size of a texel for a screen with this specific width.
    float3 skyColour = float3(0.39f, 0.58f, 0.92f);
    float4 colour;
    // Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 textureColour = texture0.Sample(Sampler0, input.tex);
    float avgColour = (textureColour.x + textureColour.y + textureColour.z) / 3;
    // take only the brightest pixels and blur them 
    if (avgColour > cullCutOff)
    {
        // if pixel is the same color as the background then don't add it to the bloom map
        if (distance(textureColour.xyz, skyColour) == 0)
        {
            return float4(0.0f, 0.0f, 0.0f, 1.0f);
        }
        else
        {
            colour = float4(textureColour.xyz, 1.0f);
        }
        return colour;
    }
    else
    {
     
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    
}