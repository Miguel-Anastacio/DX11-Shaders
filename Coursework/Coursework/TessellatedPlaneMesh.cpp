#include "TessellatedPlaneMesh.h"

TessellatedPlaneMesh::TessellatedPlaneMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution)
{
	resolution = lresolution;
	initBuffers(device);
}

TessellatedPlaneMesh::~TessellatedPlaneMesh()
{
	BaseMesh::~BaseMesh();
}

void TessellatedPlaneMesh::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;

	std::vector<unsigned long>* indicesVector;
	unsigned long* indices;
	float positionX, positionZ, u, v, increment;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	// for a grid RESxRES of quads we require 4 * RES * RES vertices
	vertexCount = 4 * (resolution) * (resolution);
	// for each quad(4 verts) we have to pass 12 control points
	// the 4 that make the quad and 8 for the neighboring quads
	int controlPointsCount = ((resolution) * (resolution)) * 12;
	indexCount = controlPointsCount;
	vertices = new VertexType[vertexCount];
	indicesVector = new std::vector<unsigned long>;
	indices = new unsigned long[indexCount];
	// UV coords.
	int vertIndex = 0;
	int index = 0;
	u = 0;
	v = 0;
	float incrementV = 1.0f / resolution;
	float incrementU = 1.0f / resolution;
	// build plane vertices
	for (int j = 0; j < resolution; j++)
	{
		for (int i = 0; i < resolution ; i++)
		{
			float x = i;
			float z = j;
			// bottom left
			vertices[index].position = XMFLOAT3(x, 0, z);
			vertices[index].texture = XMFLOAT2(u, v);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indicesVector->push_back(index);
			index++;
			// top left
			vertices[index].position = XMFLOAT3(x, 0, z +1);
			vertices[index].texture = XMFLOAT2(u, v+incrementV);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indicesVector->push_back(index);	
			index++;

			// top right
			vertices[index].position = XMFLOAT3(x + 1, 0, z+1);
			vertices[index].texture = XMFLOAT2(u + incrementU, v+incrementV);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indicesVector->push_back(index);
			index++;

			// bottom right
			vertices[index].position = XMFLOAT3(x+1, 0, z);
			vertices[index].texture = XMFLOAT2(u + incrementU, v);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indicesVector->push_back(index);
			index++;
			
			
			// 8 control points  for the neighboring quads
			
			int currQuadIndex = x * 4 + z * resolution * 4;
			//+ x
			int bottomLeftVert = getIndexOfQuad(x + 1, z, currQuadIndex);

			
			// add offset to bottom left vert ( +1- top left , +2 - top right, +3 - bottom right
			// to get index of bottom right vert of the quad to the right
			int vertIndex = bottomLeftVert + 3;
			indicesVector->push_back(vertIndex);
			// index of top right vert of quad to the right
			vertIndex = bottomLeftVert + 2;
			indicesVector->push_back(vertIndex);


			//+ z
			bottomLeftVert = getIndexOfQuad(x, z + 1, currQuadIndex);
			//index of top left vert of the quad 'above'
			vertIndex = bottomLeftVert + 1;
			indicesVector->push_back(vertIndex);
			// index of top right vert of quad above
			vertIndex = bottomLeftVert + 2;
			indicesVector->push_back(vertIndex);

			//-x
			bottomLeftVert = getIndexOfQuad(x - 1, z, currQuadIndex);
			// index of bottom left vert of the quad to the left
			vertIndex = bottomLeftVert + 0;
			indicesVector->push_back(vertIndex);
			// index of top left vert of quad to the left
			vertIndex = bottomLeftVert + 1;
			indicesVector->push_back(vertIndex);

			//-z
			bottomLeftVert = getIndexOfQuad(x, z - 1, currQuadIndex);
			//index of bottom left vert of quad below
			vertIndex = bottomLeftVert;
			indicesVector->push_back(vertIndex);
			//index of bottom right vert of quad below
			vertIndex = bottomLeftVert + 3;
			indicesVector->push_back(vertIndex);
			
			u += incrementU;
			
		}
		u = 0;
		v += incrementV;

	}
	int temp = 0;
	// copy the std::vector to an array since the plane is not rendered if we use th estd::vector directly
	for (int i = 0; i < indicesVector->size(); i++)
	{
		indices[i] = (*indicesVector)[i];
		//XMFLOAT3 a = vertices[i].position;
	}

	unsigned long* a = indices;
	//VertexType* v = vertices;
	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);


	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
	delete indicesVector;
	indicesVector = 0;
}

void TessellatedPlaneMesh::clamp(int& value, int min, int max)
{
	if (value < min)
		value = min;
	if (value > max)
		value = max;
}

int TessellatedPlaneMesh::getIndexOfQuad(int x, int z, int currQuadIndex)
{
	// quad requested does not exist
	if (z >= resolution || z < 0 || x >= resolution || x < 0)
	{
		return currQuadIndex;
	}
	return  x * 4 + z * resolution * 4;
	
}



void TessellatedPlaneMesh::sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Set the type of primitive that should be rendered from this vertex buffer, in this case control patch for tessellation.
	deviceContext->IASetPrimitiveTopology(top);
}