#pragma once
#include "engine/GameObject.h"
//#include "networking\NetworkManager.h"

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

