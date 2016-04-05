#pragma once
#include "engine/GameObject.h"

class Asteroid : public IComponent
{
public:
	Asteroid(float speed, SimpleVertexShader* vs, SimpleVertexShader* avs);
	~Asteroid();
	void awake();
	void earlyUpdate(float deltaTime, float totalTime);
	void update(float deltaTime, float totalTime);
	void draw();
private:
	SimpleVertexShader* vs, *avs;
	float speed;
	Transform* t;
	MeshRenderer* mr;
	vec3 perlinSeed;
};

