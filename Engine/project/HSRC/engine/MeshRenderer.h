#pragma once

#include "IComponent.h"
#include "Mesh.h"
#include "Material.h"

class Camera;

class MeshRenderer : public IComponent
{
public:
	MeshRenderer();
	~MeshRenderer();
	void draw(Camera* camera, Transform* transform);
	Mesh* mesh;
	Material* material;
	bool visible;
};
