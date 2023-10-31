#include "App1.h"

App1::App1()
{
	heightMapShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Load texture.
	textureMgr->loadTexture(L"heightMap", L"res/Grand Mountain/Height Map PNG.png");
	textureMgr->loadTexture(L"terrain", L"res/Grand Mountain/Diffuse Map Sandy PNG.png");
	textureMgr->loadTexture(L"wood", L"res/wood.png");
	textureMgr->loadTexture(L"water", L"res/water.png");
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");

	waveMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 100);
	lampModel = new AModel(renderer->getDevice(), "res/new lamp post.obj");
	houseModel = new AModel(renderer->getDevice(), "res/wood house.obj");
	tessellatedMesh = new TessellatedPlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 50);

	heightMapShader = new TessellationShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);
	debugShader = new DebugShader(renderer->getDevice(), hwnd);
	depthHeightMapShader = new DepthHeightMapShader(renderer->getDevice(), hwnd);

	gerstnerWaveShader = new GerstnerWaveShader(renderer->getDevice(), hwnd,L"shadow_ps.cso");
	gerstnerWaveDepthShader = new DepthWaveShader(renderer->getDevice(), hwnd);


	bloomShader = new PostProcessShader(renderer->getDevice(), hwnd, L"bloom_ps.cso");
	verticalBlurShader = new PostProcessShader(renderer->getDevice(), hwnd, L"horizontalBlur_ps.cso");
	horizontalBlurShader = new PostProcessShader(renderer->getDevice(), hwnd, L"verticalBlur_ps.cso");
	blendingShader = new BlendingShader(renderer->getDevice(), hwnd);
	

	// ortho meshes with different sizes to downsample and upsample textures
	quarterScreenMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth/4, screenHeight/4, 0, 0);
	halfScreenMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth/2, screenHeight/2, 0, 0);
	fullScreenOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);


	// stores the first render of the scene 
	renderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	// stores bloom map of the scene
	bloomMapTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	bloomMapTextureDown = new RenderTexture(renderer->getDevice(), screenWidth/2, screenHeight/2, SCREEN_NEAR, SCREEN_DEPTH);

	// stores the result of the horizontal blur shader in different resolutions
	horizontalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth/2, screenHeight/2, SCREEN_NEAR, SCREEN_DEPTH);
	horizontalBlurTextureDown = new RenderTexture(renderer->getDevice(), screenWidth/4, screenHeight/4, SCREEN_NEAR, SCREEN_DEPTH);

	verticalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth/2, screenHeight/2, SCREEN_NEAR, SCREEN_DEPTH);

	halfResVertBlurTextureDown = new RenderTexture(renderer->getDevice(), screenWidth/2, screenHeight/2, SCREEN_NEAR, SCREEN_DEPTH);
	halfResVertBlurTextureUP = new RenderTexture(renderer->getDevice(), screenWidth/2, screenHeight/2, SCREEN_NEAR, SCREEN_DEPTH);

	quarterResVertBlurTextureDown = new RenderTexture(renderer->getDevice(), screenWidth / 4, screenHeight / 4, SCREEN_NEAR, SCREEN_DEPTH);
	quarterResBlurComplete= new RenderTexture(renderer->getDevice(), screenWidth / 4, screenHeight / 4, SCREEN_NEAR, SCREEN_DEPTH);

	fullBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	sceneTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);


	//cubeMesh = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	// Variables for defining shadow map
	int shadowmapWidth = 1024;
	int shadowmapHeight = 1024;
	int sceneWidth = 100;
	int sceneHeight = 100;

	// Configure directional light
	dirLight = new Light();
	dirLight->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
	dirLight->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	dirLight->setDirection(0.0f, -0.5f, 1.0f);
	dirLight->setPosition(20.f, 30.f, -10.f);
	dirLight->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	// This is your shadow map for the directional light
	shadowMapDirLight = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	// configure  lights
	redPointLight = new Light();
	redPointLight->setAmbientColour(0.f, 0.f, 0.f, 1.0f);
	redPointLight->setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	redPointLight->setDirection(0.0f, 0.f, 0.f);
	redPointLight->setPosition(0.0f, 0.0f, .0f);
	redPointLight->generateProjectionMatrix(0.1f, 100.f);

	pointLight = new Light();
	pointLight->setDiffuseColour(1, 1.0f, 1.0f, 1.0f);
	pointLight->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	pointLight->setDirection(0.0f, 0.0f, 0.0f);
	pointLight->setPosition(0.0f, 0.0f, 0.0f);
	pointLight->generateProjectionMatrix(0.1f, 100.0f);

	bluePointLight = new Light();
	bluePointLight->setDiffuseColour(0.0, 0.0f, 1.0f, 1.0f);
	bluePointLight->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	bluePointLight->setDirection(0.0f, 0.0f, 0.0f);
	bluePointLight->setPosition(0.0f, 0.0f, 0.0f);
	bluePointLight->generateProjectionMatrix(0.1f, 100.0f);

	lights[0] = dirLight;
	lights[1] = pointLight;
	lights[2] = redPointLight;
	lights[3] = bluePointLight;

	lightAttenuation[0] = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	
	// white lamp
	lamp = new Lamp(renderer->getDevice(), shadowmapWidth, shadowmapHeight, lights[1]);
	lamp->setTranslation(XMFLOAT3(1.3, 0, 15));
	// red lamp
	lamp1 = new Lamp(renderer->getDevice(), shadowmapWidth, shadowmapHeight, lights[2]);
	lamp1->setTranslation(XMFLOAT3(40, 0, 20));
	// blue lamp
	lamp2 = new Lamp(renderer->getDevice(), shadowmapWidth, shadowmapHeight, lights[3]);
	lamp2->setTranslation(XMFLOAT3(40, 0, 40));

	setupDebugSphereLights(lights);

	setupWave();
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	cleanupLights();
	cleanupMeshes();
	cleanupShaders();
	cleanupTextures();
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	updateWaveTime();

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	//update light sphere translation
	for (int i = 0; i < 2; i++)
	{
		lightTranslation[i] =  XMMatrixTranslation(lights[i]->getPosition().x *5, lights[i]->getPosition().y*5 , lights[i]->getPosition().z*5) ;
	}
	//lamp->setTranslation(lights[2]->getPosition());

	// Perform depth pass
	depthPass();
		
	firstPass();
	bloomPass();
	// down sample texture
	downUPTexture(bloomMapTexture, bloomMapTextureDown, halfScreenMesh);
	// blur it
	horizontalBlur(bloomMapTextureDown, horizontalBlurTexture, halfScreenMesh);
	verticalBlur(horizontalBlurTexture, halfResVertBlurTextureDown, halfScreenMesh);

	if (doMultiplePasses)
	{
		// down sample 
		downUPTexture(halfResVertBlurTextureDown, quarterResVertBlurTextureDown, quarterScreenMesh);
		// blur it
		horizontalBlur(quarterResVertBlurTextureDown, horizontalBlurTextureDown, quarterScreenMesh);
		verticalBlur(horizontalBlurTextureDown, quarterResBlurComplete, quarterScreenMesh);
		// up sample it
		downUPTexture(quarterResBlurComplete, halfResVertBlurTextureUP, halfScreenMesh);
		// add it to the first blurred texture
		addTwoTextures(halfResVertBlurTextureUP, halfResVertBlurTextureDown, verticalBlurTexture, halfScreenMesh);
		// up sample it
		downUPTexture(verticalBlurTexture, fullBlurTexture, fullScreenOrthoMesh);
	}
	else
	{
		// up sample texture
		downUPTexture(halfResVertBlurTextureDown, fullBlurTexture, fullScreenOrthoMesh);
	}
	addTwoTextures(renderTexture, fullBlurTexture, sceneTexture, fullScreenOrthoMesh);
	
	// Render scene
	finalPass();

	return true;

}

void App1::firstPass()
{
	// set the render target to be the render texture and clear it
	renderTexture->setRenderTarget(renderer->getDeviceContext());
	renderTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	// get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// combine shadow maps created in the depth pass
	ShadowMap* multipleMaps [19];
	combineShadowMaps(multipleMaps);

	setupLightAttenuation();

	//render floor
	worldMatrix = terrainTranslation;
	tessellatedMesh->sendData(renderer->getDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST);
	XMFLOAT4 cameraPos = XMFLOAT4(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z, 0.f);
	heightMapShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		edgeTessellationFactor, interiorTessellationFactor, dynamicTessellation, cameraPos, textureMgr->getTexture(L"heightMap"), textureMgr->getTexture(L"terrain"), multipleMaps, lights, lightAttenuation);
	heightMapShader->render(renderer->getDeviceContext(), tessellatedMesh->getIndexCount());
	
	renderWaves(viewMatrix, projectionMatrix, multipleMaps);

	// render lamp
	renderLamp(lamp, viewMatrix, projectionMatrix, multipleMaps);
	renderLamp(lamp1, viewMatrix, projectionMatrix, multipleMaps);
	renderLamp(lamp2, viewMatrix, projectionMatrix, multipleMaps);

	// render hosue
	renderHouse(viewMatrix, projectionMatrix, multipleMaps);

	renderer->setBackBufferRenderTarget();
}

void App1::verticalBlur(RenderTexture* textureToBlur, RenderTexture* target, OrthoMesh* mesh)
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeY = (float)target->getTextureHeight();
	target->setRenderTarget(renderer->getDeviceContext());
	target->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = target->getOrthoMatrix();

	// Render for Vertical Blur
	renderer->setZBuffer(false);
	mesh->sendData(renderer->getDeviceContext());
	verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, textureToBlur->getShaderResourceView(), screenSizeY);
	verticalBlurShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::horizontalBlur(RenderTexture* textureToBlur, RenderTexture* target, OrthoMesh* mesh)
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)target->getTextureWidth();
	target->setRenderTarget(renderer->getDeviceContext());
	target->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = target->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	mesh->sendData(renderer->getDeviceContext());
	horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, textureToBlur->getShaderResourceView(), screenSizeX);
	horizontalBlurShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::downUPTexture(RenderTexture* textureToChange, RenderTexture* target, OrthoMesh* mesh)
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	target->setRenderTarget(renderer->getDeviceContext());
	target->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = target->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	mesh->sendData(renderer->getDeviceContext());
	debugShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, textureToChange->getShaderResourceView());
	debugShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}


void App1::bloomPass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix, projectionViewMatrix, projectionMatrix;

	bloomMapTexture->setRenderTarget(renderer->getDeviceContext());
	bloomMapTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	projectionViewMatrix = camera->getViewMatrix();

	orthoMatrix = bloomMapTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	fullScreenOrthoMesh->sendData(renderer->getDeviceContext());
	bloomShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, renderTexture->getShaderResourceView(), cutOFF);
	bloomShader->render(renderer->getDeviceContext(), fullScreenOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::orthoTextureRender(ID3D11ShaderResourceView* texture, OrthoMesh* mesh )
{
	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	renderer->setZBuffer(false);
	mesh->sendData(renderer->getDeviceContext());
	debugShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, texture);
	debugShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
	renderer->setZBuffer(true);
}

void App1::setupLightAttenuation()
{
	lightAttenuation[1] = lamp->getAttenuation();
	lightAttenuation[2] = lamp1->getAttenuation();
	lightAttenuation[3] = lamp2->getAttenuation();
}

void App1::depthPass()
{
	// render to the directional light depth map
	shadowMapDirLight->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.
	lights[0]->generateViewMatrix();
	XMMATRIX lightViewMatrix = lights[0]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = lights[0]->getOrthoMatrix();

	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	worldMatrix = terrainTranslation;
	// Render floor
	tessellatedMesh->sendData(renderer->getDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST);
	depthHeightMapShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"heightMap"), edgeTessellationFactor, interiorTessellationFactor, dynamicTessellation, camera->getPosition());
	depthHeightMapShader->render(renderer->getDeviceContext(), tessellatedMesh->getIndexCount());

		
	if (waveShadowsActive)
	{
		worldMatrix = XMMatrixTranslation(wavesData.translation[0], wavesData.translation[1], wavesData.translation[2]);
		waveMesh->sendData(renderer->getDeviceContext());
		gerstnerWaveDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, wavesData.parameters, wavesData.direction);
		gerstnerWaveDepthShader->render(renderer->getDeviceContext(), waveMesh->getIndexCount());
	}
		
	renderLampToDepthMap(lamp, lightViewMatrix, lightProjectionMatrix);
	renderLampToDepthMap(lamp1, lightViewMatrix, lightProjectionMatrix);
	renderLampToDepthMap(lamp2, lightViewMatrix, lightProjectionMatrix);
	renderHouseToDepthMap(lightViewMatrix, lightProjectionMatrix);
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
	

	// pointlight - lamp shadows
	generateDepthMapPointLight(lamp);
	generateDepthMapPointLight(lamp1);
	generateDepthMapPointLight(lamp2);


	// Set back buffer as render target and reset view port.

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::addTwoTextures(RenderTexture* textureA, RenderTexture* textureB, RenderTexture* target, OrthoMesh* mesh)
{
	target->setRenderTarget(renderer->getDeviceContext());
	target->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	XMMATRIX viewMatrix = camera->getOrthoViewMatrix();
	//projectionViewMatrix = camera->getViewMatrix();
	XMMATRIX orthoMatrix = target->getOrthoMatrix();

	renderer->setZBuffer(false);
	mesh->sendData(renderer->getDeviceContext());
	blendingShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, textureA->getShaderResourceView(), textureB->getShaderResourceView());
	blendingShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
	renderer->setZBuffer(true);
	// Set back buffer as render target and reset view port.

	renderer->setBackBufferRenderTarget();
}

void App1::generateDepthMapPointLight(Lamp* lampBulb)
{
	for (int i = 0; i < 6; i++)
	{
		lampBulb->createLightView();
		XMMATRIX viewMatrix, worldMatrix, projectionMatrix;
		projectionMatrix = lights[2]->getProjectionMatrix();
		lampBulb->shadowMaps[i]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());
		viewMatrix = lampBulb->lightView[i];

		worldMatrix = terrainTranslation;
		tessellatedMesh->sendData(renderer->getDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST);
		depthHeightMapShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"heightMap"), edgeTessellationFactor,interiorTessellationFactor, dynamicTessellation, camera->getPosition());
		depthHeightMapShader->render(renderer->getDeviceContext(), tessellatedMesh->getIndexCount());

		// Render lamps
		renderLampToDepthMap(lamp, viewMatrix, projectionMatrix);
		renderLampToDepthMap(lamp1, viewMatrix, projectionMatrix);
		renderLampToDepthMap(lamp2, viewMatrix, projectionMatrix);
		
		renderHouseToDepthMap(viewMatrix, projectionMatrix);

		if (waveShadowsActive)
		{
			worldMatrix = XMMatrixTranslation(wavesData.translation[0], wavesData.translation[1], wavesData.translation[2]);
			waveMesh->sendData(renderer->getDeviceContext());
			gerstnerWaveDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, wavesData.parameters, wavesData.direction);
			gerstnerWaveDepthShader->render(renderer->getDeviceContext(), waveMesh->getIndexCount());
		}

		renderer->setBackBufferRenderTarget();
		renderer->resetViewport();
	}

}

void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	camera->update();

	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering
	ID3D11ShaderResourceView* texture;
	if (postProccess)
	{
		texture = sceneTexture->getShaderResourceView();
	}
	else
	{
		texture = renderTexture->getShaderResourceView();
	}

	if (!postProccess && wireframeToggle)
		renderNoPostProcess();
	else
	{
		renderer->setZBuffer(false);
		fullScreenOrthoMesh->sendData(renderer->getDeviceContext());
		debugShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, texture);
		debugShader->render(renderer->getDeviceContext(), fullScreenOrthoMesh->getIndexCount());
		renderer->setZBuffer(true);
	}

	gui();
	renderer->endScene();
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	if (ImGui::CollapsingHeader("Lights"))
	{
		lightGUI();
	}

	waveGUI();

	if (ImGui::CollapsingHeader("Post Process"))
	{
		ImGui::Checkbox("Bloom", &postProccess);
		ImGui::Checkbox("Multiple Passes", &doMultiplePasses);
		ImGui::SliderFloat("Bloom Cut off: ", &cutOFF, 0.0f, 1.0f, "%.3f");
	}

	if (ImGui::CollapsingHeader("Tessellation"))
	{
		ImGui::Checkbox("Dynamic", &dynamicTessellation);
		ImGui::SliderFloat("Edge Factor", &edgeTessellationFactor, 1, 64);
		ImGui::SliderFloat("Interior Factor", &interiorTessellationFactor, 1, 64);
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void App1::setupDebugSphereLights(Light* lights [3])
{
	for (int i = 0; i < 2; i++)
	{
		lightSpheresMesh[i] = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
		lightTranslation[i] = XMMatrixTranslation(lights[i]->getPosition().x , lights[i]->getPosition().y, lights[i]->getPosition().z);
	}

}

void App1::renderSphereLights(XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	for (int i = 0; i < 2; i++)
	{
		XMMATRIX worldMatrix;
		
		worldMatrix = lightTranslation[i];
		
		XMMATRIX scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
		lightSpheresMesh[0]->sendData(renderer->getDeviceContext());
		debugShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"wood"));
		debugShader->render(renderer->getDeviceContext(), lightSpheresMesh[0]->getIndexCount());
	}

	XMMATRIX worldMatrix = XMMatrixTranslation(lights[2]->getPosition().x, lights[2]->getPosition().y, lights[2]->getPosition().z);
	//XMMATRIX scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	lightSpheresMesh[0]->sendData(renderer->getDeviceContext());
	debugShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"wood"));
	debugShader->render(renderer->getDeviceContext(), lightSpheresMesh[0]->getIndexCount());


}

void App1::setupWave()
{
	for (int i = 0; i < 2; i++)
	{
		wavesData.direction[i] = XMFLOAT4(1.0f, 0, 1.0f, 0.0f);
		//x - time, y - wave length, z - steepness, w - padding
		wavesData.parameters[i] = XMFLOAT4(0.0f, 20.0f, 0.5f, 0.0f);
	}
}

void App1::updateWaveTime()
{
	for (int i = 0; i < 2; i++)
	{
		wavesData.parameters[i].x += timer->getTime();
		wavesData.parameters[i].y = wavesData.guiPa[i][0];
		wavesData.parameters[i].z = wavesData.guiPa[i][1];
		wavesData.direction[i].x = wavesData.guiDir[i][0];
		wavesData.direction[i].z = wavesData.guiDir[i][1];
	}
}

void App1::lightGUI()
{
	if (ImGui::TreeNode("Directional Light"))
	{
		ImGui::SliderFloat3("Position ", positionDir, -100.0f, 200.0f, "%.2f");
		lights[0]->setPosition(positionDir[0], positionDir[1], positionDir[2]);

		ImGui::SliderFloat3("Direction ", directionDir, -1.0f, 1.0f, "%.2f");
		lights[0]->setDirection(directionDir[0], directionDir[1], directionDir[2]);

		ImGui::TreePop();
	}
	
	if (ImGui::TreeNode("Lamp 1"))
	{
		float colour[3];
		colour[0] = lights[1]->getDiffuseColour().x;
		colour[1] = lights[1]->getDiffuseColour().y;
		colour[2] = lights[1]->getDiffuseColour().z;

		ImGui::SliderFloat3("1 Colour", colour, 0.0f, 1.0f, "%.3f");
		lights[1]->setDiffuseColour(colour[0], colour[1], colour[2], 1.0f);

		float a[3];
		a[0] = lamp->getTranslation().x;
		a[1] = lamp->getTranslation().y;
		a[2] = lamp->getTranslation().z;
		ImGui::SliderFloat3("1 Position", a, -100.0f, 100.0f, "%.2f");
		lamp->setTranslation(XMFLOAT3(a[0], a[1], a[2]));

		float b[3];
		b[0] = lamp->getAttenuation().x;
		b[1] = lamp->getAttenuation().y;
		b[2] = lamp->getAttenuation().z;
		ImGui::SliderFloat3("1 Attenuation", b, 0.0001f, 1.0f, "%.2f");
		lamp->setAttenuation(XMFLOAT4(b[0], b[1], b[2], 1.0f));

		ImGui::TreePop();
	}

	
	if (ImGui::TreeNode("Lamp 2"))
	{
		float colour[3];
		colour[0] = lights[2]->getDiffuseColour().x;
		colour[1] = lights[2]->getDiffuseColour().y;
		colour[2] = lights[2]->getDiffuseColour().z;

		ImGui::SliderFloat3("2 Colour", colour, 0.0f, 1.0f, "%.3f");
		lights[2]->setDiffuseColour(colour[0], colour[1], colour[2], 1.0f);


		float a[3];
		a[0] = lamp1->getTranslation().x;
		a[1] = lamp1->getTranslation().y;
		a[2] = lamp1->getTranslation().z;

		ImGui::SliderFloat3("2 Position", a, -100.0f, 100.0f, "%.2f");
		lamp1->setTranslation(XMFLOAT3(a[0], a[1], a[2]));

		float b[3];
		b[0] = lamp1->getAttenuation().x;
		b[1] = lamp1->getAttenuation().y;
		b[2] = lamp1->getAttenuation().z;
		ImGui::SliderFloat3("2 Attenuation", b, 0.0001f, 1.0f, "%.2f");
		lamp1->setAttenuation(XMFLOAT4(b[0], b[1], b[2], 1.0f));

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Lamp 3"))
	{
		float colour[3];
		colour[0] = lights[3]->getDiffuseColour().x;
		colour[1] = lights[3]->getDiffuseColour().y;
		colour[2] = lights[3]->getDiffuseColour().z;
		ImGui::SliderFloat3("3 Colour", colour, 0.0f, 1.0f, "%.3f");
		lights[3]->setDiffuseColour(colour[0], colour[1], colour[2], 1.0f);

		float a[3];
		a[0] = lamp2->getTranslation().x;
		a[1] = lamp2->getTranslation().y;
		a[2] = lamp2->getTranslation().z;
		ImGui::SliderFloat3("3 Position", a, -100.0f, 100.0f, "%.2f");
		lamp2->setTranslation(XMFLOAT3(a[0], a[1], a[2]));

		float b[3];
		b[0] = lamp2->getAttenuation().x;
		b[1] = lamp2->getAttenuation().y;
		b[2] = lamp2->getAttenuation().z;
		ImGui::SliderFloat3("3 Attenuation", b, 0.0001f, 1.0f, "%.2f");
		lamp2->setAttenuation(XMFLOAT4(b[0], b[1], b[2], 1.0f));

		ImGui::TreePop();
	}
}

void App1::waveGUI()
{
	if (ImGui::CollapsingHeader("Ocean"))
	{
		
		ImGui::Checkbox("Wave Shadows", &waveShadowsActive);
		ImGui::SliderFloat3("Position Ocean ", wavesData.translation, -100.0f, 100.0f, "%.0f");

		if (ImGui::TreeNode("Wave 1"))
		{
			ImGui::SliderFloat("Wave 1 Steepness", &wavesData.guiPa[0][1], 0, 1, "%.2f");
			ImGui::SliderFloat("Wave 1 Length", &wavesData.guiPa[0][0], 0, 50, "%.2f");
			ImGui::SliderFloat2("Wave 1 Direction(X,Z)", wavesData.guiDir[0], -1, 1);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Wave 2"))
		{
			ImGui::SliderFloat("Wave 2 Steepness", &wavesData.guiPa[1][1], 0, 1, "%.2f");
			ImGui::SliderFloat("Wave 2 Length", &wavesData.guiPa[1][0], 0, 50, "%.2f");
			ImGui::SliderFloat2("Wave 2 Direction(X,Z)", wavesData.guiDir[1], -1, 1);
			ImGui::TreePop();
		}

		//ImGui::TreePop();

	}

}

void App1::renderNoPostProcess()
{
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// combine shadow maps created in the depth pass
	ShadowMap* multipleMaps[19];
	combineShadowMaps(multipleMaps);
	setupLightAttenuation();

	// render lamps
	renderLamp(lamp, viewMatrix, projectionMatrix, multipleMaps);
	renderLamp(lamp1, viewMatrix, projectionMatrix, multipleMaps);
	renderLamp(lamp2, viewMatrix, projectionMatrix, multipleMaps);

	renderHouse(viewMatrix, projectionMatrix, multipleMaps);
	//render floor
	worldMatrix = terrainTranslation;
	tessellatedMesh->sendData(renderer->getDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST);
	XMFLOAT4 cameraPos = XMFLOAT4(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z, 0.f);
	heightMapShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		edgeTessellationFactor, interiorTessellationFactor, dynamicTessellation, cameraPos, textureMgr->getTexture(L"heightMap"), textureMgr->getTexture(L"terrain"), multipleMaps, lights, lightAttenuation);
	heightMapShader->render(renderer->getDeviceContext(), tessellatedMesh->getIndexCount());

	// render ocean
	renderWaves(viewMatrix, projectionMatrix, multipleMaps);

}

void App1::combineShadowMaps(ShadowMap* multipleMaps[19])
{
	// combine shadow maps created in the depth pass
	multipleMaps[0] = shadowMapDirLight;
	for (int i = 1; i < 7; i++)
	{
		multipleMaps[i] = lamp->shadowMaps[i - 1];
	}

	for (int i = 7; i < 13; i++)
	{
		multipleMaps[i] = lamp1->shadowMaps[i - 7];
	}

	for (int i = 13; i < 19; i++)
	{
		multipleMaps[i] = lamp2->shadowMaps[i - 13];
	}
}

void App1::renderWaves(XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ShadowMap* maps[19])
{
	XMMATRIX worldMatrix = XMMatrixTranslation(wavesData.translation[0], wavesData.translation[1], wavesData.translation[2]);

	waveMesh->sendData(renderer->getDeviceContext());
	gerstnerWaveShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"water"),maps, lights, lightAttenuation, wavesData.parameters, wavesData.direction);
	gerstnerWaveShader->render(renderer->getDeviceContext(),waveMesh->getIndexCount());
}

void App1::renderLamp(Lamp* lamp, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ShadowMap* maps [19])
{
	XMMATRIX worldMatrix, scaleMatrix;
	XMFLOAT3 s = lamp->getScale();
	worldMatrix = lamp->getTranslationMatrix();
	scaleMatrix = XMMatrixScaling(s.x, s.y, s.z);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
;
	lampModel->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"wood"), maps, lights, lightAttenuation);
	shadowShader->render(renderer->getDeviceContext(), lampModel->getIndexCount());

}

void App1::renderHouse(XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ShadowMap* maps[19])
{
	XMMATRIX worldMatrix = houseTransaltion;
	XMMATRIX rotMatrix = XMMatrixRotationY(1.57);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotMatrix);
	houseModel->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"wood"), maps, lights, lightAttenuation);
	shadowShader->render(renderer->getDeviceContext(), lampModel->getIndexCount());
}

void App1::renderLampToDepthMap(Lamp* l, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	// render lamp to the depth map
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMFLOAT3 lampScale = l->getScale();
	worldMatrix = l->getTranslationMatrix();
	XMMATRIX scaleMatrix = XMMatrixScaling(lampScale.x, lampScale.y, lampScale.z);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	lampModel->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	depthShader->render(renderer->getDeviceContext(), lampModel->getIndexCount());
}

void App1::renderHouseToDepthMap(XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	XMMATRIX worldMatrix = houseTransaltion;
	XMMATRIX rotMatrix = XMMatrixRotationY(3.14 / 2);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotMatrix);
	houseModel->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	depthShader->render(renderer->getDeviceContext(), houseModel->getIndexCount());
}



void App1::cleanupTextures()
{
	if (renderTexture)
	{
		delete renderTexture;
		renderTexture = 0;
	}
	if (bloomMapTexture)
	{
		delete bloomMapTexture;
		bloomMapTexture = 0;
	}
	if (bloomMapTextureDown)
	{
		delete bloomMapTextureDown;
		bloomMapTextureDown = 0;
	}

	if (horizontalBlurTexture)
	{
		delete horizontalBlurTexture;
		horizontalBlurTexture = 0;
	}
	if (horizontalBlurTextureDown)
	{
		delete horizontalBlurTextureDown;
		horizontalBlurTextureDown = 0;
	}
	if (verticalBlurTexture)
	{
		delete verticalBlurTexture;
		verticalBlurTexture = 0;
	}
	if (halfResVertBlurTextureDown)
	{
		delete halfResVertBlurTextureDown;
		halfResVertBlurTextureDown = 0;
	}
	if (halfResVertBlurTextureUP)
	{
		delete halfResVertBlurTextureUP;
		halfResVertBlurTextureUP = 0;
	}
	if (quarterResBlurComplete)
	{
		delete quarterResBlurComplete;
		quarterResBlurComplete = 0;
	}
	if (quarterResVertBlurTextureDown)
	{
		delete quarterResVertBlurTextureDown;
		quarterResVertBlurTextureDown = 0;
	}
	if (fullBlurTexture)
	{
		delete fullBlurTexture;
		fullBlurTexture = 0;
	}
	if (sceneTexture)
	{
		delete sceneTexture;
		sceneTexture = 0;
	}

}

void App1::cleanupLights()
{
	for (int i = 0; i < 4; i++)
	{
		if (lights[i])
		{
			delete lights[i];
			lights[i] = 0;
		}
	}

	if (lamp)
	{
		delete lamp;
		lamp = 0;
	}
	if (lamp1)
	{
		delete lamp1;
		lamp1 = 0;
	}
	if (lamp2)
	{
		delete lamp2;
		lamp2 = 0;
	}

}

void App1::cleanupShaders()
{
	if (heightMapShader)
	{
		delete heightMapShader;
		heightMapShader = 0;
	}
	if (depthHeightMapShader)
	{
		delete depthHeightMapShader;
		depthHeightMapShader = 0;
	}
	if (gerstnerWaveShader)
	{
		delete gerstnerWaveShader;
		gerstnerWaveShader = 0;
	}
	if (gerstnerWaveDepthShader)
	{
		delete gerstnerWaveDepthShader;
		gerstnerWaveDepthShader = 0;
	}
	if (depthShader)
	{
		delete depthShader;
		depthShader = 0;
	}
	if (shadowShader)
	{
		delete shadowShader;
		shadowShader = 0;
	}
	if (debugShader)
	{
		delete debugShader;
		debugShader = 0;
	}
	if (bloomShader)
	{
		delete bloomShader;
		bloomShader = 0;
	}
	if (blendingShader)
	{
		delete blendingShader;
		blendingShader = 0;
	}
	if (horizontalBlurShader)
	{
		delete horizontalBlurShader;
		horizontalBlurShader = 0;
	}
	if (verticalBlurShader)
	{
		delete verticalBlurShader;
		verticalBlurShader = 0;
	}
}

void App1::cleanupMeshes()
{
	if (tessellatedMesh)
	{
		delete tessellatedMesh;
		tessellatedMesh = 0;
	}
	if (waveMesh)
	{
		delete waveMesh;
		waveMesh = 0;
	}
	if (lampModel)
	{
		delete lampModel;
		lampModel = 0;
	}
	if (houseModel)
	{
		delete houseModel;
		houseModel = 0;
	}

	if (fullScreenOrthoMesh)
	{
		delete fullScreenOrthoMesh;
		fullScreenOrthoMesh = 0;
	}
	if (halfScreenMesh)
	{
		delete halfScreenMesh;
		halfScreenMesh = 0;
	}
	if (quarterScreenMesh)
	{
		delete quarterScreenMesh;
		quarterScreenMesh = 0;
	}
}
