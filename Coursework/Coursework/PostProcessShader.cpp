#include "PostProcessShader.h"

PostProcessShader::PostProcessShader(ID3D11Device* device, HWND hwnd, const wchar_t* pixel_shader) : BaseShader(device, hwnd)
{
	initShader(L"basic_vs.cso", pixel_shader);
}

PostProcessShader::~PostProcessShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
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
	if (bloomBuffer)
	{
		bloomBuffer->Release();
		bloomBuffer = 0;
	}


	BaseShader::~BaseShader();
}

void PostProcessShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, float data )
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	BloomBufferType* bloomDataPtr;
	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	deviceContext->Map(bloomBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	bloomDataPtr = (BloomBufferType*)mappedResource.pData;
	bloomDataPtr->data = data;

	deviceContext->Unmap(bloomBuffer, 0);
	// set the constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(0, 1, &bloomBuffer);

	// Set shader texture and sampler resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}

void PostProcessShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC bloomBufferDesc;
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

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

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


	// Setup bloom buffer
	bloomBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bloomBufferDesc.ByteWidth = sizeof(BloomBufferType);
	bloomBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bloomBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bloomBufferDesc.MiscFlags = 0;
	bloomBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&bloomBufferDesc, NULL, &bloomBuffer);


	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleState);
}