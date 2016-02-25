#pragma once
#include "Collider.h"

#define MAX_OCTREE_DEPTH 16
#define OCTREE_SPLIT_THRESHOLD 6
#define MAX_OCTREE_SIZE 64

class Collider;

class Octree
{
public:
	Octree();
	~Octree();
	Collider* operator[](int i);
	size_t binCount();
	vector<Octree*> addCollider(Collider* collider);
	void removeCollider(Collider* collider);
private:
	Octree(vec3 pos, vec3 halfSize, size_t depth);
	void addCollider(Collider* collider, vector<Octree*> &bins);
	bool satCheck(Collider* collider, Octree* octant);
	vector<Collider*> colliders;
	Octree* octants[8];
	vec3 center;
	vec3 halfSize;
	bool isLeaf;
	size_t depth;
};
