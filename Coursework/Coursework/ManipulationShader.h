#pragma once
#include "DXF.h"
using namespace std;
using namespace DirectX;

class ManipulationShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 ambient[4];
		XMFLOAT4 diffuse[4];
		XMFLOAT4 direction[4];
		XMFLOAT4 position[4];
		XMFLOAT4 attenuation [4];
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

public:
	ManipulationShader(ID3D11Device* device, HWND hwnd);
	~ManipulationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* textureB, ShadowMap* depthMap[19], Light* lights[4], XMFLOAT4 lightAttenuation [4]);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* timerBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
};
