// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"
#include "ManipulationShader.h"
#include "TessellationShader.h"
#include "DepthShader.h"
#include "DepthHeightMapShader.h"
#include "ShadowShader.h"
#include "DebugShader.h"
#include "PostProcessShader.h"
#include "BlendingShader.h"
#include "GerstnerWaveShader.h"
#include "DepthWaveShader.h"
#include "Lamp.h"
#include "TessellatedPlaneMesh.h"

class App1 : public BaseApplication
{
	struct WaveData
	{
		//x - time, y - wave length, z - steepness, w - padding
		XMFLOAT4 parameters [2];
		float guiPa[2][2]{ 14.79, 0.19, 8.95, 0.05 };
		XMFLOAT4 direction [2];
		float guiDir[2][2] = { -1, 1, -1, -0.6 };
		float translation[3] = { -29.0f, -0.5f, -12.f };
	};

public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void firstPass();
	void depthPass();
	void finalPass();

	//postProcess
	void bloomPass();
	void verticalBlur(RenderTexture* textureToBlur, RenderTexture* target, OrthoMesh* mesh);
	void horizontalBlur(RenderTexture* textureToBlur, RenderTexture* target, OrthoMesh* mesh);
	void downUPTexture(RenderTexture* textureToChange, RenderTexture* target, OrthoMesh* mesh);
	void addTwoTextures(RenderTexture* textureA, RenderTexture* textureB, RenderTexture* target, OrthoMesh* mesh);

	// generate depth map for pointLight
	void generateDepthMapPointLight(Lamp* lamp);

	// render different objects in the scene
	void renderWaves(XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ShadowMap* maps[19]);
	//void renderHeightMap();
	void renderLamp(Lamp* lamp, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ShadowMap* maps[19]);
	void renderHouse(XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ShadowMap* maps[19]);
	void renderLampToDepthMap(Lamp* l, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
	void renderHouseToDepthMap(XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

	//debug
	void setupDebugSphereLights(Light* [3]);
	void renderSphereLights(XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
	void orthoTextureRender(ID3D11ShaderResourceView*, OrthoMesh*);

	// setup array with the attenuation of each light
	void setupLightAttenuation();

	// setup wave properties
	void setupWave();
	void updateWaveTime();

	// change scene parameter through gui
	void gui();
	void lightGUI();
	void waveGUI();

	// no post process
	// enabled when wireframe mode is true and post process is false
	void renderNoPostProcess();

	//combine all shadow maps into an array
	void combineShadowMaps(ShadowMap* map[19]);

	// cleanup fucntions
	void cleanupTextures();
	void cleanupLights();
	void cleanupShaders();
	void cleanupMeshes();


private:

	TessellationShader* heightMapShader;
	GerstnerWaveShader* gerstnerWaveShader;
	DepthWaveShader* gerstnerWaveDepthShader;

	DepthShader* depthShader;
	DepthHeightMapShader* depthHeightMapShader;
	ShadowShader* shadowShader;
	DebugShader* debugShader;

	PostProcessShader* bloomShader;
	BlendingShader* blendingShader;
	PostProcessShader* horizontalBlurShader;
	PostProcessShader* verticalBlurShader;

	TessellatedPlaneMesh* tessellatedMesh;
	float edgeTessellationFactor = 1;
	float interiorTessellationFactor = 1;
	bool dynamicTessellation = false;

	PlaneMesh* waveMesh;
	AModel* lampModel;

	AModel* houseModel;
	XMMATRIX houseTransaltion = XMMatrixTranslation(-20.f, 1.f, 5.f);

	Light* dirLight;
	Light* redPointLight;
	Light* pointLight;
	Light* bluePointLight;

	Lamp* lamp;
	Lamp* lamp1;
	Lamp* lamp2;

	int index;

	WaveData wavesData;

	ShadowMap* shadowMapDirLight ;
	Light* lights [4];
	XMFLOAT4 lightAttenuation[4];

	XMMATRIX lightTranslation[2];
	SphereMesh* lightSpheresMesh[2];

	float angleRot = 0;
	bool postProccess = true;
	bool waveShadowsActive = true;
	bool doMultiplePasses = true;
	// directional Light
	float directionDir[3] = { -0.f, -0.5f, 1.0f };
	float positionDir[3] = { 20.f, 20.43f, 0.14f };

	//spotlight
	float directionSpot[3] = {0.0f, -1.0f, 0.0f };
	float positionSpot[3] = { 0.0f, 6.5f, 0.f };

	// bloom
	float cutOFF = 0.5f;

	// floor translation
	XMMATRIX terrainTranslation = XMMatrixTranslation(-5.0f, 0.0f, 10.0f);
	
	

	// render to texture
	OrthoMesh* halfScreenMesh;
	OrthoMesh* quarterScreenMesh;
	OrthoMesh* fullScreenOrthoMesh;

	RenderTexture* renderTexture;

	RenderTexture* bloomMapTexture;
	RenderTexture* bloomMapTextureDown;

	RenderTexture* horizontalBlurTexture;
	RenderTexture* horizontalBlurTextureDown;

	RenderTexture* verticalBlurTexture;
	RenderTexture* halfResVertBlurTextureDown;
	RenderTexture* halfResVertBlurTextureUP;
	RenderTexture* quarterResVertBlurTextureDown;
	RenderTexture* quarterResBlurComplete;

	RenderTexture* fullBlurTexture;

	RenderTexture* sceneTexture;
};

#endif