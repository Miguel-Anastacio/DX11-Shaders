// Tessellation Hull Shader
// Prepares control points for tessellation
cbuffer TesselationBuffer : register(b0)
{
    float edgeTesselationFactor;
    float interiorTesselationFactor;
    bool dynamicTess;
    float padding;
}


cbuffer CameraBuffer : register(b1)
{
    matrix worldMatrix;
    float4 cameraPosition;
}

struct InputType
{
    float4 position : POSITION;
    float2 texCoord : CONTROL_POINT_TEXCOORD;
    float3 normal : NORMAL;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside [2] : SV_InsideTessFactor;
};

struct OutputType
{
    float4 position : POSITION;
    float2 texCoord : CONTROL_POINT_TEXCOORD;
    float3 normal : NORMAL;
};

float3 computePatchMidPoint(float3 controlPoint0, float3 controlPoint1, float3 controlPoint2, float3 controlPoint3)
{
    return (controlPoint0 + controlPoint1 + controlPoint2 + controlPoint3) / 4.0f;
}


float calculadeScaledDistance(float3 a, float3 b)
{
    float d = distance(a, b);
    float scaledDistance;
    //  newvalue= (max'-min')/(max-min)*(value-max)+max'
    //y = (x – min) / (max – min)
    scaledDistance = (d - 10.0f) / (40.f - 10.0f);
    
    
    return scaledDistance;
    //return d;
}

float calculateLOD(float3 midPoint)
{
    
    // calculate distance to camera
    float d = calculadeScaledDistance(midPoint, cameraPosition.xyz);
    
    // when the ditance is very small -- big LOD
    float lod = clamp(lerp(64.f, 1.0f, d), 1, 10);

    return lod;
    
    
    
}



ConstantOutputType PatchConstantFunction(InputPatch<InputType, 12> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;
    
    // method with 12 control points
    float4 patchWorldPosition[12];
    // calculate the world position of each control point
    
    for (int i = 0; i < 12; i++)
    {
        patchWorldPosition[i] = mul(inputPatch[i].position, worldMatrix);
    }
   
    
    // determine the midpoint of each patch
    float3 midPoints[] =
    {
        // main quad
        computePatchMidPoint(patchWorldPosition[0].xyz, patchWorldPosition[1].xyz, patchWorldPosition[2].xyz, patchWorldPosition[3].xyz),
        // + X neighbour
        computePatchMidPoint(patchWorldPosition[2].xyz, patchWorldPosition[3].xyz, patchWorldPosition[4].xyz, patchWorldPosition[5].xyz),
        // + Z neighbour  
        computePatchMidPoint(patchWorldPosition[1].xyz, patchWorldPosition[2].xyz, patchWorldPosition[6].xyz, patchWorldPosition[7].xyz),
        // -X neighbour
        computePatchMidPoint(patchWorldPosition[0].xyz, patchWorldPosition[1].xyz, patchWorldPosition[8].xyz, patchWorldPosition[9].xyz),
        // -z neighbour
        computePatchMidPoint(patchWorldPosition[0].xyz, patchWorldPosition[3].xyz, patchWorldPosition[10].xyz, patchWorldPosition[11].xyz)
        
    };
    
    float patchLOD[] =
    {
        // main quad
        calculateLOD(midPoints[0]),
        // +x neighbour
        calculateLOD(midPoints[1]),
        // +z neighbour
        calculateLOD(midPoints[2]),
        // -x neighbour
        calculateLOD(midPoints[3]),
        // -z neighbour
        calculateLOD(midPoints[4])
    };
       
    //The interior factor is the LOD of the main quad
    if(dynamicTess)
    {
        output.inside[0] = patchLOD[0];
        output.inside[1] = patchLOD[0];
        
          // For the edges its more complex as we have to match
    // the neighboring patches. The rule in this case is:
    //
    // - If the neighbor patch is of a lower LOD we
    // pick that LOD as the edge for this patch.
    //
    // - If the neighbor patch is a higher LOD then 
    // we stick with our LOD and expect them to blend down
    // towards u
        output.edges[0] = min(patchLOD[0], patchLOD[4]);
        output.edges[1] = min(patchLOD[0], patchLOD[3]);
        output.edges[2] = min(patchLOD[0], patchLOD[2]);
        output.edges[3] = min(patchLOD[0], patchLOD[1]);
    }
    else
    {
        output.inside[0] = interiorTesselationFactor;
        output.inside[1] = interiorTesselationFactor;
        
        output.edges[0] = edgeTesselationFactor;
        output.edges[1] = edgeTesselationFactor;
        output.edges[2] = edgeTesselationFactor;
        output.edges[3] = edgeTesselationFactor;

    }
    
    return output;
}


[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 12> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;


    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;
    output.normal = patch[pointId].normal;
    // Set the input colour as the output colour. 
    output.texCoord = patch[pointId].texCoord;

    return output;
}