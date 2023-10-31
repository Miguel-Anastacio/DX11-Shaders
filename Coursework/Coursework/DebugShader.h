#include "DXF.h"
using namespace DirectX;
class DebugShader : public BaseShader
{
public:
	DebugShader(ID3D11Device* device, HWND hwnd);
	~DebugShader();
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

	ID3D11Buffer* matrixBuffer;
};

