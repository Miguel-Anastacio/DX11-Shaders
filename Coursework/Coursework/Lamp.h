#pragma once
#include "DXF.h"
class Lamp
{
public:
	Lamp(ID3D11Device* device, int mapWidth, int mapHeight, Light* light);
	~Lamp();

	ShadowMap* shadowMaps[6];
	XMMATRIX lightView[6];
	void setTranslation(XMFLOAT3 t);
	XMFLOAT3 getTranslation() { return translation; };
	XMMATRIX getTranslationMatrix() { return tMatrix; };
	void setAttenuation(XMFLOAT4 att) { attenuation = att; };
	XMFLOAT4 getAttenuation() { return attenuation; };

	void setScale(XMFLOAT3 sc) { scale = sc; };
	XMFLOAT3 getScale() { return scale; };
	// generate a view for each normal of the cube around the light
	void createLightView();
	

private:
	Light* pointLight;
	XMFLOAT3 translation;
	XMMATRIX tMatrix;
	XMFLOAT3 scale;
	XMFLOAT4 cubeFaceNormal[6] =
	{
		XMFLOAT4(1.0, 0.0, 0.0, 0.0f),
		XMFLOAT4(-1.0, 0.0, 0.0, 0.0f),
		XMFLOAT4(0.0, 1.0, 0.0, 0.0f),
		XMFLOAT4(0.0, -1.0, 0.0, 0.0f),
		XMFLOAT4(0.0, 0.0, 1.0, 0.0f),
		XMFLOAT4(0.0, 0.0, -1.0, 0.0f),
	};
	XMFLOAT4 attenuation;


	void initShadowMaps(ID3D11Device* device, int mWidth, int mHeight);
	// pass light position
	// generate a view for each normal of the cube around the light
};

