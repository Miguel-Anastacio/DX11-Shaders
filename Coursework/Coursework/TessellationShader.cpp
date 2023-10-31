// tessellation shader.cpp
#include "tessellationshader.h"


TessellationShader::TessellationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"tessellation_vs.cso", L"tessellation_hs.cso", L"tessellatedHeightMap_ds.cso", L"manipulation_ps.cso");
}


TessellationShader::~TessellationShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (sampleStateShadow)
	{
		sampleStateShadow->Release();
		sampleStateShadow = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	if (tesselationBuffer)
	{
		tesselationBuffer->Release();
		tesselationBuffer = 0;
	}
	if (cameraBuffer)
	{
		cameraBuffer->Release();
		cameraBuffer = 0;
	}
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}
	
	//Release base shader components
	BaseShader::~BaseShader();
}

void TessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_SAMPLER_DESC samplerDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	D3D11_BUFFER_DESC tesselationBufferDesc;
	tesselationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tesselationBufferDesc.ByteWidth = sizeof(TesselationBufferType);
	tesselationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tesselationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tesselationBufferDesc.MiscFlags = 0;
	tesselationBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&tesselationBufferDesc, NULL, &tesselationBuffer);


	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleState);


	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);


	// Setup light buffer
// Setup the description of the light dynamic constant buffer that is in the pixel shader.
// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
	
}

void TessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void TessellationShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, float edgeFactor, float interiorFactor, bool status, XMFLOAT4 cameraPos, ID3D11ShaderResourceView* textureHeightMap, ID3D11ShaderResourceView* texture, ShadowMap* shadowMap[19], Light* lights[4], XMFLOAT4 lightAttenuation[4])
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	for (int i = 0; i < 2; i++)
	{
		// Transpose the matrices to prepare them for the shader.
		XMMATRIX tLightViewMatrix = XMMatrixTranspose(lights[0]->getViewMatrix());
		XMMATRIX tLightProjectionMatrix = XMMatrixTranspose(lights[i]->getOrthoMatrix());
		if (i == 1)
			tLightProjectionMatrix = XMMatrixTranspose(lights[i]->getProjectionMatrix());
		dataPtr->world = tworld;// worldMatrix;
		dataPtr->view = tview;
		dataPtr->projection = tproj;
		dataPtr->lightView[0] = tLightViewMatrix;
		dataPtr->lightProjection[i] = tLightProjectionMatrix;
	}

	dataPtr->lightPosition[0] = XMFLOAT4(lights[1]->getPosition().x, lights[1]->getPosition().y, lights[1]->getPosition().z, 0.0f);
	dataPtr->lightPosition[1] = XMFLOAT4(lights[2]->getPosition().x, lights[2]->getPosition().y, lights[2]->getPosition().z, 0.0f);
	dataPtr->lightPosition[2] = XMFLOAT4(lights[3]->getPosition().x, lights[3]->getPosition().y, lights[3]->getPosition().z, 0.0f);

	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(tesselationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	TesselationBufferType* tessPtr = (TesselationBufferType*)mappedResource.pData;
	tessPtr->edgeTesselationFactor = edgeFactor;
	tessPtr->interiorTesselationFactor = interiorFactor;
	tessPtr->dynamicTess = status;
	tessPtr->padding = 0.0f;
	deviceContext->Unmap(tesselationBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tesselationBuffer);

	result = deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CameraBufferType* cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPos = cameraPos;
	cameraPtr->world = tworld;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->HSSetConstantBuffers(1, 1, &cameraBuffer);


	// Send light data to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	for (int i = 0; i < 4; i++)
	{
		lightPtr = (LightBufferType*)mappedResource.pData;
		lightPtr->diffuse[i] = lights[i]->getDiffuseColour();
		lightPtr->direction[i] = XMFLOAT4(lights[i]->getDirection().x, lights[i]->getDirection().y, lights[i]->getDirection().z, 1.0f);
		lightPtr->ambient[i] = lights[i]->getAmbientColour();
		lightPtr->position[i] = XMFLOAT4(lights[i]->getPosition().x, lights[i]->getPosition().y, lights[i]->getPosition().z, 1.0f);
		lightPtr->attenuation[i] = lightAttenuation[i];
	}

	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	// send height map texture and sampler to the domain shader
	deviceContext->DSSetSamplers(0, 1, &sampleState);
	deviceContext->DSSetShaderResources(0, 1, &textureHeightMap);


	// Set shader texture and sampler resource in the pixel shader.
	ID3D11ShaderResourceView* heightMapTextures[2];
	heightMapTextures[0] = textureHeightMap;
	heightMapTextures[1] = texture;

	deviceContext->PSSetShaderResources(0, 2, heightMapTextures);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);


	ID3D11ShaderResourceView* depthMap[19];
	for (int i = 0; i < 19; i++)
	{
		depthMap[i] = shadowMap[i]->getDepthMapSRV();
	}
	deviceContext->PSSetShaderResources(2, 19, depthMap);
}

void TessellationShader::setDisplacementTexture(ID3D11ShaderResourceView* hmap)
{
	heightMap = hmap;
}


