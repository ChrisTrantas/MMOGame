#include "Octree.h"

Octree::Octree()
{
	center = vec3(0, 0, 0);
	halfSize = vec3(MAX_OCTREE_SIZE, MAX_OCTREE_SIZE, MAX_OCTREE_SIZE);
	isLeaf = true;
	depth = 0;
}

Octree::Octree(vec3 pos, vec3 halfSize, size_t depth)
{
	center = pos;
	this->halfSize = halfSize;
	isLeaf = true;
	this->depth = depth;
}

Octree::~Octree()
{
	if (!isLeaf)
		for (size_t i = 0; i < 8; ++i)
			delete octants[i];
}

Collider* Octree::operator[](int i)
{
	if (i >= 0 && (size_t)i < colliders.size())
		return colliders[i];
	return NULL;
}

size_t Octree::binCount()
{
	return colliders.size();
}

vector<Octree*> Octree::addCollider(Collider* collider)
{
	vector<Octree*> bins;
	addCollider(collider, bins);
	return bins;
}

void Octree::addCollider(Collider* collider, vector<Octree*> &bins)
{
	if (depth < MAX_OCTREE_DEPTH && colliders.size() == OCTREE_SPLIT_THRESHOLD)
	{
		for (size_t i = 0; i < 8; ++i)
		{
			vec3 centerOffset = vec3(i & 1 ? -0.5 : 0.5, i < 4 ? -0.5 : 0.5, i>1 && i < 6 ? 0.5 : -0.5);
			octants[i] = new Octree(center + halfSize * centerOffset, halfSize / 2.0f, depth + 1);
		}

		isLeaf = false;
		for (size_t i = 0; i < colliders.size(); ++i)
			colliders[i]->reTree();
	}

	if (isLeaf)
	{
		colliders.push_back(collider);
		bins.push_back(this);
	}
	else
		for (size_t i = 0; i < 8; ++i)
			if (satCheck(collider, octants[i]))
				octants[i]->addCollider(collider, bins);
}

bool Octree::satCheck(Collider* collider, Octree* octant)
{
	boundingVectors OBB = collider->gameObject->getComponent<MeshRenderer>()->mesh->bounds;
	Transform* t = collider->gameObject->getComponent<Transform>();

	vec3 axes[15];
	axes[0] = t->right();
	axes[1] = t->up();
	axes[2] = t->forward();

	axes[3] = vec3(1, 0, 0);
	axes[4] = vec3(0, 1, 0);
	axes[5] = vec3(0, 0, -1);
	for (size_t i = 0; i < 9; ++i)
		axes[6 + i] = cross(axes[i / 3], axes[i % 3 + 3]);

	vec3 tests[7];
	tests[0] = octant->center - vec3(t->renderMatrix(false) * vec4(OBB.center, 1));
	for (size_t i = 0; i < 6; ++i)
		tests[i + 1] = axes[i] * (i < 3 ? (OBB.halfSize[i % 3] * t->scale[i % 3]) : octant->halfSize[i % 3]);

	for (size_t i = 0; i < 15; ++i)
	{
		float abDot = 0.0f;
		for (size_t j = 1; j < 7; j += 1)
			abDot += abs(dot(tests[j], axes[i]));
		if (abs(dot(tests[0], axes[i])) > abDot)
			return false;
	}

	return true;
}

void Octree::removeCollider(Collider* collider)
{
	auto index = find(colliders.begin(), colliders.end(), collider);
	colliders.erase(colliders.begin() + (index - colliders.begin()));
}
