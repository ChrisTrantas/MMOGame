#pragma once
#include "networking\NetworkManager.h"
#include "engine/GameObject.h"


class FPController : public IComponent
{
public:
	FPController();
	~FPController();
	void awake();
	void update(float deltaTime, float totalTime);
private:
	Transform* ct;
	float runSpeed;
	float walkSpeed;
};

