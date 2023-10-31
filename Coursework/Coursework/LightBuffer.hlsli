cbuffer LightBufferType : register(b0)
{
    float4 ambient[4];
    float4 diffuse[4];
    float4 direction[4];
    float4 position[4];
    // spotlight
    // the w value is the cone of the spotlight
    float4 attenuation[4];
};
