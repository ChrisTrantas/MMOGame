#include "MeshRenderer.h"
#include "Camera.h"

MeshRenderer::MeshRenderer()
{
	mesh = DEFAULT_MESH;
	material = DEFAULT_MATERIAL;
	visible = true;
}

MeshRenderer::~MeshRenderer()
{

}

void MeshRenderer::draw(Camera* camera, Transform* transform)
{
	if (!visible)
		return;
	mesh->draw(camera->renderMatrix(), camera->perspectiveMatrix(), transform->renderMatrix(true), material);
}
