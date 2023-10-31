// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers

Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 colour : Color;
};


float4 main(InputType input) : SV_TARGET
{
    OutputType output;
    output.position = input.position;
    output.colour = float4(input.uv.x, input.uv.x, input.uv.x, 1.0f);
    
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    float4 textureColor = texture0.Sample(Sampler0, input.uv);

    return textureColor;
    
    //return output.colour;
}