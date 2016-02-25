#pragma once
#include "engine/GameObject.h"

class FPController : public IComponent
{
public:
	FPController();
	~FPController();
	void awake();
	void update();
private:
	Transform* ct;
};

