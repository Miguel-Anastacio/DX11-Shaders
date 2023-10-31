#pragma once
#include "DXF.h"
class DepthHeightMapShader : public BaseShader
{
public:
	struct TesselationBufferType
	{
		float edgeTesselationFactor;
		float interiorTesselationFactor;
		XMFLOAT2 padding;
	};

	struct CameraBufferType
	{
		XMMATRIX world;
		XMFLOAT4 cameraPos;
	};


	DepthHeightMapShader(ID3D11Device* device, HWND hwnd);
	~DepthHeightMapShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* heightMapTexture, float edgeFactor, float intFactor, bool dynamicTess, XMFLOAT3 cameraPos);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* tessellationBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11SamplerState* sampleState;
};

