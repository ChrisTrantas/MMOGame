#include "Collider.h"

static Octree tree;

Collider::Collider()
{

}

Collider::~Collider()
{
	while (bins.size() > 0)
	{
		bins.back()->removeCollider(this);
		bins.pop_back();
	}
}

void Collider::awake()
{
	mr = gameObject->getComponent<MeshRenderer>();
	t = gameObject->getComponent<Transform>();
	bins = tree.addCollider(this);
	oldPos = t->position;
	oldRot = t->rotation;
	oldScale = t->scale;
}

void Collider::earlyUpdate(float deltaTime, float totalTime)
{
	if (oldPos != t->position || oldRot != t->rotation || oldScale != t->scale)
		reTree();
	oldPos = t->position;
	oldRot = t->rotation;
	oldScale = t->scale;
}

vector<Collision> Collider::checkForCollisions()
{
	vector<Collision> collisions;
	octreeFilter.clear();

	for each (Octree* b in bins)
		for (size_t i = 0; i < b->binCount(); i += 1)
		{
			if (octreeFilter[(*b)[i]])
				continue;
			octreeFilter[(*b)[i]] = true;

			Collision c = checkCollision((*b)[i]);

			if (c.collider != NULL)
				collisions.push_back(c);
		}

	return collisions;
}

vector<Collision> Collider::checkForCollisions(bool completeMatch, size_t numTag, ...)
{
	if (numTag == 0)
		return checkForCollisions();

	vector<Collision> collisions;
	octreeFilter.clear();
	vector<string> filter = vector<string>(numTag);
	va_list args;
	va_start(args, numTag);

	for (size_t i = 0; i < numTag; ++i)
		filter[i] = va_arg(args, char*);

	for each (Octree* b in bins)
		for (size_t i = 0; i < b->binCount(); i += 1)
		{
			if (octreeFilter[(*b)[i]])
				continue;
			octreeFilter[(*b)[i]] = true;

			bool passesFilter = false;

			for (size_t j = 0; j < numTag; ++j)
			{
				bool tagCheck = (*b)[i]->gameObject->hasTag(filter[j]);
				if (tagCheck && !completeMatch)
				{
					passesFilter = true;
					break;
				}

				if (completeMatch)
				{
					if (!tagCheck)
						break;
					if (j == numTag - 1)
						passesFilter = true;
				}
			}

			if (!passesFilter)
				continue;

			Collision c = checkCollision((*b)[i]);

			if (c.collider != NULL)
				collisions.push_back(c);
		}

	va_end(args);
	return collisions;
}

static bool sphereCheck(Collider* a, Collider* b)
{
	boundingVectors aOBB = a->gameObject->getComponent<MeshRenderer>()->mesh->bounds;
	boundingVectors bOBB = b->gameObject->getComponent<MeshRenderer>()->mesh->bounds;
	Transform* aT = a->gameObject->getComponent<Transform>();
	Transform* bT = b->gameObject->getComponent<Transform>();

	aOBB.sphere = vec3(aT->renderMatrix(false) * vec4(aOBB.sphere, 1));
	bOBB.sphere = vec3(bT->renderMatrix(false) * vec4(bOBB.sphere, 1));

	float rr = aOBB.radius + bOBB.radius;
	float xx = aOBB.sphere.x - bOBB.sphere.x;
	float yy = aOBB.sphere.y - bOBB.sphere.y;
	float zz = aOBB.sphere.z - bOBB.sphere.z;

	return rr*rr >= xx*xx + yy*yy + zz*zz;
}

static void satCheck(Collider* a, Collider* b, Collision &result)
{
	boundingVectors aOBB = a->gameObject->getComponent<MeshRenderer>()->mesh->bounds;
	boundingVectors bOBB = b->gameObject->getComponent<MeshRenderer>()->mesh->bounds;
	Transform* aT = a->gameObject->getComponent<Transform>();
	Transform* bT = b->gameObject->getComponent<Transform>();

	vec3 axes[15];
	axes[0] = aT->right();
	axes[1] = aT->up();
	axes[2] = aT->forward();
	axes[3] = bT->right();
	axes[4] = bT->up();
	axes[5] = bT->forward();
	for (size_t i = 0; i < 9; ++i)
		axes[6 + i] = cross(axes[i / 3], axes[i % 3 + 3]);

	vec3 tests[7];
	tests[0] = vec3(bT->renderMatrix(false) * vec4(bOBB.center, 1) - aT->renderMatrix(false) * vec4(aOBB.center, 1));
	for (size_t i = 0; i < 6; ++i)
		tests[i + 1] = axes[i] * (i < 3 ? (aOBB.halfSize[i % 3] * aT->scale[i % 3]) : (bOBB.halfSize[i % 3] * bT->scale[i % 3]));

	for (size_t i = 0; i < 15; ++i)
	{
		float abDot = 0.0f;
		float TDot = abs(dot(tests[0], axes[i]));
		for (size_t j = 1; j < 7; ++j)
			abDot += abs(dot(tests[j], axes[i]));
		if (TDot > abDot)
		{
			result.collider = NULL;
			result.normal = vec3(0, 0, 0);
			result.depth = FLT_MAX;
			return;
		}
		else if (abDot - TDot < result.depth)
		{
			result.collider = b;
			result.normal = axes[i];
			if (dot(tests[0], axes[i]) < 0)
				result.normal *= -1;
			result.depth = abDot - TDot;
		}
	}
}

Collision Collider::checkCollision(Collider* other)
{
	Collision result = { NULL, vec3(0, 0, 0), FLT_MAX };
	if (this == other)
		return result;
	//if (!sphereCheck(this, other)) // not scale compliant yet
	//return result;
	satCheck(this, other, result);
	return result;
}

Collision Collider::checkCollision(GameObject* other)
{
	Collision result = { NULL, vec3(0, 0, 0), FLT_MAX };
	if (other == gameObject)
		return result;
	Collider* otherCollider = other->getComponent<Collider>();
	if (otherCollider == NULL)
		return result;
	result = checkCollision(otherCollider);
	return result;
}

void Collider::reTree()
{
	while (bins.size() > 0)
	{
		bins.back()->removeCollider(this);
		bins.pop_back();
	}

	bins = tree.addCollider(this);
}
