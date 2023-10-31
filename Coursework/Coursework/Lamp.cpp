#include "Lamp.h"

Lamp::Lamp(ID3D11Device* device, int mWidth, int mHeight, Light* light)
{
	initShadowMaps(device, mWidth, mHeight);
	pointLight = light;
	createLightView();
	scale = XMFLOAT3(1.0f, 2.0f, 1.0f);
	attenuation = XMFLOAT4(0.0, 0.1, 0.01, 1.0);
}

Lamp::~Lamp()
{
	if (pointLight)
	{
		delete pointLight;
		pointLight = 0;
	}
	for (int i = 0; i < 6; i++)
	{
		if (shadowMaps[i])
		{
			delete shadowMaps[i];
			shadowMaps[i] = 0;
		}
	}

}


void Lamp::setTranslation(XMFLOAT3 t)
{
	translation = t;
	pointLight->setPosition(t.x, t.y + 5.5, t.z);
	tMatrix = XMMatrixTranslation(translation.x * (1/scale.x), translation.y * (1 / scale.x), translation.z * (1 / scale.x));
}

void Lamp::initShadowMaps(ID3D11Device* device, int mWidth, int mHeight)
{
	for (int i = 0; i < 6; i++)
	{
		shadowMaps[i] = new ShadowMap(device, mWidth, mHeight);
	}


}

void Lamp::createLightView()
{
	XMVECTOR up, lookAt, position;
	position = XMLoadFloat3(&pointLight->getPosition());
	// default up vector
	up = XMVectorSet(0.0f, 1.0, 0.0, 1.0f);

	// code taken from the generate view matrix fucntion from the light class
	for (int i = 0; i < 6; i++)
	{
		// deal with cases where we have to change the vector up 
		if (cubeFaceNormal[i].y == 1 || (cubeFaceNormal[i].x == 0 && cubeFaceNormal[i].z == 0))
			up = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
		else if (cubeFaceNormal[i].y == -1 || (cubeFaceNormal[i].x == 0 && cubeFaceNormal[i].z == 0))
		{
			up = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f);
		}

		XMVECTOR dir = XMVectorSet(cubeFaceNormal[i].x, cubeFaceNormal[i].y, cubeFaceNormal[i].z, 1.0f);
		XMVECTOR right = XMVector3Cross(dir, up);

		up = XMVector3Cross(right, dir);

		lookAt = XMVectorSet(pointLight->getPosition().x + cubeFaceNormal[i].x, pointLight->getPosition().y + cubeFaceNormal[i].y, pointLight->getPosition().z + cubeFaceNormal[i].z, 1.0f);
		// Create the view matrix from the three vectors.
		lightView[i] = XMMatrixLookAtLH(position, lookAt, up);
		// reset up to  the default
		up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	}
}
