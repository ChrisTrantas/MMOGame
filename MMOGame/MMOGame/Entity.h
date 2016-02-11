#pragma once

#include <DirectXMath.h>

#include "Mesh.h"

using namespace DirectX;

class Entity
{
	public:
		Entity(Mesh* m);
		~Entity();
		XMFLOAT4X4 worldMatrix;
		XMFLOAT3 position;
		XMFLOAT3 rotation;
		XMFLOAT3 scale;
		Mesh* mesh;
		XMFLOAT3 getPosition();
		void setPosition(XMFLOAT3 pos);
		XMFLOAT3 getRotation();
		void setRotation(XMFLOAT3 rot);
		XMFLOAT3 getScale();
		void setScale(XMFLOAT3 scl);
};

