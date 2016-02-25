#include "Camera.h"
#include "Game.h"

Camera* Camera::getCamera(string name)
{
	Camera* camera = (Camera*)ResourceManager::manager->getResource("GameObject Camera " + name);
	if (!camera)
		camera = new Camera(name);
	return camera;
}

Camera::Camera(string name) : GameObject("Camera " + name)
{
	meshRenderer->visible = false;
	fov = 0.25f * (float)M_2_PI;
	nearPlane = 0.1f;
	farPlane = 256.0f;
}

Camera::~Camera()
{
	
}

mat4 Camera::renderMatrix()
{
	return lookAt(transform->position, transform->position + transform->forward(), transform->up());
}

mat4 Camera::perspectiveMatrix()
{
	return perspective(fov, Game::game->getAspectRatio(), nearPlane, farPlane);
}
