#pragma once
#include "networking\NetworkManager.h"
#include "engine/GameObject.h"

class Ship : public IComponent
{
public:
	Ship();
	~Ship();
	void awake();
	void update();
	void move(vec2 moveDir);
	void draw();	
private:
	Transform* t;
	//MeshRenderer* mr;
	//vec3 perlinSeed;
};

