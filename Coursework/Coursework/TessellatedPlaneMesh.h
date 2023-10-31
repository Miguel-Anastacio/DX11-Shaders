#pragma once
#include "DXF.h"

class TessellatedPlaneMesh : public BaseMesh
{
public:
	TessellatedPlaneMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution);
	~TessellatedPlaneMesh();
	void sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top);
private:

	void initBuffers(ID3D11Device* device);
	int resolution;

	void clamp(int& value, int min, int max);
	// returns the index of the bottom left vert of the specified quad
	// if quad does not exist - returns the current quad
	int getIndexOfQuad(int x, int z, int currQuadIndex);

};

