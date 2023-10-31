#pragma once
#include "DXF.h"
class GerstnerWaveShader : public BaseShader
{
private:
	struct WaveParametes
	{
		float time;
		float waveLength;
		float steepness;
		float padding;
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

	struct LightBufferType
	{
		XMFLOAT4 ambient[4];
		XMFLOAT4 diffuse[4];
		XMFLOAT4 direction[4];
		XMFLOAT4 position[4];
		XMFLOAT4 attenuation[4];
	};

	struct WaveBufferType
	{
		WaveParametes wave[2];
		XMFLOAT4 direction[2];
	};

public:
	GerstnerWaveShader(ID3D11Device* device, HWND hwnd, const wchar_t* ps);
	~GerstnerWaveShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, 
		ID3D11ShaderResourceView* texture, ShadowMap* shadowMap[19], Light*light [4], XMFLOAT4 lightAtt [4], XMFLOAT4 wave_parameters[2], XMFLOAT4 wave_dir[2]);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* waveBuffer;
};

