#pragma once
#include "engine/GameObject.h"

class TestComponent : public IComponent
{
public:
	TestComponent();
	~TestComponent();
	void awake();
	void update(float deltaTime, float totalTime);
private:
	float angle;
	Transform* t;
};

