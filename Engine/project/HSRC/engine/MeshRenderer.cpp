#include "MeshRenderer.h"
#include "Camera.h"
#include "Game.h"

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

	float explodedModel[16];
	float explodedCamera[16];
	float explodedPerspective[16];

	explodeMat4(transpose(transform->renderMatrix(true)), explodedModel);
	explodeMat4(transpose(camera->renderMatrix()), explodedCamera);
	explodeMat4(transpose(camera->perspectiveMatrix()), explodedPerspective);

	material->setAttribute(VERTEX_BIT, "world", explodedModel, sizeof(float) * 16, false);
	material->setAttribute(VERTEX_BIT, "view", explodedCamera, sizeof(float) * 16, false);
	material->setAttribute(VERTEX_BIT, "projection", explodedPerspective, sizeof(float) * 16, false);

	SpotLight light = DEFAULT_LIGHT->getSpotLight();
	material->setAttribute(VERTEX_BIT | PIXEL_BIT, "spotLight", &light, sizeof(SpotLight), false);

	material->apply();

	mesh->draw();

	material->remove();
}
