#pragma once
#include "DXF.h"
// setup shader to blend two textures
class BlendingShader : public BaseShader
{

public:
	BlendingShader(ID3D11Device* device, HWND hwnd);
	~BlendingShader();
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* textureA, ID3D11ShaderResourceView* textureB);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateBlend;
};

