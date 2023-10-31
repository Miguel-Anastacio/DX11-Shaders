#pragma once
#include "DXF.h"
// this class is used to generate the post process textures
// the pixel shader that it uses is passed in constructor
class PostProcessShader : public BaseShader
{
	struct BloomBufferType
	{
		float data; // screen width , screen height or cullCutOff
		XMFLOAT3 padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
	};


public:
	PostProcessShader(ID3D11Device* device, HWND hwnd, const wchar_t* ps);
	~PostProcessShader();
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, float data);
protected:
	void initShader(const wchar_t* cs, const wchar_t* ps);

	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* bloomBuffer;
};

