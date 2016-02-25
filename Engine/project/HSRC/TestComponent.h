#pragma once
#include "engine/GameObject.h"

class TestComponent : public IComponent
{
public:
	TestComponent();
	~TestComponent();
	void awake();
	void update();
private:
	float angle;
	Transform* t;
};

