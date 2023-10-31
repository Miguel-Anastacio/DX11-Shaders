cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer WaveBufferType : register(b1)
{
    // this float 4 contains time, waveLenght and steepness of the a wave
    float4 waveParameters[2];
    float4 direction[2];
}

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
};

// returns the displacement of the vertex after applied one wave
float3 GerstnerWave(int index, float4 pos, inout float3 tangent, inout float3 binormal)
{
    // unpack wave parameter
    float time = waveParameters[index].x;
    float waveLength = max(waveParameters[index].y, 0.01f);;
    float steepness = waveParameters[index].z;
    float4 dir = normalize(direction[index]);
    
    float3 newPosition = float3(0.0f, 0.0f, 0.0f);
    
    float x = pos.x;
    float z = pos.z;
    float waveNumber = 2 * 3.14 / waveLength;
    float sp = sqrt(9.8f / waveNumber);
    float amplitude = steepness / waveNumber;
    float f = waveNumber * (dot(dir.xz, float2(x, z)) - sp * time);
	
    newPosition.y = amplitude * sin(f);
    newPosition.x = dir.x * amplitude * cos(f);
    newPosition.z = dir.z * amplitude * cos(f);
	
    // tangent to the wave along the plane y = 0 z = 0
    // calculated using the partial derivate
    tangent += float3(-dir.x * dir.x * steepness * sin(f),
                            dir.x * steepness * cos(f),
                            -dir.z * dir.x * steepness * sin(f));
    
    // tangent to the wave along the plane x=0 y = 0
    // calculate using the partial derivative
    binormal += float3(-dir.z * dir.x * steepness * sin(f),
                             dir.z * steepness * cos(f),
                              -dir.z * dir.z * sin(f));
    
    return newPosition;
}

OutputType main(InputType input)
{
    OutputType output;
    float4 position = input.position;
    float3 tangent = float3(1.0, 0.0, 0.0f);
    float3 binormal = float3(0.0, 0.0, 1.0f);
    input.position.xyz += GerstnerWave(0, position, tangent, binormal);
    input.position.xyz += GerstnerWave(1, position, tangent, binormal);
    
    input.normal = normalize(cross(binormal, tangent));
    
    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the position value in a second input value for depth value calculations.
    output.depthPosition = output.position;
	
    return output;
}