// this calculation is implemented using the formula provided in the direct X documentation 
// for the function XMMatrixLookAtLH https://learn.microsoft.com/en-us/previous-versions/windows/desktop/bb281710(v=vs.85)
void matrixLookAt(float3 position, float3 lookAt, float3 up, inout matrix lightView)
{
    float3 axisZ = normalize(lookAt - position);
    float3 axisX = normalize(cross(up, axisZ));
    float3 axisY = cross(axisZ, axisX);
    
    matrix M =
    {
        axisX.x, axisY.x, axisZ.x, 0,
        axisX.y, axisY.y, axisZ.y, 0,
        axisX.z, axisY.z, axisZ.z, 0,
        -dot(axisX, position), -dot(axisY, position), -dot(axisZ, position), 1
    };

    lightView = M;
}

void calculateLightViewMatrix(float3 cubeNormal, float3 position, inout matrix lightView)
{
    // code taken from the generate view matrix fucntion from the light class
    float4 up, lookAt;
	// default up vector
    up = float4(0.0f, 1.0f, 0.0f, 1.0f);
    
    // deal with cases where we have to change the vector up 
    if (cubeNormal.y == 1 || (cubeNormal.x == 0 && cubeNormal.z == 0))
        up = float4(0.0f, 0.0f, 1.0f, 1.0f);
    else if (cubeNormal.y == -1 || (cubeNormal.x == 0 && cubeNormal.z == 0))
    {
        up = float4(0.0f, 0.0f, -1.0f, 1.0f);
    }

    float4 dir = float4(cubeNormal.x, cubeNormal.y, cubeNormal.z, 1.0f);
    float3 right = cross(dir.xyz, up.xyz);

    up.xyz = cross(right, dir.xyz);

    lookAt = float4(position.x + cubeNormal.x, position.y + cubeNormal.y, position.z + cubeNormal.z, 1.0f);
	// Create the view matrix from the three vectors.
    matrixLookAt(position, lookAt.xyz, up.xyz, lightView);
}


// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection.xyz));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

float4 createSpotlight(float3 vertexPosition, float3 position, float4 direction, float3 normal, float4 diffuse, float4 attenuation)
{
    float4 textureColour;
    float4 lightColour ;

	// get vector between light and pixel position
    float3 lightToPixel = vertexPosition - position.xyz;
    
	// find the distance between light and pixel
    float d = length(lightToPixel);
     //If pixel is too far, return pixel color with ambient light
	
    lightToPixel = normalize(lightToPixel);
    float cosineAngle = dot(lightToPixel, normalize(normal));
     //Add light to the finalColor of the pixel
    lightColour = diffuse;
                    
    //Calculate Light's Distance Falloff factor
    lightColour = lightColour / (attenuation.x + (attenuation.y * d)) + (attenuation.z * (d * d));

    //Calculate falloff from center to edge of pointlight cone
    lightColour *= pow(max(dot(lightToPixel, normalize(direction.xyz)), 0.0f), attenuation.w);
    return lightColour;
}

// calculate lighting for a point light
float4 calculatePointLight(float3 worldPosition, float4 lightPosition, float3 normal, float4 diffuse, float4 attenuation)
{
    float3 lightDirection = normalize(lightPosition.xyz - worldPosition);
    float intensity = dot(normal, lightDirection);
    float d = distance(lightPosition.xyz, worldPosition);
    
    intensity = intensity / (attenuation.x + (attenuation.y * d) + (attenuation.z * (d * d)));
    float4 colour = saturate(diffuse * intensity);
    return colour;
    
}

// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}