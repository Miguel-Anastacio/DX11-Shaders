// Light shader.h
// Basic single light shader setup
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class TessellationShader : public BaseShader
{
	struct TesselationBufferType
	{
		float edgeTesselationFactor;
		float interiorTesselationFactor;
		bool dynamicTess;
		float padding;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient[4];
		XMFLOAT4 diffuse[4];
		XMFLOAT4 direction[4];
		XMFLOAT4 position[4];
		XMFLOAT4 attenuation[4];
	};

	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView[1];
		XMMATRIX lightProjection[2];
		XMFLOAT4 lightPosition[3];
	};

	struct CameraBufferType
	{
		XMMATRIX world;
		XMFLOAT4 cameraPos;
	};

public:

	TessellationShader(ID3D11Device* device, HWND hwnd);
	~TessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, float edgeFactor, float interiorFactor, bool dynamicTess, 
		XMFLOAT4 cameraPos, ID3D11ShaderResourceView* textureHM, ID3D11ShaderResourceView* textureDiffuse, ShadowMap* depthMap[19], Light* lights[4], XMFLOAT4 lightAttenuation[4]);
	void setDisplacementTexture(ID3D11ShaderResourceView* hmap);
private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* tesselationBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* lightBuffer;

	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11ShaderResourceView* heightMap;
};
