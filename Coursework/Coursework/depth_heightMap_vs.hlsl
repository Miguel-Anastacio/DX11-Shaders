
struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float3 position : POSITION;
    float2 texCoord : CONTROL_POINT_TEXCOORD;
    float3 normal : NORMAL;
};

OutputType main(InputType input)
{
    OutputType output;

	 // Pass the vertex position into the hull shader.
    output.position = input.position;
    output.normal = input.normal;
    // Pass the input color into the hull shader.
    output.texCoord = input.tex;
    
    return output;
}
