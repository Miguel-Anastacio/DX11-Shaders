#include "DepthHeightMapShader.h"
#include "DepthShader.h"
DepthHeightMapShader::DepthHeightMapShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"tessellation_vs.cso", L"tessellation_hs.cso", L"depth_heightMap_ds.cso",L"depth_ps.cso");
}

DepthHeightMapShader::~DepthHeightMapShader()
{
	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	if (tessellationBuffer)
	{
		tessellationBuffer->Release();
		tessellationBuffer = 0;
	}
	if (cameraBuffer)
	{
		cameraBuffer->Release();
		cameraBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void DepthHeightMapShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}

void DepthHeightMapShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);


	// Setup the description of the  camera constant buffer that is in the domain shader.
	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	// Setup the description of the tessellation constant buffer that is in the domain shader.
	D3D11_BUFFER_DESC tesselationBufferDesc;
	tesselationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tesselationBufferDesc.ByteWidth = sizeof(TesselationBufferType);
	tesselationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tesselationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tesselationBufferDesc.MiscFlags = 0;
	tesselationBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&tesselationBufferDesc, NULL, &tessellationBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

}

void DepthHeightMapShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* heightMapTexture, float edgeFactor,float intFactor, bool status, XMFLOAT3 cameraPos)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	TesselationBufferType* tessPtr = (TesselationBufferType*)mappedResource.pData;
	tessPtr->edgeTesselationFactor = edgeFactor;
	// not in use
	tessPtr->interiorTesselationFactor = 0.0f;
	tessPtr->padding = XMFLOAT2(0.0f, 0.0f);
	deviceContext->Unmap(tessellationBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tessellationBuffer);

	result = deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CameraBufferType* cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPos = XMFLOAT4(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);
	cameraPtr->world = tworld;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->HSSetConstantBuffers(1, 1, &cameraBuffer);


	// Set shader texture resource in the vertex shader.
	deviceContext->DSSetShaderResources(0, 1, &heightMapTexture);
	deviceContext->DSSetSamplers(0, 1, &sampleState);
}