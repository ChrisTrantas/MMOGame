#pragma once
#include "GameObject.h"
#include "Octree.h"

class Octree;
class Collider;

struct Collision
{
	Collider* collider;
	vec3 normal;
	float depth;
};

class Collider : public IComponent
{
public:
	Collider();
	~Collider();
	void awake();
	void earlyUpdate(float deltaTime, float totalTime);
	vector<Collision> checkForCollisions();
	vector<Collision> checkForCollisions(bool completeMatch, size_t numTag, ...);
	Collision checkCollision(Collider* other);
	Collision checkCollision(GameObject* other);
	void reTree();
private:
	MeshRenderer* mr;
	Transform* t;
	vector<Octree*> bins;
	vec3 oldPos, oldScale;
	quat oldRot;
	map<Collider*, bool> octreeFilter;
};

