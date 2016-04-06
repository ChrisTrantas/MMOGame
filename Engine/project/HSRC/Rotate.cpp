#include "Rotate.h"

Rotate::Rotate(float speed, vec3 axis)
{
	this->speed = speed;
	this->axis = axis;
}

Rotate::~Rotate()
{

}

void Rotate::awake()
{
	t = gameObject->getComponent<Transform>();
	
	Input::bindToControl("randomize", 'R');
}

void Rotate::update(float deltaTime, float totalTime)
{
	if (Input::wasControlPressed("randomize"))
		axis = RAND_VEC3;

	t->rotate(angleAxis(speed * deltaTime, axis));
}

void Rotate::setSpeed(float speed)
{
	this->speed = speed;
}

void Rotate::setAxis(vec3 axis)
{
	if (length(axis) > 0)
		this->axis = normalize(axis);
}
