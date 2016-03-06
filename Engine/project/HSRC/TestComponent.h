#pragma once
#include "engine/GameObject.h"

class TestComponent : public IComponent
{
public:
	TestComponent(float speed);
	~TestComponent();
	void awake();
	void update(float deltaTime, float totalTime);
private:
	float angle, speed;
	Transform* t;
};

