#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "Vertex.h"
#include "DirectXGameCore.h"

class Mesh
{
	public:
		Mesh(Vertex* verts, int numVerts, UINT* indices, int numIndex, ID3D11Device* bufferDevice);
		~Mesh();
		ID3D11Buffer* GetVertexBuffer();
		ID3D11Buffer* GetIndexBuffer();
		int GetIndexCount();
		void DrawMesh(ID3D11DeviceContext* deviceContext);
	private:
		ID3D11Buffer* vertBuffer;
		ID3D11Buffer* indexBuffer;
		int numIndicies;
};

