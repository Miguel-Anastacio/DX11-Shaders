#pragma once
#include "DXF.h"
using namespace std;
using namespace DirectX;
class DepthShader : public BaseShader
{
public:

	DepthShader(ID3D11Device* device, HWND hwnd);
	~DepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection);

protected:
	virtual void initShader(const wchar_t* vs, const wchar_t* ps) ;
	void initMatrixBuffer();
	ID3D11Buffer* matrixBuffer;
};

