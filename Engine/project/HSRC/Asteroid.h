#pragma once
#include "engine/GameObject.h"

class Asteroid : public IComponent
{
public:
	Asteroid();
	~Asteroid();
	void awake();
	void update(float deltaTime, float totalTime);
	void draw();
private:
	Transform* t;
	MeshRenderer* mr;
	vec3 perlinSeed;
};

