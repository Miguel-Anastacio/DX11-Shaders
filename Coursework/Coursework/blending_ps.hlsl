Texture2D sceneTexture : register(t0);
Texture2D bloomTexture : register(t1);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);

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
    float4 sceneColour = sceneTexture.Sample(sampler0, input.tex);
    float4 bloomColour = bloomTexture.Sample(sampler1, input.tex);
    
    float4 textureColor = lerp(sceneColour, bloomColour, 0.5);
    //return float4(1.0f, 0.0f, 0.0f, 1.0f);
    return sceneColour + bloomColour;
    return textureColor;
    
}