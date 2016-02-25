#pragma once

#include "GameObject.h"

#define DEFAULT_CAMERA Camera::getCamera("DEFAULT")

class Camera : public GameObject
{
public:
	static Camera* getCamera(string name);
	~Camera();
	mat4 renderMatrix();
	mat4 perspectiveMatrix();
	void draw() {};
private:
	Camera(string name);
	float nearPlane;
	float farPlane;
	float fov;
};
