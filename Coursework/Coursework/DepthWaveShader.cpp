#include "DepthWaveShader.h"

DepthWaveShader::DepthWaveShader(ID3D11Device* device, HWND hwnd) : DepthShader(device, hwnd)
{
	initShader(L"depth_wave_vs.cso", L"depth_ps.cso");
}

DepthWaveShader::~DepthWaveShader()
{
	if (waveBuffer)
	{
		delete waveBuffer;
		waveBuffer = nullptr;
	}

	if (matrixBuffer)
	{
		delete matrixBuffer;
		matrixBuffer = nullptr;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void DepthWaveShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	initMatrixBuffer();
	initWaveBuffer();

}

void DepthWaveShader::initWaveBuffer()
{
	D3D11_BUFFER_DESC waveBufferDesc;
	
	// Setup the description of the wave buffer that is in the vertex shader.
	waveBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	waveBufferDesc.ByteWidth = sizeof(WaveBufferType);
	waveBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	waveBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	waveBufferDesc.MiscFlags = 0;
	waveBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&waveBufferDesc, NULL, &waveBuffer);
}

void DepthWaveShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, XMFLOAT4 waveParameters[WAVES], XMFLOAT4 waveDirection[WAVES])
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(world);
	tview = XMMatrixTranspose(view);
	tproj = XMMatrixTranspose(projection);
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);


	WaveBufferType* wavePtr;
	result = deviceContext->Map(waveBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	wavePtr = (WaveBufferType*)mappedResource.pData;
	for (int i = 0; i < 2; i++)
	{
		wavePtr->wave[i].time = waveParameters[i].x;
		wavePtr->wave[i].waveLength = waveParameters[i].y;
		wavePtr->wave[i].steepness = waveParameters[i].z;
		wavePtr->wave[i].padding = waveParameters[i].w;;
		wavePtr->direction[i] = waveDirection[i];
	}

	deviceContext->Unmap(waveBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &waveBuffer);


}

