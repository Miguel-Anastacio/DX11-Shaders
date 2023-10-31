#pragma once
#include "DepthShader.h"
const int WAVES = 2;
class DepthWaveShader : public DepthShader
{
	struct WaveParametes
	{
		float time;
		float waveLength;
		float steepness;
		float padding;
	};

	struct WaveBufferType
	{
		WaveParametes wave[2];
		XMFLOAT4 direction[2];
	};

public:
	DepthWaveShader(ID3D11Device* device, HWND hwnd);
	~DepthWaveShader();
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, XMFLOAT4 waveParameters [WAVES], XMFLOAT4 waveDirection [WAVES]);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps) override;
	void initWaveBuffer();

	ID3D11Buffer* waveBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
};

