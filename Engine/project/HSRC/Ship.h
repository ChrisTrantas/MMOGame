#pragma once
#include "networking\NetworkManager.h"
#include "engine/GameObject.h"

class Ship : public IComponent
{
public:
	Ship();
	~Ship();
	void awake();
	void update(float deltaTime, float totalTime);
	void draw();
private:
	Transform* t;
	MeshRenderer* mr;
	vec3 perlinSeed;
};

