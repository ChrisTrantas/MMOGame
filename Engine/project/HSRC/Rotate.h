#pragma once
#include "engine/GameObject.h"

class Rotate : public IComponent
{
public:
	Rotate(float speed, vec3 axis);
	~Rotate();
	void awake();
	void update(float deltaTime, float totalTime);
	void setSpeed(float speed);
	void setAxis(vec3 axis);
private:
	float speed;
	vec3 axis;
	Transform* t;
};

